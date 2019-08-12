#ifndef __CHIPINFOHWIO_H__
#define __CHIPINFOHWIO_H__
/*
===========================================================================
*/
/**
  @file ChipInfoHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    TCSR_TCSR_REGS

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

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/chipinfo/src/ChipinfoHWIO.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                           (CORE_TOP_CSR_BASE      + 0x00037000)

#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_ADDR                                                                            (TCSR_TCSR_REGS_REG_BASE      + 0x00000100)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_RMSK                                                                               0x10001
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_IN          \
        in_dword_masked(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_ADDR, HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_RMSK)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_INM(m)      \
        in_dword_masked(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_ADDR, m)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_OUT(v)      \
        out_dword(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_ADDR,v)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_ADDR,m,v,HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_IN)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_BIMC_CFG_QRIB_XPU2_NSEN_INIT_BMSK                                                  0x10000
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_BIMC_CFG_QRIB_XPU2_NSEN_INIT_SHFT                                                     0x10
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_BIMC_CFG_QRIB_XPU2_EN_TZ_BMSK                                                          0x1
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_INIT_BIMC_CFG_QRIB_XPU2_EN_TZ_SHFT                                                          0x0

#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_ADDR                                                                           (TCSR_TCSR_REGS_REG_BASE      + 0x00000104)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_RMSK                                                                              0x10001
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_IN          \
        in_dword_masked(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_ADDR, HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_RMSK)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_INM(m)      \
        in_dword_masked(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_ADDR, m)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_OUT(v)      \
        out_dword(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_ADDR,v)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_ADDR,m,v,HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_IN)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_BIMC_QXS0_QRIB_XPU2_NSEN_INIT_BMSK                                                0x10000
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_BIMC_QXS0_QRIB_XPU2_NSEN_INIT_SHFT                                                   0x10
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_BIMC_QXS0_QRIB_XPU2_EN_TZ_BMSK                                                        0x1
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_INIT_BIMC_QXS0_QRIB_XPU2_EN_TZ_SHFT                                                        0x0

#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_ADDR                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x00000380)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_RMSK                                                                       0x10001
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_ADDR, HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_RMSK)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_ADDR, m)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_ADDR,v)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_ADDR,m,v,HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_IN)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_MMSS_VENUS0_VBIF_QRIB_XPU2_NSEN_INIT_BMSK                                  0x10000
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_MMSS_VENUS0_VBIF_QRIB_XPU2_NSEN_INIT_SHFT                                     0x10
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_MMSS_VENUS0_VBIF_QRIB_XPU2_EN_TZ_BMSK                                          0x1
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_INIT_MMSS_VENUS0_VBIF_QRIB_XPU2_EN_TZ_SHFT                                          0x0

#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_ADDR                                                                 (TCSR_TCSR_REGS_REG_BASE      + 0x00000384)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_RMSK                                                                    0x10001
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_ADDR, HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_RMSK)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_ADDR, m)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_ADDR,v)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_ADDR,m,v,HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_IN)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_MMSS_VENUS0_WRAPPER_QRIB_XPU2_NSEN_INIT_BMSK                            0x10000
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_MMSS_VENUS0_WRAPPER_QRIB_XPU2_NSEN_INIT_SHFT                               0x10
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_MMSS_VENUS0_WRAPPER_QRIB_XPU2_EN_TZ_BMSK                                    0x1
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_INIT_MMSS_VENUS0_WRAPPER_QRIB_XPU2_EN_TZ_SHFT                                    0x0

#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_ADDR                                                                             (TCSR_TCSR_REGS_REG_BASE      + 0x00001000)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_RMSK                                                                             0xffffffff
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_IN          \
        in_dword_masked(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_ADDR, HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_RMSK)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_INM(m)      \
        in_dword_masked(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_ADDR, m)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_OUT(v)      \
        out_dword(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_ADDR,v)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_ADDR,m,v,HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_IN)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_BIMC_CFG_QRIB_XPU2_ACR_BMSK                                                      0xffffffff
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_ACR_BIMC_CFG_QRIB_XPU2_ACR_SHFT                                                             0x0

#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_ADDR                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x00001004)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_RMSK                                                                        0x10001
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_IN          \
        in_dword_masked(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_ADDR, HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_RMSK)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_INM(m)      \
        in_dword_masked(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_ADDR, m)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_OUT(v)      \
        out_dword(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_ADDR,v)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_ADDR,m,v,HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_IN)
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_BMSK                                         0x10000
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_SHFT                                            0x10
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_EN_HV_BMSK                                       0x1
#define HWIO_TCSR_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_BIMC_CFG_QRIB_XPU2_VMIDEN_INIT_EN_HV_SHFT                                       0x0

#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_ADDR                                                                            (TCSR_TCSR_REGS_REG_BASE      + 0x00001008)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_RMSK                                                                            0xffffffff
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_IN          \
        in_dword_masked(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_ADDR, HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_RMSK)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_INM(m)      \
        in_dword_masked(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_ADDR, m)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_OUT(v)      \
        out_dword(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_ADDR,v)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_ADDR,m,v,HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_IN)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_BIMC_QXS0_QRIB_XPU2_ACR_BMSK                                                    0xffffffff
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_ACR_BIMC_QXS0_QRIB_XPU2_ACR_SHFT                                                           0x0

#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_ADDR                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x0000100c)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_RMSK                                                                       0x10001
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_IN          \
        in_dword_masked(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_ADDR, HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_RMSK)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_INM(m)      \
        in_dword_masked(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_ADDR, m)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_OUT(v)      \
        out_dword(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_ADDR,v)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_ADDR,m,v,HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_IN)
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_BMSK                                       0x10000
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_SHFT                                          0x10
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_EN_HV_BMSK                                     0x1
#define HWIO_TCSR_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_BIMC_QXS0_QRIB_XPU2_VMIDEN_INIT_EN_HV_SHFT                                     0x0

#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_ADDR                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x00001380)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_RMSK                                                                     0xffffffff
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_ADDR, HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_RMSK)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_ADDR, m)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_ADDR,v)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_ADDR,m,v,HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_IN)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_BMSK                                      0xffffffff
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_MMSS_VENUS0_VBIF_QRIB_XPU2_ACR_SHFT                                             0x0

#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_ADDR                                                             (TCSR_TCSR_REGS_REG_BASE      + 0x00001384)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_RMSK                                                                0x10001
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_ADDR, HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_RMSK)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_ADDR, m)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_ADDR,v)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_ADDR,m,v,HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_IN)
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_BMSK                         0x10000
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_SHFT                            0x10
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_EN_HV_BMSK                       0x1
#define HWIO_TCSR_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_MMSS_VENUS0_VBIF_QRIB_XPU2_VMIDEN_INIT_EN_HV_SHFT                       0x0

#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_ADDR                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00001390)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_RMSK                                                                  0xffffffff
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_ADDR, HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_RMSK)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_ADDR, m)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_ADDR,v)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_ADDR,m,v,HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_IN)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_BMSK                                0xffffffff
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_ACR_SHFT                                       0x0

#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_ADDR                                                          (TCSR_TCSR_REGS_REG_BASE      + 0x00001394)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_RMSK                                                             0x10001
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_ADDR, HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_RMSK)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_ADDR, m)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_ADDR,v)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_ADDR,m,v,HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_IN)
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_BMSK                   0x10000
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_SHFT                      0x10
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_EN_HV_BMSK                 0x1
#define HWIO_TCSR_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_MMSS_VENUS0_WRAPPER_QRIB_XPU2_VMIDEN_INIT_EN_HV_SHFT                 0x0

#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ADDR                                                                              (TCSR_TCSR_REGS_REG_BASE      + 0x00002000)
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_RMSK                                                                               0x7fffaff
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_IN          \
        in_dword_masked(HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ADDR, HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_RMSK)
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ADDR, m)
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_BLSP1_XPU2_NON_SEC_INTR_BMSK                                                       0x4000000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_BLSP1_XPU2_NON_SEC_INTR_SHFT                                                            0x1a
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SMMU_CFG_XPU2_NON_SEC_INTR_BMSK                                                    0x2000000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SMMU_CFG_XPU2_NON_SEC_INTR_SHFT                                                         0x19
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SMMU_QDSP_TBU_XPU2_NON_SEC_INTR_BMSK                                               0x1000000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SMMU_QDSP_TBU_XPU2_NON_SEC_INTR_SHFT                                                    0x18
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SMMU_GPS_TBU_XPU2_NON_SEC_INTR_BMSK                                                 0x800000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SMMU_GPS_TBU_XPU2_NON_SEC_INTR_SHFT                                                     0x17
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SMMU_DIME_TBU_XPU2_NON_SEC_INTR_BMSK                                                0x400000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SMMU_DIME_TBU_XPU2_NON_SEC_INTR_SHFT                                                    0x16
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_RPM_CFG_XPU2_NON_SEC_INTR_BMSK                                                      0x200000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_RPM_CFG_XPU2_NON_SEC_INTR_SHFT                                                          0x15
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_PRNG_CFG_XPU2_NON_SEC_INTR_BMSK                                                     0x100000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_PRNG_CFG_XPU2_NON_SEC_INTR_SHFT                                                         0x14
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_PCNOC_CFG_XPU2_NON_SEC_INTR_BMSK                                                     0x80000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_PCNOC_CFG_XPU2_NON_SEC_INTR_SHFT                                                        0x13
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SNOC_CFG_XPU2_NON_SEC_INTR_BMSK                                                      0x40000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SNOC_CFG_XPU2_NON_SEC_INTR_SHFT                                                         0x12
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_OCIMEM_XPU2_NON_SEC_INTR_BMSK                                                        0x20000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_OCIMEM_XPU2_NON_SEC_INTR_SHFT                                                           0x11
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SEC_CTRL_XPU2_NON_SEC_INTR_BMSK                                                      0x10000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_SEC_CTRL_XPU2_NON_SEC_INTR_SHFT                                                         0x10
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_DEHR_XPU2_NON_SEC_INTR_BMSK                                                           0x8000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_DEHR_XPU2_NON_SEC_INTR_SHFT                                                              0xf
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_MPM_XPU2_NON_SEC_INTR_BMSK                                                            0x4000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_MPM_XPU2_NON_SEC_INTR_SHFT                                                               0xe
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_BOOTROM_XPU2_NON_SEC_INTR_BMSK                                                        0x2000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_BOOTROM_XPU2_NON_SEC_INTR_SHFT                                                           0xd
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_CRYPTO0_BAM_XPU2_NON_SEC_INTR_BMSK                                                    0x1000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_CRYPTO0_BAM_XPU2_NON_SEC_INTR_SHFT                                                       0xc
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_TCSR_XPU2_NON_SEC_INTR_BMSK                                                            0x800
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_TCSR_XPU2_NON_SEC_INTR_SHFT                                                              0xb
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_TLMM_XPU2_NON_SEC_INTR_BMSK                                                            0x200
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_TLMM_XPU2_NON_SEC_INTR_SHFT                                                              0x9
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_PRONTO_XPU2_NON_SEC_INTR_BMSK                                                           0x80
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_PRONTO_XPU2_NON_SEC_INTR_SHFT                                                            0x7
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_PMIC_ARB_XPU2_NON_SEC_INTR_BMSK                                                         0x40
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_PMIC_ARB_XPU2_NON_SEC_INTR_SHFT                                                          0x6
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_BIMC_CH0_XPU2_NON_SEC_INTR_BMSK                                                         0x20
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_BIMC_CH0_XPU2_NON_SEC_INTR_SHFT                                                          0x5
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_BIMC_CFG_XPU2_NON_SEC_INTR_BMSK                                                         0x10
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_BIMC_CFG_XPU2_NON_SEC_INTR_SHFT                                                          0x4
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_GCC_XPU2_NON_SEC_INTR_BMSK                                                               0x8
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_GCC_XPU2_NON_SEC_INTR_SHFT                                                               0x3
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_VENUS0_XPU2_NON_SEC_INTR_BMSK                                                            0x4
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_VENUS0_XPU2_NON_SEC_INTR_SHFT                                                            0x2
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_RPM_XPU2_NON_SEC_INTR_BMSK                                                               0x2
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_RPM_XPU2_NON_SEC_INTR_SHFT                                                               0x1
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_MSS_XPU2_NON_SEC_INTR_BMSK                                                               0x1
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_MSS_XPU2_NON_SEC_INTR_SHFT                                                               0x0

#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_ADDR                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x00002040)
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_RMSK                                                                        0x7fffaff
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_ADDR, HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_RMSK)
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_ADDR, m)
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_ADDR,v)
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_ADDR,m,v,HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_IN)
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_BLSP1_XPU2_NON_SEC_INTR_ENABLE_BMSK                                         0x4000000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_BLSP1_XPU2_NON_SEC_INTR_ENABLE_SHFT                                              0x1a
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SMMU_CFG_XPU2_NON_SEC_INTR_ENABLE_BMSK                                      0x2000000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SMMU_CFG_XPU2_NON_SEC_INTR_ENABLE_SHFT                                           0x19
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SMMU_QDSP_TBU_XPU2_NON_SEC_INTR_ENABLE_BMSK                                 0x1000000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SMMU_QDSP_TBU_XPU2_NON_SEC_INTR_ENABLE_SHFT                                      0x18
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SMMU_GPS_TBU_XPU2_NON_SEC_INTR_ENABLE_BMSK                                   0x800000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SMMU_GPS_TBU_XPU2_NON_SEC_INTR_ENABLE_SHFT                                       0x17
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SMMU_DIME_TBU_XPU2_NON_SEC_INTR_ENABLE_BMSK                                  0x400000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SMMU_DIME_TBU_XPU2_NON_SEC_INTR_ENABLE_SHFT                                      0x16
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_RPM_CFG_XPU2_NON_SEC_INTR_ENABLE_BMSK                                        0x200000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_RPM_CFG_XPU2_NON_SEC_INTR_ENABLE_SHFT                                            0x15
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_PRNG_CFG_XPU2_NON_SEC_INTR_ENABLE_BMSK                                       0x100000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_PRNG_CFG_XPU2_NON_SEC_INTR_ENABLE_SHFT                                           0x14
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_PCNOC_CFG_XPU2_NON_SEC_INTR_ENABLE_BMSK                                       0x80000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_PCNOC_CFG_XPU2_NON_SEC_INTR_ENABLE_SHFT                                          0x13
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SNOC_CFG_XPU2_NON_SEC_INTR_ENABLE_BMSK                                        0x40000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SNOC_CFG_XPU2_NON_SEC_INTR_ENABLE_SHFT                                           0x12
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_OCIMEM_XPU2_NON_SEC_INTR_ENABLE_BMSK                                          0x20000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_OCIMEM_XPU2_NON_SEC_INTR_ENABLE_SHFT                                             0x11
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SEC_CTRL_XPU2_NON_SEC_INTR_ENABLE_BMSK                                        0x10000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_SEC_CTRL_XPU2_NON_SEC_INTR_ENABLE_SHFT                                           0x10
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_DEHR_XPU2_NON_SEC_INTR_ENABLE_BMSK                                             0x8000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_DEHR_XPU2_NON_SEC_INTR_ENABLE_SHFT                                                0xf
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_MPM_XPU2_NON_SEC_INTR_ENABLE_BMSK                                              0x4000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_MPM_XPU2_NON_SEC_INTR_ENABLE_SHFT                                                 0xe
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_BOOTROM_XPU2_NON_SEC_INTR_ENABLE_BMSK                                          0x2000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_BOOTROM_XPU2_NON_SEC_INTR_ENABLE_SHFT                                             0xd
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_CRYPTO0_BAM_XPU2_NON_SEC_INTR_ENABLE_BMSK                                      0x1000
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_CRYPTO0_BAM_XPU2_NON_SEC_INTR_ENABLE_SHFT                                         0xc
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_TCSR_XPU2_NON_SEC_INTR_ENABLE_BMSK                                              0x800
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_TCSR_XPU2_NON_SEC_INTR_ENABLE_SHFT                                                0xb
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_TLMM_XPU2_NON_SEC_INTR_ENABLE_BMSK                                              0x200
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_TLMM_XPU2_NON_SEC_INTR_ENABLE_SHFT                                                0x9
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_PRONTO_XPU2_NON_SEC_INTR_ENABLE_BMSK                                             0x80
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_PRONTO_XPU2_NON_SEC_INTR_ENABLE_SHFT                                              0x7
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_PMIC_ARB_XPU2_NON_SEC_INTR_ENABLE_BMSK                                           0x40
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_PMIC_ARB_XPU2_NON_SEC_INTR_ENABLE_SHFT                                            0x6
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_BIMC_CH0_XPU2_NON_SEC_INTR_ENABLE_BMSK                                           0x20
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_BIMC_CH0_XPU2_NON_SEC_INTR_ENABLE_SHFT                                            0x5
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_BIMC_CFG_XPU2_NON_SEC_INTR_ENABLE_BMSK                                           0x10
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_BIMC_CFG_XPU2_NON_SEC_INTR_ENABLE_SHFT                                            0x4
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_GCC_XPU2_NON_SEC_INTR_ENABLE_BMSK                                                 0x8
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_GCC_XPU2_NON_SEC_INTR_ENABLE_SHFT                                                 0x3
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_VENUS0_XPU2_NON_SEC_INTR_ENABLE_BMSK                                              0x4
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_VENUS0_XPU2_NON_SEC_INTR_ENABLE_SHFT                                              0x2
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_RPM_XPU2_NON_SEC_INTR_ENABLE_BMSK                                                 0x2
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_RPM_XPU2_NON_SEC_INTR_ENABLE_SHFT                                                 0x1
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_MSS_XPU2_NON_SEC_INTR_ENABLE_BMSK                                                 0x1
#define HWIO_TCSR_SS_XPU2_NON_SEC_INTR0_ENABLE_MSS_XPU2_NON_SEC_INTR_ENABLE_SHFT                                                 0x0

#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ADDR                                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x00002010)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_RMSK                                                                             0xf
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_IN          \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ADDR, HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_RMSK)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ADDR, m)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_DEHR_VMIDMT_CLIENT_NON_SEC_INTR_BMSK                                             0x8
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_DEHR_VMIDMT_CLIENT_NON_SEC_INTR_SHFT                                             0x3
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_CRYPTO0_VMIDMT_CLIENT_NON_SEC_INTR_BMSK                                          0x4
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_CRYPTO0_VMIDMT_CLIENT_NON_SEC_INTR_SHFT                                          0x2
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_BLSP_VMIDMT_CLIENT_NON_SEC_INTR_BMSK                                             0x2
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_BLSP_VMIDMT_CLIENT_NON_SEC_INTR_SHFT                                             0x1
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_RPM_VMIDMT_CLIENT_NON_SEC_INTR_BMSK                                              0x1
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_RPM_VMIDMT_CLIENT_NON_SEC_INTR_SHFT                                              0x0

#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_ADDR                                                               (TCSR_TCSR_REGS_REG_BASE      + 0x00002050)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_RMSK                                                                      0xf
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_ADDR, HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_RMSK)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_ADDR, m)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_ADDR,v)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_ADDR,m,v,HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_IN)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_DEHR_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_BMSK                               0x8
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_DEHR_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_SHFT                               0x3
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_CRYPTO0_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_BMSK                            0x4
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_CRYPTO0_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_SHFT                            0x2
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_BLSP_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_BMSK                               0x2
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_BLSP_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_SHFT                               0x1
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_RPM_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_BMSK                                0x1
#define HWIO_TCSR_SS_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_RPM_VMIDMT_CLIENT_NON_SEC_INTR_ENABLE_SHFT                                0x0

#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ADDR                                                                         (TCSR_TCSR_REGS_REG_BASE      + 0x00002090)
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_RMSK                                                                                0xf
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_IN          \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ADDR, HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_RMSK)
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ADDR, m)
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_DEHR_VMIDMT_CFG_NON_SEC_INTR_BMSK                                                   0x8
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_DEHR_VMIDMT_CFG_NON_SEC_INTR_SHFT                                                   0x3
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_CRYPTO0_VMIDMT_CFG_NON_SEC_INTR_BMSK                                                0x4
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_CRYPTO0_VMIDMT_CFG_NON_SEC_INTR_SHFT                                                0x2
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_BLSP_VMIDMT_CFG_NON_SEC_INTR_BMSK                                                   0x2
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_BLSP_VMIDMT_CFG_NON_SEC_INTR_SHFT                                                   0x1
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_RPM_VMIDMT_CFG_NON_SEC_INTR_BMSK                                                    0x1
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_RPM_VMIDMT_CFG_NON_SEC_INTR_SHFT                                                    0x0

#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_ADDR                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x000020d0)
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_RMSK                                                                         0xf
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_ADDR, HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_RMSK)
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_ADDR, m)
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_ADDR,v)
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_ADDR,m,v,HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_IN)
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_DEHR_VMIDMT_CFG_NON_SEC_INTR_ENABLE_BMSK                                     0x8
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_DEHR_VMIDMT_CFG_NON_SEC_INTR_ENABLE_SHFT                                     0x3
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_CRYPTO0_VMIDMT_CFG_NON_SEC_INTR_ENABLE_BMSK                                  0x4
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_CRYPTO0_VMIDMT_CFG_NON_SEC_INTR_ENABLE_SHFT                                  0x2
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_BLSP_VMIDMT_CFG_NON_SEC_INTR_ENABLE_BMSK                                     0x2
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_BLSP_VMIDMT_CFG_NON_SEC_INTR_ENABLE_SHFT                                     0x1
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_RPM_VMIDMT_CFG_NON_SEC_INTR_ENABLE_BMSK                                      0x1
#define HWIO_TCSR_SS_VMIDMT_CFG_NON_SEC_INTR_ENABLE_RPM_VMIDMT_CFG_NON_SEC_INTR_ENABLE_SHFT                                      0x0

#define HWIO_TCSR_MUTEX_REG_RESET_ADDR                                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x00002800)
#define HWIO_TCSR_MUTEX_REG_RESET_RMSK                                                                                           0x1
#define HWIO_TCSR_MUTEX_REG_RESET_IN          \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_RESET_ADDR, HWIO_TCSR_MUTEX_REG_RESET_RMSK)
#define HWIO_TCSR_MUTEX_REG_RESET_INM(m)      \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_RESET_ADDR, m)
#define HWIO_TCSR_MUTEX_REG_RESET_OUT(v)      \
        out_dword(HWIO_TCSR_MUTEX_REG_RESET_ADDR,v)
#define HWIO_TCSR_MUTEX_REG_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MUTEX_REG_RESET_ADDR,m,v,HWIO_TCSR_MUTEX_REG_RESET_IN)
#define HWIO_TCSR_MUTEX_REG_RESET_MUTEX_RESET_BMSK                                                                               0x1
#define HWIO_TCSR_MUTEX_REG_RESET_MUTEX_RESET_SHFT                                                                               0x0

#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00002400)
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_RMSK                                                                                   0x7fffaff
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_IN          \
        in_dword_masked(HWIO_TCSR_SS_XPU2_SEC_INTR0_ADDR, HWIO_TCSR_SS_XPU2_SEC_INTR0_RMSK)
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_XPU2_SEC_INTR0_ADDR, m)
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_BLSP1_XPU2_SEC_INTR_BMSK                                                               0x4000000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_BLSP1_XPU2_SEC_INTR_SHFT                                                                    0x1a
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SMMU_CFG_XPU2_SEC_INTR_BMSK                                                            0x2000000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SMMU_CFG_XPU2_SEC_INTR_SHFT                                                                 0x19
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SMMU_QDSP_TBU_XPU2_SEC_INTR_BMSK                                                       0x1000000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SMMU_QDSP_TBU_XPU2_SEC_INTR_SHFT                                                            0x18
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SMMU_GPS_TBU_XPU2_SEC_INTR_BMSK                                                         0x800000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SMMU_GPS_TBU_XPU2_SEC_INTR_SHFT                                                             0x17
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SMMU_DIME_TBU_XPU2_SEC_INTR_BMSK                                                        0x400000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SMMU_DIME_TBU_XPU2_SEC_INTR_SHFT                                                            0x16
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_RPM_CFG_XPU2_SEC_INTR_BMSK                                                              0x200000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_RPM_CFG_XPU2_SEC_INTR_SHFT                                                                  0x15
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_PRNG_CFG_XPU2_SEC_INTR_BMSK                                                             0x100000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_PRNG_CFG_XPU2_SEC_INTR_SHFT                                                                 0x14
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_PCNOC_CFG_XPU2_SEC_INTR_BMSK                                                             0x80000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_PCNOC_CFG_XPU2_SEC_INTR_SHFT                                                                0x13
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SNOC_CFG_XPU2_SEC_INTR_BMSK                                                              0x40000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SNOC_CFG_XPU2_SEC_INTR_SHFT                                                                 0x12
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_OCIMEM_XPU2_SEC_INTR_BMSK                                                                0x20000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_OCIMEM_XPU2_SEC_INTR_SHFT                                                                   0x11
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SEC_CTRL_XPU2_SEC_INTR_BMSK                                                              0x10000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_SEC_CTRL_XPU2_SEC_INTR_SHFT                                                                 0x10
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_DEHR_XPU2_SEC_INTR_BMSK                                                                   0x8000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_DEHR_XPU2_SEC_INTR_SHFT                                                                      0xf
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_MPM_XPU2_SEC_INTR_BMSK                                                                    0x4000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_MPM_XPU2_SEC_INTR_SHFT                                                                       0xe
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_BOOTROM_XPU2_SEC_INTR_BMSK                                                                0x2000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_BOOTROM_XPU2_SEC_INTR_SHFT                                                                   0xd
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_CRYPTO0_BAM_XPU2_SEC_INTR_BMSK                                                            0x1000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_CRYPTO0_BAM_XPU2_SEC_INTR_SHFT                                                               0xc
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_TCSR_XPU2_SEC_INTR_BMSK                                                                    0x800
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_TCSR_XPU2_SEC_INTR_SHFT                                                                      0xb
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_TLMM_XPU2_SEC_INTR_BMSK                                                                    0x200
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_TLMM_XPU2_SEC_INTR_SHFT                                                                      0x9
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_PRONTO_XPU2_SEC_INTR_BMSK                                                                   0x80
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_PRONTO_XPU2_SEC_INTR_SHFT                                                                    0x7
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_PMIC_ARB_XPU2_SEC_INTR_BMSK                                                                 0x40
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_PMIC_ARB_XPU2_SEC_INTR_SHFT                                                                  0x6
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_BIMC_CH0_XPU2_SEC_INTR_BMSK                                                                 0x20
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_BIMC_CH0_XPU2_SEC_INTR_SHFT                                                                  0x5
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_BIMC_CFG_XPU2_SEC_INTR_BMSK                                                                 0x10
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_BIMC_CFG_XPU2_SEC_INTR_SHFT                                                                  0x4
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_GCC_XPU2_SEC_INTR_BMSK                                                                       0x8
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_GCC_XPU2_SEC_INTR_SHFT                                                                       0x3
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_VENUS0_XPU2_SEC_INTR_BMSK                                                                    0x4
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_VENUS0_XPU2_SEC_INTR_SHFT                                                                    0x2
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_RPM_XPU2_SEC_INTR_BMSK                                                                       0x2
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_RPM_XPU2_SEC_INTR_SHFT                                                                       0x1
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_MSS_XPU2_SEC_INTR_BMSK                                                                       0x1
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_MSS_XPU2_SEC_INTR_SHFT                                                                       0x0

#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_ADDR                                                                           (TCSR_TCSR_REGS_REG_BASE      + 0x00002440)
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_RMSK                                                                            0x7fffaff
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_ADDR, HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_RMSK)
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_ADDR, m)
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_ADDR,v)
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_ADDR,m,v,HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_IN)
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_BLSP1_XPU2_SEC_INTR_ENABLE_BMSK                                                 0x4000000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_BLSP1_XPU2_SEC_INTR_ENABLE_SHFT                                                      0x1a
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SMMU_CFG_XPU2_SEC_INTR_ENABLE_BMSK                                              0x2000000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SMMU_CFG_XPU2_SEC_INTR_ENABLE_SHFT                                                   0x19
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SMMU_QDSP_TBU_XPU2_SEC_INTR_ENABLE_BMSK                                         0x1000000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SMMU_QDSP_TBU_XPU2_SEC_INTR_ENABLE_SHFT                                              0x18
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SMMU_GPS_TBU_XPU2_SEC_INTR_ENABLE_BMSK                                           0x800000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SMMU_GPS_TBU_XPU2_SEC_INTR_ENABLE_SHFT                                               0x17
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SMMU_DIME_TBU_XPU2_SEC_INTR_ENABLE_BMSK                                          0x400000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SMMU_DIME_TBU_XPU2_SEC_INTR_ENABLE_SHFT                                              0x16
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_RPM_CFG_XPU2_SEC_INTR_ENABLE_BMSK                                                0x200000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_RPM_CFG_XPU2_SEC_INTR_ENABLE_SHFT                                                    0x15
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_PRNG_CFG_XPU2_SEC_INTR_ENABLE_BMSK                                               0x100000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_PRNG_CFG_XPU2_SEC_INTR_ENABLE_SHFT                                                   0x14
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_PCNOC_CFG_XPU2_SEC_INTR_ENABLE_BMSK                                               0x80000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_PCNOC_CFG_XPU2_SEC_INTR_ENABLE_SHFT                                                  0x13
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SNOC_CFG_XPU2_SEC_INTR_ENABLE_BMSK                                                0x40000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SNOC_CFG_XPU2_SEC_INTR_ENABLE_SHFT                                                   0x12
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_OCIMEM_XPU2_SEC_INTR_ENABLE_BMSK                                                  0x20000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_OCIMEM_XPU2_SEC_INTR_ENABLE_SHFT                                                     0x11
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SEC_CTRL_XPU2_SEC_INTR_ENABLE_BMSK                                                0x10000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_SEC_CTRL_XPU2_SEC_INTR_ENABLE_SHFT                                                   0x10
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_DEHR_XPU2_SEC_INTR_ENABLE_BMSK                                                     0x8000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_DEHR_XPU2_SEC_INTR_ENABLE_SHFT                                                        0xf
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_MPM_XPU2_SEC_INTR_ENABLE_BMSK                                                      0x4000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_MPM_XPU2_SEC_INTR_ENABLE_SHFT                                                         0xe
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_BOOTROM_XPU2_SEC_INTR_ENABLE_BMSK                                                  0x2000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_BOOTROM_XPU2_SEC_INTR_ENABLE_SHFT                                                     0xd
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_CRYPTO0_BAM_XPU2_SEC_INTR_ENABLE_BMSK                                              0x1000
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_CRYPTO0_BAM_XPU2_SEC_INTR_ENABLE_SHFT                                                 0xc
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_TCSR_XPU2_SEC_INTR_ENABLE_BMSK                                                      0x800
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_TCSR_XPU2_SEC_INTR_ENABLE_SHFT                                                        0xb
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_TLMM_XPU2_SEC_INTR_ENABLE_BMSK                                                      0x200
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_TLMM_XPU2_SEC_INTR_ENABLE_SHFT                                                        0x9
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_PRONTO_XPU2_SEC_INTR_ENABLE_BMSK                                                     0x80
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_PRONTO_XPU2_SEC_INTR_ENABLE_SHFT                                                      0x7
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_PMIC_ARB_XPU2_SEC_INTR_ENABLE_BMSK                                                   0x40
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_PMIC_ARB_XPU2_SEC_INTR_ENABLE_SHFT                                                    0x6
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_BIMC_CH0_XPU2_SEC_INTR_ENABLE_BMSK                                                   0x20
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_BIMC_CH0_XPU2_SEC_INTR_ENABLE_SHFT                                                    0x5
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_BIMC_CFG_XPU2_SEC_INTR_ENABLE_BMSK                                                   0x10
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_BIMC_CFG_XPU2_SEC_INTR_ENABLE_SHFT                                                    0x4
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_GCC_XPU2_SEC_INTR_ENABLE_BMSK                                                         0x8
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_GCC_XPU2_SEC_INTR_ENABLE_SHFT                                                         0x3
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_VENUS0_XPU2_SEC_INTR_ENABLE_BMSK                                                      0x4
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_VENUS0_XPU2_SEC_INTR_ENABLE_SHFT                                                      0x2
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_RPM_XPU2_SEC_INTR_ENABLE_BMSK                                                         0x2
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_RPM_XPU2_SEC_INTR_ENABLE_SHFT                                                         0x1
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_MSS_XPU2_SEC_INTR_ENABLE_BMSK                                                         0x1
#define HWIO_TCSR_SS_XPU2_SEC_INTR0_ENABLE_MSS_XPU2_SEC_INTR_ENABLE_SHFT                                                         0x0

#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ADDR                                                                          (TCSR_TCSR_REGS_REG_BASE      + 0x00002410)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_RMSK                                                                                 0xf
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_IN          \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ADDR, HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_RMSK)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ADDR, m)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_DEHR_VMIDMT_CLIENT_SEC_INTR_BMSK                                                     0x8
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_DEHR_VMIDMT_CLIENT_SEC_INTR_SHFT                                                     0x3
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_CRYPTO0_VMIDMT_CLIENT_SEC_INTR_BMSK                                                  0x4
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_CRYPTO0_VMIDMT_CLIENT_SEC_INTR_SHFT                                                  0x2
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_BLSP_VMIDMT_CLIENT_SEC_INTR_BMSK                                                     0x2
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_BLSP_VMIDMT_CLIENT_SEC_INTR_SHFT                                                     0x1
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_RPM_VMIDMT_CLIENT_SEC_INTR_BMSK                                                      0x1
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_RPM_VMIDMT_CLIENT_SEC_INTR_SHFT                                                      0x0

#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_ADDR                                                                   (TCSR_TCSR_REGS_REG_BASE      + 0x00002450)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_RMSK                                                                          0xf
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_ADDR, HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_RMSK)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_ADDR, m)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_ADDR,v)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_ADDR,m,v,HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_IN)
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_DEHR_VMIDMT_CLIENT_SEC_INTR_ENABLE_BMSK                                       0x8
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_DEHR_VMIDMT_CLIENT_SEC_INTR_ENABLE_SHFT                                       0x3
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_CRYPTO0_VMIDMT_CLIENT_SEC_INTR_ENABLE_BMSK                                    0x4
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_CRYPTO0_VMIDMT_CLIENT_SEC_INTR_ENABLE_SHFT                                    0x2
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_BLSP_VMIDMT_CLIENT_SEC_INTR_ENABLE_BMSK                                       0x2
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_BLSP_VMIDMT_CLIENT_SEC_INTR_ENABLE_SHFT                                       0x1
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_RPM_VMIDMT_CLIENT_SEC_INTR_ENABLE_BMSK                                        0x1
#define HWIO_TCSR_SS_VMIDMT_CLIENT_SEC_INTR_ENABLE_RPM_VMIDMT_CLIENT_SEC_INTR_ENABLE_SHFT                                        0x0

#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ADDR                                                                             (TCSR_TCSR_REGS_REG_BASE      + 0x00002490)
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_RMSK                                                                                    0xf
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_IN          \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ADDR, HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_RMSK)
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ADDR, m)
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_DEHR_VMIDMT_CFG_SEC_INTR_BMSK                                                           0x8
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_DEHR_VMIDMT_CFG_SEC_INTR_SHFT                                                           0x3
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_CRYPTO0_VMIDMT_CFG_SEC_INTR_BMSK                                                        0x4
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_CRYPTO0_VMIDMT_CFG_SEC_INTR_SHFT                                                        0x2
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_BLSP_VMIDMT_CFG_SEC_INTR_BMSK                                                           0x2
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_BLSP_VMIDMT_CFG_SEC_INTR_SHFT                                                           0x1
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_RPM_VMIDMT_CFG_SEC_INTR_BMSK                                                            0x1
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_RPM_VMIDMT_CFG_SEC_INTR_SHFT                                                            0x0

#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_ADDR                                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x000024d0)
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_RMSK                                                                             0xf
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_ADDR, HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_RMSK)
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_ADDR, m)
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_ADDR,v)
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_ADDR,m,v,HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_IN)
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_DEHR_VMIDMT_CFG_SEC_INTR_ENABLE_BMSK                                             0x8
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_DEHR_VMIDMT_CFG_SEC_INTR_ENABLE_SHFT                                             0x3
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_CRYPTO0_VMIDMT_CFG_SEC_INTR_ENABLE_BMSK                                          0x4
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_CRYPTO0_VMIDMT_CFG_SEC_INTR_ENABLE_SHFT                                          0x2
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_BLSP_VMIDMT_CFG_SEC_INTR_ENABLE_BMSK                                             0x2
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_BLSP_VMIDMT_CFG_SEC_INTR_ENABLE_SHFT                                             0x1
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_RPM_VMIDMT_CFG_SEC_INTR_ENABLE_BMSK                                              0x1
#define HWIO_TCSR_SS_VMIDMT_CFG_SEC_INTR_ENABLE_RPM_VMIDMT_CFG_SEC_INTR_ENABLE_SHFT                                              0x0

#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00003000)
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_RMSK                                                                                   0x7fffafe
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_IN          \
        in_dword_masked(HWIO_TCSR_SS_XPU2_MSA_INTR0_ADDR, HWIO_TCSR_SS_XPU2_MSA_INTR0_RMSK)
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_XPU2_MSA_INTR0_ADDR, m)
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_BLSP1_XPU2_MSA_INTR_BMSK                                                               0x4000000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_BLSP1_XPU2_MSA_INTR_SHFT                                                                    0x1a
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SMMU_CFG_XPU2_MSA_INTR_BMSK                                                            0x2000000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SMMU_CFG_XPU2_MSA_INTR_SHFT                                                                 0x19
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SMMU_QDSP_TBU_XPU2_MSA_INTR_BMSK                                                       0x1000000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SMMU_QDSP_TBU_XPU2_MSA_INTR_SHFT                                                            0x18
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SMMU_GPS_TBU_XPU2_MSA_INTR_BMSK                                                         0x800000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SMMU_GPS_TBU_XPU2_MSA_INTR_SHFT                                                             0x17
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SMMU_DIME_TBU_XPU2_MSA_INTR_BMSK                                                        0x400000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SMMU_DIME_TBU_XPU2_MSA_INTR_SHFT                                                            0x16
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_RPM_CFG_XPU2_MSA_INTR_BMSK                                                              0x200000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_RPM_CFG_XPU2_MSA_INTR_SHFT                                                                  0x15
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_PRNG_CFG_XPU2_MSA_INTR_BMSK                                                             0x100000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_PRNG_CFG_XPU2_MSA_INTR_SHFT                                                                 0x14
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_PCNOC_CFG_XPU2_MSA_INTR_BMSK                                                             0x80000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_PCNOC_CFG_XPU2_MSA_INTR_SHFT                                                                0x13
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SNOC_CFG_XPU2_MSA_INTR_BMSK                                                              0x40000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SNOC_CFG_XPU2_MSA_INTR_SHFT                                                                 0x12
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_OCIMEM_XPU2_MSA_INTR_BMSK                                                                0x20000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_OCIMEM_XPU2_MSA_INTR_SHFT                                                                   0x11
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SEC_CTRL_XPU2_MSA_INTR_BMSK                                                              0x10000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_SEC_CTRL_XPU2_MSA_INTR_SHFT                                                                 0x10
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_DEHR_XPU2_MSA_INTR_BMSK                                                                   0x8000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_DEHR_XPU2_MSA_INTR_SHFT                                                                      0xf
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_MPM_XPU2_MSA_INTR_BMSK                                                                    0x4000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_MPM_XPU2_MSA_INTR_SHFT                                                                       0xe
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_BOOTROM_XPU2_MSA_INTR_BMSK                                                                0x2000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_BOOTROM_XPU2_MSA_INTR_SHFT                                                                   0xd
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_CRYPTO0_BAM_XPU2_MSA_INTR_BMSK                                                            0x1000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_CRYPTO0_BAM_XPU2_MSA_INTR_SHFT                                                               0xc
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_TCSR_XPU2_MSA_INTR_BMSK                                                                    0x800
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_TCSR_XPU2_MSA_INTR_SHFT                                                                      0xb
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_TLMM_XPU2_MSA_INTR_BMSK                                                                    0x200
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_TLMM_XPU2_MSA_INTR_SHFT                                                                      0x9
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_PRONTO_XPU2_MSA_INTR_BMSK                                                                   0x80
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_PRONTO_XPU2_MSA_INTR_SHFT                                                                    0x7
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_PMIC_ARB_XPU2_MSA_INTR_BMSK                                                                 0x40
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_PMIC_ARB_XPU2_MSA_INTR_SHFT                                                                  0x6
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_BIMC_CH0_XPU2_MSA_INTR_BMSK                                                                 0x20
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_BIMC_CH0_XPU2_MSA_INTR_SHFT                                                                  0x5
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_BIMC_CFG_XPU2_MSA_INTR_BMSK                                                                 0x10
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_BIMC_CFG_XPU2_MSA_INTR_SHFT                                                                  0x4
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_GCC_XPU2_MSA_INTR_BMSK                                                                       0x8
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_GCC_XPU2_MSA_INTR_SHFT                                                                       0x3
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_VENUS0_XPU2_MSA_INTR_BMSK                                                                    0x4
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_VENUS0_XPU2_MSA_INTR_SHFT                                                                    0x2
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_RPM_XPU2_MSA_INTR_BMSK                                                                       0x2
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_RPM_XPU2_MSA_INTR_SHFT                                                                       0x1

#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_ADDR                                                                           (TCSR_TCSR_REGS_REG_BASE      + 0x00003010)
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_RMSK                                                                            0x7fffafe
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_ADDR, HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_RMSK)
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_ADDR, m)
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_ADDR,v)
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_ADDR,m,v,HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_IN)
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_BLSP1_XPU2_MSA_INTR_ENABLE_BMSK                                                 0x4000000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_BLSP1_XPU2_MSA_INTR_ENABLE_SHFT                                                      0x1a
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SMMU_CFG_XPU2_MSA_INTR_ENABLE_BMSK                                              0x2000000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SMMU_CFG_XPU2_MSA_INTR_ENABLE_SHFT                                                   0x19
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SMMU_QDSP_TBU_XPU2_MSA_INTR_ENABLE_BMSK                                         0x1000000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SMMU_QDSP_TBU_XPU2_MSA_INTR_ENABLE_SHFT                                              0x18
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SMMU_GPS_TBU_XPU2_MSA_INTR_ENABLE_BMSK                                           0x800000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SMMU_GPS_TBU_XPU2_MSA_INTR_ENABLE_SHFT                                               0x17
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SMMU_DIME_TBU_XPU2_MSA_INTR_ENABLE_BMSK                                          0x400000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SMMU_DIME_TBU_XPU2_MSA_INTR_ENABLE_SHFT                                              0x16
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_RPM_CFG_XPU2_MSA_INTR_ENABLE_BMSK                                                0x200000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_RPM_CFG_XPU2_MSA_INTR_ENABLE_SHFT                                                    0x15
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_PRNG_CFG_XPU2_MSA_INTR_ENABLE_BMSK                                               0x100000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_PRNG_CFG_XPU2_MSA_INTR_ENABLE_SHFT                                                   0x14
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_PCNOC_CFG_XPU2_MSA_INTR_ENABLE_BMSK                                               0x80000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_PCNOC_CFG_XPU2_MSA_INTR_ENABLE_SHFT                                                  0x13
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SNOC_CFG_XPU2_MSA_INTR_ENABLE_BMSK                                                0x40000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SNOC_CFG_XPU2_MSA_INTR_ENABLE_SHFT                                                   0x12
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_OCIMEM_XPU2_MSA_INTR_ENABLE_BMSK                                                  0x20000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_OCIMEM_XPU2_MSA_INTR_ENABLE_SHFT                                                     0x11
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SEC_CTRL_XPU2_MSA_INTR_ENABLE_BMSK                                                0x10000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_SEC_CTRL_XPU2_MSA_INTR_ENABLE_SHFT                                                   0x10
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_DEHR_XPU2_MSA_INTR_ENABLE_BMSK                                                     0x8000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_DEHR_XPU2_MSA_INTR_ENABLE_SHFT                                                        0xf
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_MPM_XPU2_MSA_INTR_ENABLE_BMSK                                                      0x4000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_MPM_XPU2_MSA_INTR_ENABLE_SHFT                                                         0xe
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_BOOTROM_XPU2_MSA_INTR_ENABLE_BMSK                                                  0x2000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_BOOTROM_XPU2_MSA_INTR_ENABLE_SHFT                                                     0xd
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_CRYPTO0_BAM_XPU2_MSA_INTR_ENABLE_BMSK                                              0x1000
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_CRYPTO0_BAM_XPU2_MSA_INTR_ENABLE_SHFT                                                 0xc
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_TCSR_XPU2_MSA_INTR_ENABLE_BMSK                                                      0x800
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_TCSR_XPU2_MSA_INTR_ENABLE_SHFT                                                        0xb
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_TLMM_XPU2_MSA_INTR_ENABLE_BMSK                                                      0x200
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_TLMM_XPU2_MSA_INTR_ENABLE_SHFT                                                        0x9
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_PRONTO_XPU2_MSA_INTR_ENABLE_BMSK                                                     0x80
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_PRONTO_XPU2_MSA_INTR_ENABLE_SHFT                                                      0x7
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_PMIC_ARB_XPU2_MSA_INTR_ENABLE_BMSK                                                   0x40
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_PMIC_ARB_XPU2_MSA_INTR_ENABLE_SHFT                                                    0x6
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_BIMC_CH0_XPU2_MSA_INTR_ENABLE_BMSK                                                   0x20
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_BIMC_CH0_XPU2_MSA_INTR_ENABLE_SHFT                                                    0x5
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_BIMC_CFG_XPU2_MSA_INTR_ENABLE_BMSK                                                   0x10
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_BIMC_CFG_XPU2_MSA_INTR_ENABLE_SHFT                                                    0x4
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_GCC_XPU2_MSA_INTR_ENABLE_BMSK                                                         0x8
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_GCC_XPU2_MSA_INTR_ENABLE_SHFT                                                         0x3
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_VENUS0_XPU2_MSA_INTR_ENABLE_BMSK                                                      0x4
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_VENUS0_XPU2_MSA_INTR_ENABLE_SHFT                                                      0x2
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_RPM_XPU2_MSA_INTR_ENABLE_BMSK                                                         0x2
#define HWIO_TCSR_SS_XPU2_MSA_INTR0_ENABLE_RPM_XPU2_MSA_INTR_ENABLE_SHFT                                                         0x1

#define HWIO_TCSR_TCSR_CLK_EN_ADDR                                                                                        (TCSR_TCSR_REGS_REG_BASE      + 0x0000407c)
#define HWIO_TCSR_TCSR_CLK_EN_RMSK                                                                                               0x1
#define HWIO_TCSR_TCSR_CLK_EN_IN          \
        in_dword_masked(HWIO_TCSR_TCSR_CLK_EN_ADDR, HWIO_TCSR_TCSR_CLK_EN_RMSK)
#define HWIO_TCSR_TCSR_CLK_EN_INM(m)      \
        in_dword_masked(HWIO_TCSR_TCSR_CLK_EN_ADDR, m)
#define HWIO_TCSR_TCSR_CLK_EN_OUT(v)      \
        out_dword(HWIO_TCSR_TCSR_CLK_EN_ADDR,v)
#define HWIO_TCSR_TCSR_CLK_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TCSR_CLK_EN_ADDR,m,v,HWIO_TCSR_TCSR_CLK_EN_IN)
#define HWIO_TCSR_TCSR_CLK_EN_TCSR_CLK_EN_BMSK                                                                                   0x1
#define HWIO_TCSR_TCSR_CLK_EN_TCSR_CLK_EN_SHFT                                                                                   0x0

#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR                                                                               (TCSR_TCSR_REGS_REG_BASE      + 0x00005000)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_RMSK                                                                                      0x1
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR, HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_RMSK)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR, m)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR,v)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR,m,v,HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_IN)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_TIMEOUT_SLAVE_GLB_EN_BMSK                                                                 0x1
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_TIMEOUT_SLAVE_GLB_EN_SHFT                                                                 0x0

#define HWIO_TCSR_XPU_NSEN_STATUS_ADDR                                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x00005004)
#define HWIO_TCSR_XPU_NSEN_STATUS_RMSK                                                                                           0x2
#define HWIO_TCSR_XPU_NSEN_STATUS_IN          \
        in_dword_masked(HWIO_TCSR_XPU_NSEN_STATUS_ADDR, HWIO_TCSR_XPU_NSEN_STATUS_RMSK)
#define HWIO_TCSR_XPU_NSEN_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_XPU_NSEN_STATUS_ADDR, m)
#define HWIO_TCSR_XPU_NSEN_STATUS_REGS_XPU2_NSEN_STATUS_BMSK                                                                     0x2
#define HWIO_TCSR_XPU_NSEN_STATUS_REGS_XPU2_NSEN_STATUS_SHFT                                                                     0x1

#define HWIO_TCSR_XPU_VMIDEN_STATUS_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00005008)
#define HWIO_TCSR_XPU_VMIDEN_STATUS_RMSK                                                                                         0x2
#define HWIO_TCSR_XPU_VMIDEN_STATUS_IN          \
        in_dword_masked(HWIO_TCSR_XPU_VMIDEN_STATUS_ADDR, HWIO_TCSR_XPU_VMIDEN_STATUS_RMSK)
#define HWIO_TCSR_XPU_VMIDEN_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_XPU_VMIDEN_STATUS_ADDR, m)
#define HWIO_TCSR_XPU_VMIDEN_STATUS_REGS_XPU2_VMIDEN_STATUS_BMSK                                                                 0x2
#define HWIO_TCSR_XPU_VMIDEN_STATUS_REGS_XPU2_VMIDEN_STATUS_SHFT                                                                 0x1

#define HWIO_TCSR_XPU_MSAEN_STATUS_ADDR                                                                                   (TCSR_TCSR_REGS_REG_BASE      + 0x0000500c)
#define HWIO_TCSR_XPU_MSAEN_STATUS_RMSK                                                                                          0x2
#define HWIO_TCSR_XPU_MSAEN_STATUS_IN          \
        in_dword_masked(HWIO_TCSR_XPU_MSAEN_STATUS_ADDR, HWIO_TCSR_XPU_MSAEN_STATUS_RMSK)
#define HWIO_TCSR_XPU_MSAEN_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_XPU_MSAEN_STATUS_ADDR, m)
#define HWIO_TCSR_XPU_MSAEN_STATUS_REGS_XPU2_MSAEN_STATUS_BMSK                                                                   0x2
#define HWIO_TCSR_XPU_MSAEN_STATUS_REGS_XPU2_MSAEN_STATUS_SHFT                                                                   0x1

#define HWIO_TCSR_TZ_WONCE_n_ADDR(n)                                                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x00006000 + 0x4 * (n))
#define HWIO_TCSR_TZ_WONCE_n_RMSK                                                                                         0xffffffff
#define HWIO_TCSR_TZ_WONCE_n_MAXn                                                                                                 15
#define HWIO_TCSR_TZ_WONCE_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_TZ_WONCE_n_ADDR(n), HWIO_TCSR_TZ_WONCE_n_RMSK)
#define HWIO_TCSR_TZ_WONCE_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_TZ_WONCE_n_ADDR(n), mask)
#define HWIO_TCSR_TZ_WONCE_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_TZ_WONCE_n_ADDR(n),val)
#define HWIO_TCSR_TZ_WONCE_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_TZ_WONCE_n_ADDR(n),mask,val,HWIO_TCSR_TZ_WONCE_n_INI(n))
#define HWIO_TCSR_TZ_WONCE_n_TZ_WONCE_ADDRESS_BMSK                                                                        0xffffffff
#define HWIO_TCSR_TZ_WONCE_n_TZ_WONCE_ADDRESS_SHFT                                                                               0x0

#define HWIO_TCSR_BOOT_MISC_DETECT_ADDR                                                                                   (TCSR_TCSR_REGS_REG_BASE      + 0x00006100)
#define HWIO_TCSR_BOOT_MISC_DETECT_RMSK                                                                                   0xffffffff
#define HWIO_TCSR_BOOT_MISC_DETECT_IN          \
        in_dword_masked(HWIO_TCSR_BOOT_MISC_DETECT_ADDR, HWIO_TCSR_BOOT_MISC_DETECT_RMSK)
#define HWIO_TCSR_BOOT_MISC_DETECT_INM(m)      \
        in_dword_masked(HWIO_TCSR_BOOT_MISC_DETECT_ADDR, m)
#define HWIO_TCSR_BOOT_MISC_DETECT_OUT(v)      \
        out_dword(HWIO_TCSR_BOOT_MISC_DETECT_ADDR,v)
#define HWIO_TCSR_BOOT_MISC_DETECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_BOOT_MISC_DETECT_ADDR,m,v,HWIO_TCSR_BOOT_MISC_DETECT_IN)
#define HWIO_TCSR_BOOT_MISC_DETECT_BOOT_MISC_DETECT_BMSK                                                                  0xffffffff
#define HWIO_TCSR_BOOT_MISC_DETECT_BOOT_MISC_DETECT_SHFT                                                                         0x0

#define HWIO_TCSR_APSS_VMID_ADDR                                                                                          (TCSR_TCSR_REGS_REG_BASE      + 0x00006110)
#define HWIO_TCSR_APSS_VMID_RMSK                                                                                                0x1f
#define HWIO_TCSR_APSS_VMID_IN          \
        in_dword_masked(HWIO_TCSR_APSS_VMID_ADDR, HWIO_TCSR_APSS_VMID_RMSK)
#define HWIO_TCSR_APSS_VMID_INM(m)      \
        in_dword_masked(HWIO_TCSR_APSS_VMID_ADDR, m)
#define HWIO_TCSR_APSS_VMID_OUT(v)      \
        out_dword(HWIO_TCSR_APSS_VMID_ADDR,v)
#define HWIO_TCSR_APSS_VMID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_APSS_VMID_ADDR,m,v,HWIO_TCSR_APSS_VMID_IN)
#define HWIO_TCSR_APSS_VMID_APSS_VMID_BMSK                                                                                      0x1f
#define HWIO_TCSR_APSS_VMID_APSS_VMID_SHFT                                                                                       0x0

#define HWIO_TCSR_MMSS_RPM_IRQ_EN_ADDR                                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x00007000)
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RMSK                                                                                     0x1f1fffe
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_RPM_IRQ_EN_ADDR, HWIO_TCSR_MMSS_RPM_IRQ_EN_RMSK)
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_RPM_IRQ_EN_ADDR, m)
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_RPM_IRQ_EN_ADDR,v)
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_RPM_IRQ_EN_ADDR,m,v,HWIO_TCSR_MMSS_RPM_IRQ_EN_IN)
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_VENUS0_IRQ_BMSK                                                               0x1000000
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_VENUS0_IRQ_SHFT                                                                    0x18
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ10_BMSK                                                               0x800000
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ10_SHFT                                                                   0x17
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ11_BMSK                                                               0x400000
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ11_SHFT                                                                   0x16
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ12_BMSK                                                               0x200000
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ12_SHFT                                                                   0x15
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_MDSS_IRQ_BMSK                                                                  0x100000
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_MDSS_IRQ_SHFT                                                                      0x14
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_GC_SYS_IRQ3_BMSK                                                                0x10000
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_GC_SYS_IRQ3_SHFT                                                                   0x10
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_GC_SYS_IRQ2_BMSK                                                                 0x8000
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_GC_SYS_IRQ2_SHFT                                                                    0xf
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_GC_SYS_IRQ1_BMSK                                                                 0x4000
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_GC_SYS_IRQ1_SHFT                                                                    0xe
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_GC_SYS_IRQ0_BMSK                                                                 0x2000
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_GC_SYS_IRQ0_SHFT                                                                    0xd
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ0_BMSK                                                                  0x1000
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ0_SHFT                                                                     0xc
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ1_BMSK                                                                   0x800
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ1_SHFT                                                                     0xb
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ2_BMSK                                                                   0x400
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ2_SHFT                                                                     0xa
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ3_BMSK                                                                   0x200
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ3_SHFT                                                                     0x9
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ4_BMSK                                                                   0x100
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ4_SHFT                                                                     0x8
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ5_BMSK                                                                    0x80
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ5_SHFT                                                                     0x7
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ6_BMSK                                                                    0x40
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ6_SHFT                                                                     0x6
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ7_BMSK                                                                    0x20
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ7_SHFT                                                                     0x5
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ8_BMSK                                                                    0x10
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ8_SHFT                                                                     0x4
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ9_BMSK                                                                     0x8
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CAMSS_IRQ9_SHFT                                                                     0x3
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CSIPHY_0_IRQ_BMSK                                                                   0x4
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CSIPHY_0_IRQ_SHFT                                                                   0x2
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CSIPHY_1_IRQ_BMSK                                                                   0x2
#define HWIO_TCSR_MMSS_RPM_IRQ_EN_RPM_IRQ_EN_CSIPHY_1_IRQ_SHFT                                                                   0x1

#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_ADDR                                                                                 (TCSR_TCSR_REGS_REG_BASE      + 0x00007004)
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RMSK                                                                                  0x1f1fffe
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_ADDR, HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RMSK)
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_ADDR, m)
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_ADDR,v)
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_ADDR,m,v,HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_IN)
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_VENUS0_IRQ_BMSK                                                         0x1000000
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_VENUS0_IRQ_SHFT                                                              0x18
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ10_BMSK                                                         0x800000
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ10_SHFT                                                             0x17
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ11_BMSK                                                         0x400000
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ11_SHFT                                                             0x16
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ12_BMSK                                                         0x200000
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ12_SHFT                                                             0x15
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_MDSS_IRQ_BMSK                                                            0x100000
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_MDSS_IRQ_SHFT                                                                0x14
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_GC_SYS_IRQ3_BMSK                                                          0x10000
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_GC_SYS_IRQ3_SHFT                                                             0x10
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_GC_SYS_IRQ2_BMSK                                                           0x8000
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_GC_SYS_IRQ2_SHFT                                                              0xf
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_GC_SYS_IRQ1_BMSK                                                           0x4000
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_GC_SYS_IRQ1_SHFT                                                              0xe
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_GC_SYS_IRQ0_BMSK                                                           0x2000
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_GC_SYS_IRQ0_SHFT                                                              0xd
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ0_BMSK                                                            0x1000
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ0_SHFT                                                               0xc
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ1_BMSK                                                             0x800
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ1_SHFT                                                               0xb
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ2_BMSK                                                             0x400
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ2_SHFT                                                               0xa
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ3_BMSK                                                             0x200
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ3_SHFT                                                               0x9
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ4_BMSK                                                             0x100
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ4_SHFT                                                               0x8
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ5_BMSK                                                              0x80
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ5_SHFT                                                               0x7
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ6_BMSK                                                              0x40
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ6_SHFT                                                               0x6
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ7_BMSK                                                              0x20
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ7_SHFT                                                               0x5
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ8_BMSK                                                              0x10
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ8_SHFT                                                               0x4
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ9_BMSK                                                               0x8
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CAMSS_IRQ9_SHFT                                                               0x3
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CSIPHY_0_IRQ_BMSK                                                             0x4
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CSIPHY_0_IRQ_SHFT                                                             0x2
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CSIPHY_1_IRQ_BMSK                                                             0x2
#define HWIO_TCSR_MMSS_RPM_IRQ_CLEAR_RPM_IRQ_CLEAR_CSIPHY_1_IRQ_SHFT                                                             0x1

#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00007008)
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RMSK                                                                                 0x1f1fffe
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_RPM_IRQ_STATUS_ADDR, HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RMSK)
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_RPM_IRQ_STATUS_ADDR, m)
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_VENUS0_IRQ_BMSK                                                       0x1000000
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_VENUS0_IRQ_SHFT                                                            0x18
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ10_BMSK                                                       0x800000
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ10_SHFT                                                           0x17
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ11_BMSK                                                       0x400000
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ11_SHFT                                                           0x16
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ12_BMSK                                                       0x200000
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ12_SHFT                                                           0x15
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_MDSS_IRQ_BMSK                                                          0x100000
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_MDSS_IRQ_SHFT                                                              0x14
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_GC_SYS_IRQ3_BMSK                                                        0x10000
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_GC_SYS_IRQ3_SHFT                                                           0x10
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_GC_SYS_IRQ2_BMSK                                                         0x8000
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_GC_SYS_IRQ2_SHFT                                                            0xf
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_GC_SYS_IRQ1_BMSK                                                         0x4000
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_GC_SYS_IRQ1_SHFT                                                            0xe
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_GC_SYS_IRQ0_BMSK                                                         0x2000
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_GC_SYS_IRQ0_SHFT                                                            0xd
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ0_BMSK                                                          0x1000
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ0_SHFT                                                             0xc
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ1_BMSK                                                           0x800
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ1_SHFT                                                             0xb
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ2_BMSK                                                           0x400
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ2_SHFT                                                             0xa
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ3_BMSK                                                           0x200
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ3_SHFT                                                             0x9
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ4_BMSK                                                           0x100
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ4_SHFT                                                             0x8
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ5_BMSK                                                            0x80
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ5_SHFT                                                             0x7
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ6_BMSK                                                            0x40
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ6_SHFT                                                             0x6
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ7_BMSK                                                            0x20
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ7_SHFT                                                             0x5
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ8_BMSK                                                            0x10
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ8_SHFT                                                             0x4
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ9_BMSK                                                             0x8
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CAMSS_IRQ9_SHFT                                                             0x3
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CSIPHY_0_IRQ_BMSK                                                           0x4
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CSIPHY_0_IRQ_SHFT                                                           0x2
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CSIPHY_1_IRQ_BMSK                                                           0x2
#define HWIO_TCSR_MMSS_RPM_IRQ_STATUS_RPM_IRQ_STATUS_CSIPHY_1_IRQ_SHFT                                                           0x1

#define HWIO_TCSR_MMSS_OXILI_GC_SYS_AHB_STATUS_ADDR                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x00007010)
#define HWIO_TCSR_MMSS_OXILI_GC_SYS_AHB_STATUS_RMSK                                                                       0xffffffff
#define HWIO_TCSR_MMSS_OXILI_GC_SYS_AHB_STATUS_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_OXILI_GC_SYS_AHB_STATUS_ADDR, HWIO_TCSR_MMSS_OXILI_GC_SYS_AHB_STATUS_RMSK)
#define HWIO_TCSR_MMSS_OXILI_GC_SYS_AHB_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_OXILI_GC_SYS_AHB_STATUS_ADDR, m)
#define HWIO_TCSR_MMSS_OXILI_GC_SYS_AHB_STATUS_OXILI_GC_SYS_AHB_STATUS_BMSK                                               0xffffffff
#define HWIO_TCSR_MMSS_OXILI_GC_SYS_AHB_STATUS_OXILI_GC_SYS_AHB_STATUS_SHFT                                                      0x0

#define HWIO_TCSR_MMSS_OXILI_CMD_REG_ADDR                                                                                 (TCSR_TCSR_REGS_REG_BASE      + 0x00007014)
#define HWIO_TCSR_MMSS_OXILI_CMD_REG_RMSK                                                                                        0xf
#define HWIO_TCSR_MMSS_OXILI_CMD_REG_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_OXILI_CMD_REG_ADDR, HWIO_TCSR_MMSS_OXILI_CMD_REG_RMSK)
#define HWIO_TCSR_MMSS_OXILI_CMD_REG_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_OXILI_CMD_REG_ADDR, m)
#define HWIO_TCSR_MMSS_OXILI_CMD_REG_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_OXILI_CMD_REG_ADDR,v)
#define HWIO_TCSR_MMSS_OXILI_CMD_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_OXILI_CMD_REG_ADDR,m,v,HWIO_TCSR_MMSS_OXILI_CMD_REG_IN)
#define HWIO_TCSR_MMSS_OXILI_CMD_REG_OXILI_CMD_BMSK                                                                              0xf
#define HWIO_TCSR_MMSS_OXILI_CMD_REG_OXILI_CMD_SHFT                                                                              0x0

#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_ADDR                                                                            (TCSR_TCSR_REGS_REG_BASE      + 0x00007020)
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_RMSK                                                                                0x83ff
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_ADDR, HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_RMSK)
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_ADDR, m)
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_ADDR,v)
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_ADDR,m,v,HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_IN)
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLAMP_EN_BMSK                                                                  0x8000
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLAMP_EN_SHFT                                                                     0xf
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_EN_BMSK                                                                   0x200
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_EN_SHFT                                                                     0x9
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_ULPS_REQUEST_BMSK                                                         0x100
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_ULPS_REQUEST_SHFT                                                           0x8
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_EN_BMSK                                                                     0x80
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_EN_SHFT                                                                      0x7
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_ULPS_REQUEST_BMSK                                                           0x40
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_ULPS_REQUEST_SHFT                                                            0x6
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_EN_BMSK                                                                     0x20
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_EN_SHFT                                                                      0x5
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_ULPS_REQUEST_BMSK                                                           0x10
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_ULPS_REQUEST_SHFT                                                            0x4
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_EN_BMSK                                                                      0x8
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_EN_SHFT                                                                      0x3
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_ULPS_REQUEST_BMSK                                                            0x4
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_ULPS_REQUEST_SHFT                                                            0x2
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_EN_BMSK                                                                      0x2
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_EN_SHFT                                                                      0x1
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_ULPS_REQUEST_BMSK                                                            0x1
#define HWIO_TCSR_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_ULPS_REQUEST_SHFT                                                            0x0

#define HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_ADDR                                                                             (TCSR_TCSR_REGS_REG_BASE      + 0x00007024)
#define HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_RMSK                                                                                    0x1
#define HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_ADDR, HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_RMSK)
#define HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_ADDR, m)
#define HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_ADDR,v)
#define HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_ADDR,m,v,HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_IN)
#define HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_DSI_PHYRESET_CTRL_BMSK                                                                  0x1
#define HWIO_TCSR_MMSS_DSI_PHYRESET_CTRL_DSI_PHYRESET_CTRL_SHFT                                                                  0x0

#define HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_ADDR                                                                             (TCSR_TCSR_REGS_REG_BASE      + 0x00007030)
#define HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_RMSK                                                                                   0xff
#define HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_ADDR, HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_RMSK)
#define HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_ADDR, m)
#define HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_ADDR,v)
#define HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_ADDR,m,v,HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_IN)
#define HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_WAKEUP_COUNTER_BMSK                                                                    0xf0
#define HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_WAKEUP_COUNTER_SHFT                                                                     0x4
#define HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_TO_SLEEP_COUNTER_BMSK                                                                   0xf
#define HWIO_TCSR_MMSS_IMEM_FSCGC_TIMERS_TO_SLEEP_COUNTER_SHFT                                                                   0x0

#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_ADDR                                                                         (TCSR_TCSR_REGS_REG_BASE      + 0x00007034)
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_RMSK                                                                           0xffffff
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_ADDR, HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_RMSK)
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_ADDR, m)
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_ADDR,v)
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_ADDR,m,v,HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_IN)
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_HALT_CLOCK_BMSK                                                                0xff0000
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_HALT_CLOCK_SHFT                                                                    0x10
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_CORE_ON_BMSK                                                                     0xff00
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_CORE_ON_SHFT                                                                        0x8
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_PERIF_ON_BMSK                                                                      0xff
#define HWIO_TCSR_MMSS_IMEM_EX_FSCGC_CONTROL_PERIF_ON_SHFT                                                                       0x0

#define HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_ADDR                                                                               (TCSR_TCSR_REGS_REG_BASE      + 0x00007038)
#define HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_RMSK                                                                                      0x7
#define HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_IN          \
        in_dword_masked(HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_ADDR, HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_RMSK)
#define HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_INM(m)      \
        in_dword_masked(HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_ADDR, m)
#define HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_OUT(v)      \
        out_dword(HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_ADDR,v)
#define HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_ADDR,m,v,HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_IN)
#define HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_EX_RAM_CLK_EN_BMSK                                                                        0x4
#define HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_EX_RAM_CLK_EN_SHFT                                                                        0x2
#define HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_EX_RAM_CONFIG_BMSK                                                                        0x3
#define HWIO_TCSR_MMSS_IMEM_RAM_CONFIG_EX_RAM_CONFIG_SHFT                                                                        0x0

#define HWIO_TCSR_TBU_BYPASS_ENABLE_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00008000)
#define HWIO_TCSR_TBU_BYPASS_ENABLE_RMSK                                                                                       0x3ff
#define HWIO_TCSR_TBU_BYPASS_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TBU_BYPASS_ENABLE_ADDR, HWIO_TCSR_TBU_BYPASS_ENABLE_RMSK)
#define HWIO_TCSR_TBU_BYPASS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TBU_BYPASS_ENABLE_ADDR, m)
#define HWIO_TCSR_TBU_BYPASS_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TBU_BYPASS_ENABLE_ADDR,v)
#define HWIO_TCSR_TBU_BYPASS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TBU_BYPASS_ENABLE_ADDR,m,v,HWIO_TCSR_TBU_BYPASS_ENABLE_IN)
#define HWIO_TCSR_TBU_BYPASS_ENABLE_TBU_BYPASS_ENABLE_BMSK                                                                     0x3ff
#define HWIO_TCSR_TBU_BYPASS_ENABLE_TBU_BYPASS_ENABLE_SHFT                                                                       0x0

#define HWIO_TCSR_RESET_DEBUG_SW_ENTRY_ADDR                                                                               (TCSR_TCSR_REGS_REG_BASE      + 0x00009000)
#define HWIO_TCSR_RESET_DEBUG_SW_ENTRY_RMSK                                                                               0xffffffff
#define HWIO_TCSR_RESET_DEBUG_SW_ENTRY_IN          \
        in_dword_masked(HWIO_TCSR_RESET_DEBUG_SW_ENTRY_ADDR, HWIO_TCSR_RESET_DEBUG_SW_ENTRY_RMSK)
#define HWIO_TCSR_RESET_DEBUG_SW_ENTRY_INM(m)      \
        in_dword_masked(HWIO_TCSR_RESET_DEBUG_SW_ENTRY_ADDR, m)
#define HWIO_TCSR_RESET_DEBUG_SW_ENTRY_OUT(v)      \
        out_dword(HWIO_TCSR_RESET_DEBUG_SW_ENTRY_ADDR,v)
#define HWIO_TCSR_RESET_DEBUG_SW_ENTRY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_RESET_DEBUG_SW_ENTRY_ADDR,m,v,HWIO_TCSR_RESET_DEBUG_SW_ENTRY_IN)
#define HWIO_TCSR_RESET_DEBUG_SW_ENTRY_RESET_DEBUG_SW_ENTRY_BMSK                                                          0xffffffff
#define HWIO_TCSR_RESET_DEBUG_SW_ENTRY_RESET_DEBUG_SW_ENTRY_SHFT                                                                 0x0

#define HWIO_TCSR_QPDI_DISABLE_CFG_ADDR                                                                                   (TCSR_TCSR_REGS_REG_BASE      + 0x0000a000)
#define HWIO_TCSR_QPDI_DISABLE_CFG_RMSK                                                                                          0x1
#define HWIO_TCSR_QPDI_DISABLE_CFG_IN          \
        in_dword_masked(HWIO_TCSR_QPDI_DISABLE_CFG_ADDR, HWIO_TCSR_QPDI_DISABLE_CFG_RMSK)
#define HWIO_TCSR_QPDI_DISABLE_CFG_INM(m)      \
        in_dword_masked(HWIO_TCSR_QPDI_DISABLE_CFG_ADDR, m)
#define HWIO_TCSR_QPDI_DISABLE_CFG_OUT(v)      \
        out_dword(HWIO_TCSR_QPDI_DISABLE_CFG_ADDR,v)
#define HWIO_TCSR_QPDI_DISABLE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_QPDI_DISABLE_CFG_ADDR,m,v,HWIO_TCSR_QPDI_DISABLE_CFG_IN)
#define HWIO_TCSR_QPDI_DISABLE_CFG_QPDI_DISABLE_CFG_BMSK                                                                         0x1
#define HWIO_TCSR_QPDI_DISABLE_CFG_QPDI_DISABLE_CFG_SHFT                                                                         0x0

#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_ADDR                                                                               (TCSR_TCSR_REGS_REG_BASE      + 0x0000b080)
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_RMSK                                                                               0xffffffff
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_IN          \
        in_dword_masked(HWIO_TCSR_COMPILER_VDDCX_ACC_0_ADDR, HWIO_TCSR_COMPILER_VDDCX_ACC_0_RMSK)
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_INM(m)      \
        in_dword_masked(HWIO_TCSR_COMPILER_VDDCX_ACC_0_ADDR, m)
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_OUT(v)      \
        out_dword(HWIO_TCSR_COMPILER_VDDCX_ACC_0_ADDR,v)
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_COMPILER_VDDCX_ACC_0_ADDR,m,v,HWIO_TCSR_COMPILER_VDDCX_ACC_0_IN)
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_HDHPLLSP127_BMSK                                                          0xfc000000
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_HDHPLLSP127_SHFT                                                                0x1a
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_HPLVRF_BMSK                                                                0x3f00000
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_HPLVRF_SHFT                                                                     0x14
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_LLRF240_1_BMSK                                                               0xf0000
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_LLRF240_1_SHFT                                                                  0x10
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_STDSP155_BMSK                                                                 0xf000
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_STDSP155_SHFT                                                                    0xc
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_LLRF240_2_BMSK                                                                 0xf00
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_LLRF240_2_SHFT                                                                   0x8
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_LLPDP155_BMSK                                                                   0xf0
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_LLPDP155_SHFT                                                                    0x4
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_LLSP155_BMSK                                                                     0xf
#define HWIO_TCSR_COMPILER_VDDCX_ACC_0_COMPILER_LLSP155_SHFT                                                                     0x0

#define HWIO_TCSR_COMPILER_VDDCX_ACC_1_ADDR                                                                               (TCSR_TCSR_REGS_REG_BASE      + 0x0000b084)
#define HWIO_TCSR_COMPILER_VDDCX_ACC_1_RMSK                                                                                      0xf
#define HWIO_TCSR_COMPILER_VDDCX_ACC_1_IN          \
        in_dword_masked(HWIO_TCSR_COMPILER_VDDCX_ACC_1_ADDR, HWIO_TCSR_COMPILER_VDDCX_ACC_1_RMSK)
#define HWIO_TCSR_COMPILER_VDDCX_ACC_1_INM(m)      \
        in_dword_masked(HWIO_TCSR_COMPILER_VDDCX_ACC_1_ADDR, m)
#define HWIO_TCSR_COMPILER_VDDCX_ACC_1_OUT(v)      \
        out_dword(HWIO_TCSR_COMPILER_VDDCX_ACC_1_ADDR,v)
#define HWIO_TCSR_COMPILER_VDDCX_ACC_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_COMPILER_VDDCX_ACC_1_ADDR,m,v,HWIO_TCSR_COMPILER_VDDCX_ACC_1_IN)
#define HWIO_TCSR_COMPILER_VDDCX_ACC_1_COMPILER_UHDLLPDP155_BMSK                                                                 0xf
#define HWIO_TCSR_COMPILER_VDDCX_ACC_1_COMPILER_UHDLLPDP155_SHFT                                                                 0x0

#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_ADDR                                                                                 (TCSR_TCSR_REGS_REG_BASE      + 0x0000b090)
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_RMSK                                                                                 0xffffffff
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_IN          \
        in_dword_masked(HWIO_TCSR_CUSTOM_VDDCX_ACC_0_ADDR, HWIO_TCSR_CUSTOM_VDDCX_ACC_0_RMSK)
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_INM(m)      \
        in_dword_masked(HWIO_TCSR_CUSTOM_VDDCX_ACC_0_ADDR, m)
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_OUT(v)      \
        out_dword(HWIO_TCSR_CUSTOM_VDDCX_ACC_0_ADDR,v)
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_CUSTOM_VDDCX_ACC_0_ADDR,m,v,HWIO_TCSR_CUSTOM_VDDCX_ACC_0_IN)
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_UHPSP127_L_BMSK                                                               0xc0000000
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_UHPSP127_L_SHFT                                                                     0x1e
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_UHPSP155_OTHERS_BMSK                                                          0x3f000000
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_UHPSP155_OTHERS_SHFT                                                                0x18
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_UHPSP155_DATA_BMSK                                                              0xfc0000
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_UHPSP155_DATA_SHFT                                                                  0x12
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_8P_RF_BMSK                                                                       0x3f000
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_8P_RF_SHFT                                                                           0xc
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_6P_RF_BMSK                                                                         0xfc0
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_6P_RF_SHFT                                                                           0x6
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_HPLVF240_BYPASS_BMSK                                                                0x3f
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_0_CUSTOM_HPLVF240_BYPASS_SHFT                                                                 0x0

#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_ADDR                                                                                 (TCSR_TCSR_REGS_REG_BASE      + 0x0000b094)
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_RMSK                                                                                   0x3fffff
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_IN          \
        in_dword_masked(HWIO_TCSR_CUSTOM_VDDCX_ACC_1_ADDR, HWIO_TCSR_CUSTOM_VDDCX_ACC_1_RMSK)
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_INM(m)      \
        in_dword_masked(HWIO_TCSR_CUSTOM_VDDCX_ACC_1_ADDR, m)
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_OUT(v)      \
        out_dword(HWIO_TCSR_CUSTOM_VDDCX_ACC_1_ADDR,v)
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_CUSTOM_VDDCX_ACC_1_ADDR,m,v,HWIO_TCSR_CUSTOM_VDDCX_ACC_1_IN)
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_CUSTOM_VHPLLSP127_BMSK                                                                 0x3f0000
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_CUSTOM_VHPLLSP127_SHFT                                                                     0x10
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_CUSTOM_HPLLSP155_BMSK                                                                    0xfc00
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_CUSTOM_HPLLSP155_SHFT                                                                       0xa
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_CUSTOM_HPLLSP127_BMSK                                                                     0x3f0
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_CUSTOM_HPLLSP127_SHFT                                                                       0x4
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_CUSTOM_UHPSP127_H_BMSK                                                                      0xf
#define HWIO_TCSR_CUSTOM_VDDCX_ACC_1_CUSTOM_UHPSP127_H_SHFT                                                                      0x0

#define HWIO_TCSR_MEM_ACC_SEL_VDDCX_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x0000b100)
#define HWIO_TCSR_MEM_ACC_SEL_VDDCX_RMSK                                                                                         0x3
#define HWIO_TCSR_MEM_ACC_SEL_VDDCX_IN          \
        in_dword_masked(HWIO_TCSR_MEM_ACC_SEL_VDDCX_ADDR, HWIO_TCSR_MEM_ACC_SEL_VDDCX_RMSK)
#define HWIO_TCSR_MEM_ACC_SEL_VDDCX_INM(m)      \
        in_dword_masked(HWIO_TCSR_MEM_ACC_SEL_VDDCX_ADDR, m)
#define HWIO_TCSR_MEM_ACC_SEL_VDDCX_OUT(v)      \
        out_dword(HWIO_TCSR_MEM_ACC_SEL_VDDCX_ADDR,v)
#define HWIO_TCSR_MEM_ACC_SEL_VDDCX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MEM_ACC_SEL_VDDCX_ADDR,m,v,HWIO_TCSR_MEM_ACC_SEL_VDDCX_IN)
#define HWIO_TCSR_MEM_ACC_SEL_VDDCX_MEM_ACC_SEL_VDDCX_BMSK                                                                       0x3
#define HWIO_TCSR_MEM_ACC_SEL_VDDCX_MEM_ACC_SEL_VDDCX_SHFT                                                                       0x0

#define HWIO_TCSR_MEM_ACC_SEL_APC_ADDR                                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x0000f000)
#define HWIO_TCSR_MEM_ACC_SEL_APC_RMSK                                                                                         0x303
#define HWIO_TCSR_MEM_ACC_SEL_APC_IN          \
        in_dword_masked(HWIO_TCSR_MEM_ACC_SEL_APC_ADDR, HWIO_TCSR_MEM_ACC_SEL_APC_RMSK)
#define HWIO_TCSR_MEM_ACC_SEL_APC_INM(m)      \
        in_dword_masked(HWIO_TCSR_MEM_ACC_SEL_APC_ADDR, m)
#define HWIO_TCSR_MEM_ACC_SEL_APC_OUT(v)      \
        out_dword(HWIO_TCSR_MEM_ACC_SEL_APC_ADDR,v)
#define HWIO_TCSR_MEM_ACC_SEL_APC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MEM_ACC_SEL_APC_ADDR,m,v,HWIO_TCSR_MEM_ACC_SEL_APC_IN)
#define HWIO_TCSR_MEM_ACC_SEL_APC_MEM_ACC_SEL_APC_L2_BMSK                                                                      0x300
#define HWIO_TCSR_MEM_ACC_SEL_APC_MEM_ACC_SEL_APC_L2_SHFT                                                                        0x8
#define HWIO_TCSR_MEM_ACC_SEL_APC_MEM_ACC_SEL_APC_L1_BMSK                                                                        0x3
#define HWIO_TCSR_MEM_ACC_SEL_APC_MEM_ACC_SEL_APC_L1_SHFT                                                                        0x0

#define HWIO_TCSR_CUSTOM_APC_ACC_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x0000f004)
#define HWIO_TCSR_CUSTOM_APC_ACC_RMSK                                                                                          0xfff
#define HWIO_TCSR_CUSTOM_APC_ACC_IN          \
        in_dword_masked(HWIO_TCSR_CUSTOM_APC_ACC_ADDR, HWIO_TCSR_CUSTOM_APC_ACC_RMSK)
#define HWIO_TCSR_CUSTOM_APC_ACC_INM(m)      \
        in_dword_masked(HWIO_TCSR_CUSTOM_APC_ACC_ADDR, m)
#define HWIO_TCSR_CUSTOM_APC_ACC_OUT(v)      \
        out_dword(HWIO_TCSR_CUSTOM_APC_ACC_ADDR,v)
#define HWIO_TCSR_CUSTOM_APC_ACC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_CUSTOM_APC_ACC_ADDR,m,v,HWIO_TCSR_CUSTOM_APC_ACC_IN)
#define HWIO_TCSR_CUSTOM_APC_ACC_CUSTOM_UHPSP155_APC_L2_BMSK                                                                   0xfc0
#define HWIO_TCSR_CUSTOM_APC_ACC_CUSTOM_UHPSP155_APC_L2_SHFT                                                                     0x6
#define HWIO_TCSR_CUSTOM_APC_ACC_CUSTOM_UHPSP155_APC_L1_BMSK                                                                    0x3f
#define HWIO_TCSR_CUSTOM_APC_ACC_CUSTOM_UHPSP155_APC_L1_SHFT                                                                     0x0

#define HWIO_TCSR_SPARE_APU_REG2_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x0000f008)
#define HWIO_TCSR_SPARE_APU_REG2_RMSK                                                                                     0xffffffff
#define HWIO_TCSR_SPARE_APU_REG2_IN          \
        in_dword_masked(HWIO_TCSR_SPARE_APU_REG2_ADDR, HWIO_TCSR_SPARE_APU_REG2_RMSK)
#define HWIO_TCSR_SPARE_APU_REG2_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPARE_APU_REG2_ADDR, m)
#define HWIO_TCSR_SPARE_APU_REG2_OUT(v)      \
        out_dword(HWIO_TCSR_SPARE_APU_REG2_ADDR,v)
#define HWIO_TCSR_SPARE_APU_REG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPARE_APU_REG2_ADDR,m,v,HWIO_TCSR_SPARE_APU_REG2_IN)
#define HWIO_TCSR_SPARE_APU_REG2_SPARE_APU_REG2_BMSK                                                                      0xffffffff
#define HWIO_TCSR_SPARE_APU_REG2_SPARE_APU_REG2_SHFT                                                                             0x0

#define HWIO_TCSR_SPARE_APU_REG3_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x0000f00c)
#define HWIO_TCSR_SPARE_APU_REG3_RMSK                                                                                     0xffffffff
#define HWIO_TCSR_SPARE_APU_REG3_IN          \
        in_dword_masked(HWIO_TCSR_SPARE_APU_REG3_ADDR, HWIO_TCSR_SPARE_APU_REG3_RMSK)
#define HWIO_TCSR_SPARE_APU_REG3_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPARE_APU_REG3_ADDR, m)
#define HWIO_TCSR_SPARE_APU_REG3_OUT(v)      \
        out_dword(HWIO_TCSR_SPARE_APU_REG3_ADDR,v)
#define HWIO_TCSR_SPARE_APU_REG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPARE_APU_REG3_ADDR,m,v,HWIO_TCSR_SPARE_APU_REG3_IN)
#define HWIO_TCSR_SPARE_APU_REG3_SPARE_APU_REG3_BMSK                                                                      0xffffffff
#define HWIO_TCSR_SPARE_APU_REG3_SPARE_APU_REG3_SHFT                                                                             0x0

#define HWIO_TCSR_SYS_POWER_CTRL_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x00010000)
#define HWIO_TCSR_SYS_POWER_CTRL_RMSK                                                                                         0xffff
#define HWIO_TCSR_SYS_POWER_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_SYS_POWER_CTRL_ADDR, HWIO_TCSR_SYS_POWER_CTRL_RMSK)
#define HWIO_TCSR_SYS_POWER_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_SYS_POWER_CTRL_ADDR, m)
#define HWIO_TCSR_SYS_POWER_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_SYS_POWER_CTRL_ADDR,v)
#define HWIO_TCSR_SYS_POWER_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SYS_POWER_CTRL_ADDR,m,v,HWIO_TCSR_SYS_POWER_CTRL_IN)
#define HWIO_TCSR_SYS_POWER_CTRL_SYS_POWER_CTRL_BMSK                                                                          0xffff
#define HWIO_TCSR_SYS_POWER_CTRL_SYS_POWER_CTRL_SHFT                                                                             0x0

#define HWIO_TCSR_USB_CORE_ID_ADDR                                                                                        (TCSR_TCSR_REGS_REG_BASE      + 0x00010004)
#define HWIO_TCSR_USB_CORE_ID_RMSK                                                                                               0x3
#define HWIO_TCSR_USB_CORE_ID_IN          \
        in_dword_masked(HWIO_TCSR_USB_CORE_ID_ADDR, HWIO_TCSR_USB_CORE_ID_RMSK)
#define HWIO_TCSR_USB_CORE_ID_INM(m)      \
        in_dword_masked(HWIO_TCSR_USB_CORE_ID_ADDR, m)
#define HWIO_TCSR_USB_CORE_ID_OUT(v)      \
        out_dword(HWIO_TCSR_USB_CORE_ID_ADDR,v)
#define HWIO_TCSR_USB_CORE_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_USB_CORE_ID_ADDR,m,v,HWIO_TCSR_USB_CORE_ID_IN)
#define HWIO_TCSR_USB_CORE_ID_USB_CORE_ID_BMSK                                                                                   0x3
#define HWIO_TCSR_USB_CORE_ID_USB_CORE_ID_SHFT                                                                                   0x0

#define HWIO_TCSR_LDO_SLEEP_CTRL_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x00011000)
#define HWIO_TCSR_LDO_SLEEP_CTRL_RMSK                                                                                            0x1
#define HWIO_TCSR_LDO_SLEEP_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_LDO_SLEEP_CTRL_ADDR, HWIO_TCSR_LDO_SLEEP_CTRL_RMSK)
#define HWIO_TCSR_LDO_SLEEP_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_SLEEP_CTRL_ADDR, m)
#define HWIO_TCSR_LDO_SLEEP_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_SLEEP_CTRL_ADDR,v)
#define HWIO_TCSR_LDO_SLEEP_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_SLEEP_CTRL_ADDR,m,v,HWIO_TCSR_LDO_SLEEP_CTRL_IN)
#define HWIO_TCSR_LDO_SLEEP_CTRL_LDO_SLEEP_BMSK                                                                                  0x1
#define HWIO_TCSR_LDO_SLEEP_CTRL_LDO_SLEEP_SHFT                                                                                  0x0

#define HWIO_TCSR_LDO_UPDATE_STATE_CTRL_ADDR                                                                              (TCSR_TCSR_REGS_REG_BASE      + 0x00011004)
#define HWIO_TCSR_LDO_UPDATE_STATE_CTRL_RMSK                                                                                     0x1
#define HWIO_TCSR_LDO_UPDATE_STATE_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_LDO_UPDATE_STATE_CTRL_ADDR, HWIO_TCSR_LDO_UPDATE_STATE_CTRL_RMSK)
#define HWIO_TCSR_LDO_UPDATE_STATE_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_UPDATE_STATE_CTRL_ADDR, m)
#define HWIO_TCSR_LDO_UPDATE_STATE_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_UPDATE_STATE_CTRL_ADDR,v)
#define HWIO_TCSR_LDO_UPDATE_STATE_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_UPDATE_STATE_CTRL_ADDR,m,v,HWIO_TCSR_LDO_UPDATE_STATE_CTRL_IN)
#define HWIO_TCSR_LDO_UPDATE_STATE_CTRL_LDO_UPDATE_STATE_BMSK                                                                    0x1
#define HWIO_TCSR_LDO_UPDATE_STATE_CTRL_LDO_UPDATE_STATE_SHFT                                                                    0x0

#define HWIO_TCSR_LDO_OBIAS_CTRL_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x00011008)
#define HWIO_TCSR_LDO_OBIAS_CTRL_RMSK                                                                                            0x1
#define HWIO_TCSR_LDO_OBIAS_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_LDO_OBIAS_CTRL_ADDR, HWIO_TCSR_LDO_OBIAS_CTRL_RMSK)
#define HWIO_TCSR_LDO_OBIAS_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_OBIAS_CTRL_ADDR, m)
#define HWIO_TCSR_LDO_OBIAS_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_OBIAS_CTRL_ADDR,v)
#define HWIO_TCSR_LDO_OBIAS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_OBIAS_CTRL_ADDR,m,v,HWIO_TCSR_LDO_OBIAS_CTRL_IN)
#define HWIO_TCSR_LDO_OBIAS_CTRL_LDO_OBIAS_ON_BMSK                                                                               0x1
#define HWIO_TCSR_LDO_OBIAS_CTRL_LDO_OBIAS_ON_SHFT                                                                               0x0

#define HWIO_TCSR_LDO_VREF_CONFIG_ADDR                                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x0001100c)
#define HWIO_TCSR_LDO_VREF_CONFIG_RMSK                                                                                           0xf
#define HWIO_TCSR_LDO_VREF_CONFIG_IN          \
        in_dword_masked(HWIO_TCSR_LDO_VREF_CONFIG_ADDR, HWIO_TCSR_LDO_VREF_CONFIG_RMSK)
#define HWIO_TCSR_LDO_VREF_CONFIG_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_VREF_CONFIG_ADDR, m)
#define HWIO_TCSR_LDO_VREF_CONFIG_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_VREF_CONFIG_ADDR,v)
#define HWIO_TCSR_LDO_VREF_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_VREF_CONFIG_ADDR,m,v,HWIO_TCSR_LDO_VREF_CONFIG_IN)
#define HWIO_TCSR_LDO_VREF_CONFIG_LDO_VREF_CONFIG_BMSK                                                                           0xf
#define HWIO_TCSR_LDO_VREF_CONFIG_LDO_VREF_CONFIG_SHFT                                                                           0x0

#define HWIO_TCSR_LDO_IB_CONFIG_ADDR                                                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x00011010)
#define HWIO_TCSR_LDO_IB_CONFIG_RMSK                                                                                             0x7
#define HWIO_TCSR_LDO_IB_CONFIG_IN          \
        in_dword_masked(HWIO_TCSR_LDO_IB_CONFIG_ADDR, HWIO_TCSR_LDO_IB_CONFIG_RMSK)
#define HWIO_TCSR_LDO_IB_CONFIG_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_IB_CONFIG_ADDR, m)
#define HWIO_TCSR_LDO_IB_CONFIG_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_IB_CONFIG_ADDR,v)
#define HWIO_TCSR_LDO_IB_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_IB_CONFIG_ADDR,m,v,HWIO_TCSR_LDO_IB_CONFIG_IN)
#define HWIO_TCSR_LDO_IB_CONFIG_LDO_IB_CONFIG_BMSK                                                                               0x7
#define HWIO_TCSR_LDO_IB_CONFIG_LDO_IB_CONFIG_SHFT                                                                               0x0

#define HWIO_TCSR_LDO_BGC_CONFIG_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x00011014)
#define HWIO_TCSR_LDO_BGC_CONFIG_RMSK                                                                                            0x7
#define HWIO_TCSR_LDO_BGC_CONFIG_IN          \
        in_dword_masked(HWIO_TCSR_LDO_BGC_CONFIG_ADDR, HWIO_TCSR_LDO_BGC_CONFIG_RMSK)
#define HWIO_TCSR_LDO_BGC_CONFIG_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_BGC_CONFIG_ADDR, m)
#define HWIO_TCSR_LDO_BGC_CONFIG_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_BGC_CONFIG_ADDR,v)
#define HWIO_TCSR_LDO_BGC_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_BGC_CONFIG_ADDR,m,v,HWIO_TCSR_LDO_BGC_CONFIG_IN)
#define HWIO_TCSR_LDO_BGC_CONFIG_LDO_BGC_BMSK                                                                                    0x7
#define HWIO_TCSR_LDO_BGC_CONFIG_LDO_BGC_SHFT                                                                                    0x0

#define HWIO_TCSR_LDO_VREF_CTRL_ADDR                                                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x00011018)
#define HWIO_TCSR_LDO_VREF_CTRL_RMSK                                                                                         0x10001
#define HWIO_TCSR_LDO_VREF_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_LDO_VREF_CTRL_ADDR, HWIO_TCSR_LDO_VREF_CTRL_RMSK)
#define HWIO_TCSR_LDO_VREF_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_VREF_CTRL_ADDR, m)
#define HWIO_TCSR_LDO_VREF_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_VREF_CTRL_ADDR,v)
#define HWIO_TCSR_LDO_VREF_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_VREF_CTRL_ADDR,m,v,HWIO_TCSR_LDO_VREF_CTRL_IN)
#define HWIO_TCSR_LDO_VREF_CTRL_LDO_VREF_SEL_OVR_BMSK                                                                        0x10000
#define HWIO_TCSR_LDO_VREF_CTRL_LDO_VREF_SEL_OVR_SHFT                                                                           0x10
#define HWIO_TCSR_LDO_VREF_CTRL_LDO_VREF_SEL_SW_BMSK                                                                             0x1
#define HWIO_TCSR_LDO_VREF_CTRL_LDO_VREF_SEL_SW_SHFT                                                                             0x0

#define HWIO_TCSR_LDO_LD_EN_ADDR                                                                                          (TCSR_TCSR_REGS_REG_BASE      + 0x0001101c)
#define HWIO_TCSR_LDO_LD_EN_RMSK                                                                                          0x80000000
#define HWIO_TCSR_LDO_LD_EN_IN          \
        in_dword_masked(HWIO_TCSR_LDO_LD_EN_ADDR, HWIO_TCSR_LDO_LD_EN_RMSK)
#define HWIO_TCSR_LDO_LD_EN_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_LD_EN_ADDR, m)
#define HWIO_TCSR_LDO_LD_EN_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_LD_EN_ADDR,v)
#define HWIO_TCSR_LDO_LD_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_LD_EN_ADDR,m,v,HWIO_TCSR_LDO_LD_EN_IN)
#define HWIO_TCSR_LDO_LD_EN_LDO_LD_EN_BMSK                                                                                0x80000000
#define HWIO_TCSR_LDO_LD_EN_LDO_LD_EN_SHFT                                                                                      0x1f

#define HWIO_TCSR_LDO_LD_CTRL_ADDR                                                                                        (TCSR_TCSR_REGS_REG_BASE      + 0x00011020)
#define HWIO_TCSR_LDO_LD_CTRL_RMSK                                                                                          0xff00ff
#define HWIO_TCSR_LDO_LD_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_LDO_LD_CTRL_ADDR, HWIO_TCSR_LDO_LD_CTRL_RMSK)
#define HWIO_TCSR_LDO_LD_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_LD_CTRL_ADDR, m)
#define HWIO_TCSR_LDO_LD_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_LD_CTRL_ADDR,v)
#define HWIO_TCSR_LDO_LD_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_LD_CTRL_ADDR,m,v,HWIO_TCSR_LDO_LD_CTRL_IN)
#define HWIO_TCSR_LDO_LD_CTRL_LDO_LD_MSB_BMSK                                                                               0xff0000
#define HWIO_TCSR_LDO_LD_CTRL_LDO_LD_MSB_SHFT                                                                                   0x10
#define HWIO_TCSR_LDO_LD_CTRL_LDO_LD_LSB_BMSK                                                                                   0xff
#define HWIO_TCSR_LDO_LD_CTRL_LDO_LD_LSB_SHFT                                                                                    0x0

#define HWIO_TCSR_LDO_OSC_RESETB_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x00011024)
#define HWIO_TCSR_LDO_OSC_RESETB_RMSK                                                                                     0x80000000
#define HWIO_TCSR_LDO_OSC_RESETB_IN          \
        in_dword_masked(HWIO_TCSR_LDO_OSC_RESETB_ADDR, HWIO_TCSR_LDO_OSC_RESETB_RMSK)
#define HWIO_TCSR_LDO_OSC_RESETB_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_OSC_RESETB_ADDR, m)
#define HWIO_TCSR_LDO_OSC_RESETB_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_OSC_RESETB_ADDR,v)
#define HWIO_TCSR_LDO_OSC_RESETB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_OSC_RESETB_ADDR,m,v,HWIO_TCSR_LDO_OSC_RESETB_IN)
#define HWIO_TCSR_LDO_OSC_RESETB_LDO_OSC_RESETB_BMSK                                                                      0x80000000
#define HWIO_TCSR_LDO_OSC_RESETB_LDO_OSC_RESETB_SHFT                                                                            0x1f

#define HWIO_TCSR_LDO_OSC_CTRL_ADDR                                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x00011028)
#define HWIO_TCSR_LDO_OSC_CTRL_RMSK                                                                                              0x3
#define HWIO_TCSR_LDO_OSC_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_LDO_OSC_CTRL_ADDR, HWIO_TCSR_LDO_OSC_CTRL_RMSK)
#define HWIO_TCSR_LDO_OSC_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_OSC_CTRL_ADDR, m)
#define HWIO_TCSR_LDO_OSC_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_OSC_CTRL_ADDR,v)
#define HWIO_TCSR_LDO_OSC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_OSC_CTRL_ADDR,m,v,HWIO_TCSR_LDO_OSC_CTRL_IN)
#define HWIO_TCSR_LDO_OSC_CTRL_LDO_OSC_CTRL_BMSK                                                                                 0x3
#define HWIO_TCSR_LDO_OSC_CTRL_LDO_OSC_CTRL_SHFT                                                                                 0x0

#define HWIO_TCSR_LDO_DFT_EN_CTRL_ADDR                                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x0001102c)
#define HWIO_TCSR_LDO_DFT_EN_CTRL_RMSK                                                                                    0x80000000
#define HWIO_TCSR_LDO_DFT_EN_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_LDO_DFT_EN_CTRL_ADDR, HWIO_TCSR_LDO_DFT_EN_CTRL_RMSK)
#define HWIO_TCSR_LDO_DFT_EN_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_DFT_EN_CTRL_ADDR, m)
#define HWIO_TCSR_LDO_DFT_EN_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_DFT_EN_CTRL_ADDR,v)
#define HWIO_TCSR_LDO_DFT_EN_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_DFT_EN_CTRL_ADDR,m,v,HWIO_TCSR_LDO_DFT_EN_CTRL_IN)
#define HWIO_TCSR_LDO_DFT_EN_CTRL_LDO_DFT_EN_BMSK                                                                         0x80000000
#define HWIO_TCSR_LDO_DFT_EN_CTRL_LDO_DFT_EN_SHFT                                                                               0x1f

#define HWIO_TCSR_LDO_DFT_CTRL_ADDR                                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x00011030)
#define HWIO_TCSR_LDO_DFT_CTRL_RMSK                                                                                              0x7
#define HWIO_TCSR_LDO_DFT_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_LDO_DFT_CTRL_ADDR, HWIO_TCSR_LDO_DFT_CTRL_RMSK)
#define HWIO_TCSR_LDO_DFT_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_LDO_DFT_CTRL_ADDR, m)
#define HWIO_TCSR_LDO_DFT_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_LDO_DFT_CTRL_ADDR,v)
#define HWIO_TCSR_LDO_DFT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_LDO_DFT_CTRL_ADDR,m,v,HWIO_TCSR_LDO_DFT_CTRL_IN)
#define HWIO_TCSR_LDO_DFT_CTRL_LDO_DFT_CONFIG_BMSK                                                                               0x7
#define HWIO_TCSR_LDO_DFT_CTRL_LDO_DFT_CONFIG_SHFT                                                                               0x0

#define HWIO_TCSR_SPDM_CNT_CLK_CTRL_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00013000)
#define HWIO_TCSR_SPDM_CNT_CLK_CTRL_RMSK                                                                                      0xffff
#define HWIO_TCSR_SPDM_CNT_CLK_CTRL_IN          \
        in_dword_masked(HWIO_TCSR_SPDM_CNT_CLK_CTRL_ADDR, HWIO_TCSR_SPDM_CNT_CLK_CTRL_RMSK)
#define HWIO_TCSR_SPDM_CNT_CLK_CTRL_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPDM_CNT_CLK_CTRL_ADDR, m)
#define HWIO_TCSR_SPDM_CNT_CLK_CTRL_OUT(v)      \
        out_dword(HWIO_TCSR_SPDM_CNT_CLK_CTRL_ADDR,v)
#define HWIO_TCSR_SPDM_CNT_CLK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPDM_CNT_CLK_CTRL_ADDR,m,v,HWIO_TCSR_SPDM_CNT_CLK_CTRL_IN)
#define HWIO_TCSR_SPDM_CNT_CLK_CTRL_SPDM_CNT_CLK_MUX_SEL_BMSK                                                                 0xffff
#define HWIO_TCSR_SPDM_CNT_CLK_CTRL_SPDM_CNT_CLK_MUX_SEL_SHFT                                                                    0x0

#define HWIO_TCSR_SPDM_DLY_FIFO_EN_ADDR                                                                                   (TCSR_TCSR_REGS_REG_BASE      + 0x00013004)
#define HWIO_TCSR_SPDM_DLY_FIFO_EN_RMSK                                                                                   0xffffffff
#define HWIO_TCSR_SPDM_DLY_FIFO_EN_IN          \
        in_dword_masked(HWIO_TCSR_SPDM_DLY_FIFO_EN_ADDR, HWIO_TCSR_SPDM_DLY_FIFO_EN_RMSK)
#define HWIO_TCSR_SPDM_DLY_FIFO_EN_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPDM_DLY_FIFO_EN_ADDR, m)
#define HWIO_TCSR_SPDM_DLY_FIFO_EN_OUT(v)      \
        out_dword(HWIO_TCSR_SPDM_DLY_FIFO_EN_ADDR,v)
#define HWIO_TCSR_SPDM_DLY_FIFO_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPDM_DLY_FIFO_EN_ADDR,m,v,HWIO_TCSR_SPDM_DLY_FIFO_EN_IN)
#define HWIO_TCSR_SPDM_DLY_FIFO_EN_SPDM_DLY_FIFO_EN_BMSK                                                                  0xffffffff
#define HWIO_TCSR_SPDM_DLY_FIFO_EN_SPDM_DLY_FIFO_EN_SHFT                                                                         0x0

#define HWIO_TCSR_SPDM_STG1_MUX_SEL_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00013008)
#define HWIO_TCSR_SPDM_STG1_MUX_SEL_RMSK                                                                                      0xffff
#define HWIO_TCSR_SPDM_STG1_MUX_SEL_IN          \
        in_dword_masked(HWIO_TCSR_SPDM_STG1_MUX_SEL_ADDR, HWIO_TCSR_SPDM_STG1_MUX_SEL_RMSK)
#define HWIO_TCSR_SPDM_STG1_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPDM_STG1_MUX_SEL_ADDR, m)
#define HWIO_TCSR_SPDM_STG1_MUX_SEL_OUT(v)      \
        out_dword(HWIO_TCSR_SPDM_STG1_MUX_SEL_ADDR,v)
#define HWIO_TCSR_SPDM_STG1_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPDM_STG1_MUX_SEL_ADDR,m,v,HWIO_TCSR_SPDM_STG1_MUX_SEL_IN)
#define HWIO_TCSR_SPDM_STG1_MUX_SEL_SPDM_STG1_MUX_SEL_BMSK                                                                    0xffff
#define HWIO_TCSR_SPDM_STG1_MUX_SEL_SPDM_STG1_MUX_SEL_SHFT                                                                       0x0

#define HWIO_TCSR_SPDM_STG2_A_MUX_SEL_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x0001300c)
#define HWIO_TCSR_SPDM_STG2_A_MUX_SEL_RMSK                                                                                0xffffffff
#define HWIO_TCSR_SPDM_STG2_A_MUX_SEL_IN          \
        in_dword_masked(HWIO_TCSR_SPDM_STG2_A_MUX_SEL_ADDR, HWIO_TCSR_SPDM_STG2_A_MUX_SEL_RMSK)
#define HWIO_TCSR_SPDM_STG2_A_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPDM_STG2_A_MUX_SEL_ADDR, m)
#define HWIO_TCSR_SPDM_STG2_A_MUX_SEL_OUT(v)      \
        out_dword(HWIO_TCSR_SPDM_STG2_A_MUX_SEL_ADDR,v)
#define HWIO_TCSR_SPDM_STG2_A_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPDM_STG2_A_MUX_SEL_ADDR,m,v,HWIO_TCSR_SPDM_STG2_A_MUX_SEL_IN)
#define HWIO_TCSR_SPDM_STG2_A_MUX_SEL_SPDM_STG2_A_MUX_SEL_BMSK                                                            0xffffffff
#define HWIO_TCSR_SPDM_STG2_A_MUX_SEL_SPDM_STG2_A_MUX_SEL_SHFT                                                                   0x0

#define HWIO_TCSR_SPDM_STG2_B_MUX_SEL_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00013010)
#define HWIO_TCSR_SPDM_STG2_B_MUX_SEL_RMSK                                                                                0xffffffff
#define HWIO_TCSR_SPDM_STG2_B_MUX_SEL_IN          \
        in_dword_masked(HWIO_TCSR_SPDM_STG2_B_MUX_SEL_ADDR, HWIO_TCSR_SPDM_STG2_B_MUX_SEL_RMSK)
#define HWIO_TCSR_SPDM_STG2_B_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPDM_STG2_B_MUX_SEL_ADDR, m)
#define HWIO_TCSR_SPDM_STG2_B_MUX_SEL_OUT(v)      \
        out_dword(HWIO_TCSR_SPDM_STG2_B_MUX_SEL_ADDR,v)
#define HWIO_TCSR_SPDM_STG2_B_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPDM_STG2_B_MUX_SEL_ADDR,m,v,HWIO_TCSR_SPDM_STG2_B_MUX_SEL_IN)
#define HWIO_TCSR_SPDM_STG2_B_MUX_SEL_SPDM_STG2_B_MUX_SEL_BMSK                                                            0xffffffff
#define HWIO_TCSR_SPDM_STG2_B_MUX_SEL_SPDM_STG2_B_MUX_SEL_SHFT                                                                   0x0

#define HWIO_TCSR_SPDM_STG3_A_MUX_SEL_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00013014)
#define HWIO_TCSR_SPDM_STG3_A_MUX_SEL_RMSK                                                                                0xffffffff
#define HWIO_TCSR_SPDM_STG3_A_MUX_SEL_IN          \
        in_dword_masked(HWIO_TCSR_SPDM_STG3_A_MUX_SEL_ADDR, HWIO_TCSR_SPDM_STG3_A_MUX_SEL_RMSK)
#define HWIO_TCSR_SPDM_STG3_A_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPDM_STG3_A_MUX_SEL_ADDR, m)
#define HWIO_TCSR_SPDM_STG3_A_MUX_SEL_OUT(v)      \
        out_dword(HWIO_TCSR_SPDM_STG3_A_MUX_SEL_ADDR,v)
#define HWIO_TCSR_SPDM_STG3_A_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPDM_STG3_A_MUX_SEL_ADDR,m,v,HWIO_TCSR_SPDM_STG3_A_MUX_SEL_IN)
#define HWIO_TCSR_SPDM_STG3_A_MUX_SEL_SPDM_STG3_A_MUX_SEL_BMSK                                                            0xffffffff
#define HWIO_TCSR_SPDM_STG3_A_MUX_SEL_SPDM_STG3_A_MUX_SEL_SHFT                                                                   0x0

#define HWIO_TCSR_SPDM_STG3_B_MUX_SEL_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00013018)
#define HWIO_TCSR_SPDM_STG3_B_MUX_SEL_RMSK                                                                                0xffffffff
#define HWIO_TCSR_SPDM_STG3_B_MUX_SEL_IN          \
        in_dword_masked(HWIO_TCSR_SPDM_STG3_B_MUX_SEL_ADDR, HWIO_TCSR_SPDM_STG3_B_MUX_SEL_RMSK)
#define HWIO_TCSR_SPDM_STG3_B_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPDM_STG3_B_MUX_SEL_ADDR, m)
#define HWIO_TCSR_SPDM_STG3_B_MUX_SEL_OUT(v)      \
        out_dword(HWIO_TCSR_SPDM_STG3_B_MUX_SEL_ADDR,v)
#define HWIO_TCSR_SPDM_STG3_B_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPDM_STG3_B_MUX_SEL_ADDR,m,v,HWIO_TCSR_SPDM_STG3_B_MUX_SEL_IN)
#define HWIO_TCSR_SPDM_STG3_B_MUX_SEL_SPDM_STG3_B_MUX_SEL_BMSK                                                            0xffffffff
#define HWIO_TCSR_SPDM_STG3_B_MUX_SEL_SPDM_STG3_B_MUX_SEL_SHFT                                                                   0x0

#define HWIO_TCSR_PHSS_USB2_PHY_SEL_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00014000)
#define HWIO_TCSR_PHSS_USB2_PHY_SEL_RMSK                                                                                         0x1
#define HWIO_TCSR_PHSS_USB2_PHY_SEL_IN          \
        in_dword_masked(HWIO_TCSR_PHSS_USB2_PHY_SEL_ADDR, HWIO_TCSR_PHSS_USB2_PHY_SEL_RMSK)
#define HWIO_TCSR_PHSS_USB2_PHY_SEL_INM(m)      \
        in_dword_masked(HWIO_TCSR_PHSS_USB2_PHY_SEL_ADDR, m)
#define HWIO_TCSR_PHSS_USB2_PHY_SEL_OUT(v)      \
        out_dword(HWIO_TCSR_PHSS_USB2_PHY_SEL_ADDR,v)
#define HWIO_TCSR_PHSS_USB2_PHY_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_PHSS_USB2_PHY_SEL_ADDR,m,v,HWIO_TCSR_PHSS_USB2_PHY_SEL_IN)
#define HWIO_TCSR_PHSS_USB2_PHY_SEL_PHSS_USB2_PHY_SEL_BMSK                                                                       0x1
#define HWIO_TCSR_PHSS_USB2_PHY_SEL_PHSS_USB2_PHY_SEL_SHFT                                                                       0x0

#define HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_ADDR                                                                            (TCSR_TCSR_REGS_REG_BASE      + 0x00014004)
#define HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_RMSK                                                                                   0x1
#define HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_IN          \
        in_dword_masked(HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_ADDR, HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_RMSK)
#define HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_INM(m)      \
        in_dword_masked(HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_ADDR, m)
#define HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_OUT(v)      \
        out_dword(HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_ADDR,v)
#define HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_ADDR,m,v,HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_IN)
#define HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_PHSS_ATEST_USB2_PHY_SEL_BMSK                                                           0x1
#define HWIO_TCSR_PHSS_ATEST_USB2_PHY_SEL_PHSS_ATEST_USB2_PHY_SEL_SHFT                                                           0x0

#define HWIO_TCSR_PHSS_TEST_BUS_SEL_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00014008)
#define HWIO_TCSR_PHSS_TEST_BUS_SEL_RMSK                                                                                         0x3
#define HWIO_TCSR_PHSS_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_TCSR_PHSS_TEST_BUS_SEL_ADDR, HWIO_TCSR_PHSS_TEST_BUS_SEL_RMSK)
#define HWIO_TCSR_PHSS_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_TCSR_PHSS_TEST_BUS_SEL_ADDR, m)
#define HWIO_TCSR_PHSS_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_TCSR_PHSS_TEST_BUS_SEL_ADDR,v)
#define HWIO_TCSR_PHSS_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_PHSS_TEST_BUS_SEL_ADDR,m,v,HWIO_TCSR_PHSS_TEST_BUS_SEL_IN)
#define HWIO_TCSR_PHSS_TEST_BUS_SEL_PHSS_TEST_BUS_SEL_BMSK                                                                       0x3
#define HWIO_TCSR_PHSS_TEST_BUS_SEL_PHSS_TEST_BUS_SEL_SHFT                                                                       0x0

#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_ADDR(n)                                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x00014080 + 0x10 * (n))
#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_RMSK                                                                                0x3
#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_MAXn                                                                                  1
#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_ADDR(n), HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_RMSK)
#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_ADDR(n), mask)
#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_ADDR(n),val)
#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_ADDR(n),mask,val,HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_INI(n))
#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_PRONTO_BLSP1_UART_2_IRQ_ENABLE_BMSK                                                 0x2
#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_PRONTO_BLSP1_UART_2_IRQ_ENABLE_SHFT                                                 0x1
#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_PRONTO_BLSP1_UART_1_IRQ_ENABLE_BMSK                                                 0x1
#define HWIO_TCSR_PHSS_UART_PRONTO_INT_SEL_n_PRONTO_BLSP1_UART_1_IRQ_ENABLE_SHFT                                                 0x0

#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_ADDR(n)                                                                         (TCSR_TCSR_REGS_REG_BASE      + 0x000140c0 + 0x10 * (n))
#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_RMSK                                                                                   0x3
#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_MAXn                                                                                     1
#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_ADDR(n), HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_RMSK)
#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_ADDR(n), mask)
#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_ADDR(n),val)
#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_ADDR(n),mask,val,HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_INI(n))
#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_MSS_BLSP1_UART_2_IRQ_ENABLE_BMSK                                                       0x2
#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_MSS_BLSP1_UART_2_IRQ_ENABLE_SHFT                                                       0x1
#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_MSS_BLSP1_UART_1_IRQ_ENABLE_BMSK                                                       0x1
#define HWIO_TCSR_PHSS_UART_MSS_INT_SEL_n_MSS_BLSP1_UART_1_IRQ_ENABLE_SHFT                                                       0x0

#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_ADDR(n)                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x00014100 + 0x10 * (n))
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_RMSK                                                                                0x3f
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_MAXn                                                                                   1
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_ADDR(n), HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_RMSK)
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_ADDR(n), mask)
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_ADDR(n),val)
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_ADDR(n),mask,val,HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_INI(n))
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_6_IRQ_ENABLE_BMSK                                                  0x20
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_6_IRQ_ENABLE_SHFT                                                   0x5
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_5_IRQ_ENABLE_BMSK                                                  0x10
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_5_IRQ_ENABLE_SHFT                                                   0x4
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_4_IRQ_ENABLE_BMSK                                                   0x8
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_4_IRQ_ENABLE_SHFT                                                   0x3
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_3_IRQ_ENABLE_BMSK                                                   0x4
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_3_IRQ_ENABLE_SHFT                                                   0x2
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_2_IRQ_ENABLE_BMSK                                                   0x2
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_2_IRQ_ENABLE_SHFT                                                   0x1
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_1_IRQ_ENABLE_BMSK                                                   0x1
#define HWIO_TCSR_PHSS_QUP_PRONTO_INT_SEL_n_PRONTO_BLSP1_QUP_1_IRQ_ENABLE_SHFT                                                   0x0

#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_ADDR(n)                                                                          (TCSR_TCSR_REGS_REG_BASE      + 0x00014140 + 0x10 * (n))
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_RMSK                                                                                   0x3f
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MAXn                                                                                      1
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_ADDR(n), HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_RMSK)
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_ADDR(n), mask)
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_ADDR(n),val)
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_ADDR(n),mask,val,HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_INI(n))
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_6_IRQ_ENABLE_BMSK                                                        0x20
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_6_IRQ_ENABLE_SHFT                                                         0x5
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_5_IRQ_ENABLE_BMSK                                                        0x10
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_5_IRQ_ENABLE_SHFT                                                         0x4
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_4_IRQ_ENABLE_BMSK                                                         0x8
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_4_IRQ_ENABLE_SHFT                                                         0x3
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_3_IRQ_ENABLE_BMSK                                                         0x4
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_3_IRQ_ENABLE_SHFT                                                         0x2
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_2_IRQ_ENABLE_BMSK                                                         0x2
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_2_IRQ_ENABLE_SHFT                                                         0x1
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_1_IRQ_ENABLE_BMSK                                                         0x1
#define HWIO_TCSR_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_1_IRQ_ENABLE_SHFT                                                         0x0

#define HWIO_TCSR_MEM_ARRY_STBY_ADDR                                                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x00015180)
#define HWIO_TCSR_MEM_ARRY_STBY_RMSK                                                                                             0x1
#define HWIO_TCSR_MEM_ARRY_STBY_IN          \
        in_dword_masked(HWIO_TCSR_MEM_ARRY_STBY_ADDR, HWIO_TCSR_MEM_ARRY_STBY_RMSK)
#define HWIO_TCSR_MEM_ARRY_STBY_INM(m)      \
        in_dword_masked(HWIO_TCSR_MEM_ARRY_STBY_ADDR, m)
#define HWIO_TCSR_MEM_ARRY_STBY_OUT(v)      \
        out_dword(HWIO_TCSR_MEM_ARRY_STBY_ADDR,v)
#define HWIO_TCSR_MEM_ARRY_STBY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MEM_ARRY_STBY_ADDR,m,v,HWIO_TCSR_MEM_ARRY_STBY_IN)
#define HWIO_TCSR_MEM_ARRY_STBY_MEM_ARRY_STBY_N_BMSK                                                                             0x1
#define HWIO_TCSR_MEM_ARRY_STBY_MEM_ARRY_STBY_N_SHFT                                                                             0x0

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

#define HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00016020)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RMSK                                                                                  0x5063ff
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR, HWIO_TCSR_TIMEOUT_INTR_STATUS_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_ULTAUDIO_AHBI_TIMEOUT_IRQ_BMSK                                                        0x400000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_ULTAUDIO_AHBI_TIMEOUT_IRQ_SHFT                                                            0x16
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_MSS_CONFIG_TIMEOUT_IRQ_BMSK                                                           0x100000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_MSS_CONFIG_TIMEOUT_IRQ_SHFT                                                               0x14
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S1_TIMEOUT_IRQ_BMSK                                                                0x4000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S1_TIMEOUT_IRQ_SHFT                                                                   0xe
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S0_TIMEOUT_IRQ_BMSK                                                                0x2000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S0_TIMEOUT_IRQ_SHFT                                                                   0xd
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S9_TIMEOUT_IRQ_BMSK                                                                0x200
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S9_TIMEOUT_IRQ_SHFT                                                                  0x9
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S8_TIMEOUT_IRQ_BMSK                                                                0x100
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S8_TIMEOUT_IRQ_SHFT                                                                  0x8
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S7_TIMEOUT_IRQ_BMSK                                                                 0x80
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S7_TIMEOUT_IRQ_SHFT                                                                  0x7
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S6_TIMEOUT_IRQ_BMSK                                                                 0x40
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S6_TIMEOUT_IRQ_SHFT                                                                  0x6
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S5_TIMEOUT_IRQ_BMSK                                                                 0x20
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S5_TIMEOUT_IRQ_SHFT                                                                  0x5
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S4_TIMEOUT_IRQ_BMSK                                                                 0x10
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S4_TIMEOUT_IRQ_SHFT                                                                  0x4
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S3_TIMEOUT_IRQ_BMSK                                                                  0x8
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S3_TIMEOUT_IRQ_SHFT                                                                  0x3
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S2_TIMEOUT_IRQ_BMSK                                                                  0x4
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S2_TIMEOUT_IRQ_SHFT                                                                  0x2
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S1_TIMEOUT_IRQ_BMSK                                                                  0x2
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S1_TIMEOUT_IRQ_SHFT                                                                  0x1
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S0_TIMEOUT_IRQ_BMSK                                                                  0x1
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S0_TIMEOUT_IRQ_SHFT                                                                  0x0

#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR                                                                            (TCSR_TCSR_REGS_REG_BASE      + 0x00016030)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_RMSK                                                                              0x5063ff
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                         0x400000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                             0x16
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                            0x100000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                0x14
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                 0x4000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                    0xe
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                 0x2000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                    0xd
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S9_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                 0x200
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S9_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                   0x9
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S8_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                 0x100
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S8_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                   0x8
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S7_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                  0x80
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S7_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                   0x7
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S6_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                  0x40
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S6_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                   0x6
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S5_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                  0x20
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S5_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                   0x5
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S4_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                  0x10
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S4_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                   0x4
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S3_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                   0x8
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S3_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                   0x3
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S2_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                   0x4
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S2_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                   0x2
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                   0x2
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                   0x1
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                   0x1
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                   0x0

#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR                                                                           (TCSR_TCSR_REGS_REG_BASE      + 0x00016040)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_RMSK                                                                             0x5063ff
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                       0x400000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                           0x16
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                          0x100000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                              0x14
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                               0x4000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                  0xe
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                               0x2000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                  0xd
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S9_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                               0x200
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S9_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                 0x9
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S8_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                               0x100
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S8_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                 0x8
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                0x80
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                 0x7
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                0x40
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                 0x6
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                0x20
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                 0x5
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                0x10
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                 0x4
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                 0x8
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                 0x3
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                 0x4
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                 0x2
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                 0x2
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                 0x1
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                 0x1
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                 0x0

#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR                                                                            (TCSR_TCSR_REGS_REG_BASE      + 0x00016060)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_RMSK                                                                              0x5063ff
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR, HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                         0x400000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                             0x16
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                            0x100000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                0x14
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                 0x4000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                    0xe
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                 0x2000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                    0xd
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S9_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                 0x200
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S9_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                   0x9
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S8_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                 0x100
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S8_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                   0x8
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                  0x80
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                   0x7
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                  0x40
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                   0x6
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                  0x20
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                   0x5
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                  0x10
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                   0x4
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                   0x8
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                   0x3
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                   0x4
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                   0x2
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                   0x2
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                   0x1
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                   0x1
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                   0x0

#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_ADDR                                                                           (TCSR_TCSR_REGS_REG_BASE      + 0x00016070)
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_RMSK                                                                                  0x1
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_IN          \
        in_dword_masked(HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_ADDR, HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_RMSK)
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_INM(m)      \
        in_dword_masked(HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_ADDR, m)
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_OUT(v)      \
        out_dword(HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_ADDR,v)
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_ADDR,m,v,HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_IN)
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_WCSS_BUS_TIMEOUT_NOC_SOFT_RESET_BMSK                                                  0x1
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_WCSS_BUS_TIMEOUT_NOC_SOFT_RESET_SHFT                                                  0x0

#define HWIO_TCSR_CRYPTO0_HALTREQ_ADDR                                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x00017000)
#define HWIO_TCSR_CRYPTO0_HALTREQ_RMSK                                                                                           0x1
#define HWIO_TCSR_CRYPTO0_HALTREQ_IN          \
        in_dword_masked(HWIO_TCSR_CRYPTO0_HALTREQ_ADDR, HWIO_TCSR_CRYPTO0_HALTREQ_RMSK)
#define HWIO_TCSR_CRYPTO0_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_TCSR_CRYPTO0_HALTREQ_ADDR, m)
#define HWIO_TCSR_CRYPTO0_HALTREQ_OUT(v)      \
        out_dword(HWIO_TCSR_CRYPTO0_HALTREQ_ADDR,v)
#define HWIO_TCSR_CRYPTO0_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_CRYPTO0_HALTREQ_ADDR,m,v,HWIO_TCSR_CRYPTO0_HALTREQ_IN)
#define HWIO_TCSR_CRYPTO0_HALTREQ_CRYPTO0_HALTREQ_BMSK                                                                           0x1
#define HWIO_TCSR_CRYPTO0_HALTREQ_CRYPTO0_HALTREQ_SHFT                                                                           0x0

#define HWIO_TCSR_CRYPTO0_HALTACK_ADDR                                                                                    (TCSR_TCSR_REGS_REG_BASE      + 0x00017004)
#define HWIO_TCSR_CRYPTO0_HALTACK_RMSK                                                                                           0x1
#define HWIO_TCSR_CRYPTO0_HALTACK_IN          \
        in_dword_masked(HWIO_TCSR_CRYPTO0_HALTACK_ADDR, HWIO_TCSR_CRYPTO0_HALTACK_RMSK)
#define HWIO_TCSR_CRYPTO0_HALTACK_INM(m)      \
        in_dword_masked(HWIO_TCSR_CRYPTO0_HALTACK_ADDR, m)
#define HWIO_TCSR_CRYPTO0_HALTACK_CRYPTO0_HALTACK_BMSK                                                                           0x1
#define HWIO_TCSR_CRYPTO0_HALTACK_CRYPTO0_HALTACK_SHFT                                                                           0x0

#define HWIO_TCSR_CRYPTO0_MASTER_IDLE_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00017008)
#define HWIO_TCSR_CRYPTO0_MASTER_IDLE_RMSK                                                                                       0x1
#define HWIO_TCSR_CRYPTO0_MASTER_IDLE_IN          \
        in_dword_masked(HWIO_TCSR_CRYPTO0_MASTER_IDLE_ADDR, HWIO_TCSR_CRYPTO0_MASTER_IDLE_RMSK)
#define HWIO_TCSR_CRYPTO0_MASTER_IDLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_CRYPTO0_MASTER_IDLE_ADDR, m)
#define HWIO_TCSR_CRYPTO0_MASTER_IDLE_CRYPTO0_MASTER_IDLE_BMSK                                                                   0x1
#define HWIO_TCSR_CRYPTO0_MASTER_IDLE_CRYPTO0_MASTER_IDLE_SHFT                                                                   0x0

#define HWIO_TCSR_MSSQ6_HALTREQ_ADDR                                                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x00018000)
#define HWIO_TCSR_MSSQ6_HALTREQ_RMSK                                                                                             0x1
#define HWIO_TCSR_MSSQ6_HALTREQ_IN          \
        in_dword_masked(HWIO_TCSR_MSSQ6_HALTREQ_ADDR, HWIO_TCSR_MSSQ6_HALTREQ_RMSK)
#define HWIO_TCSR_MSSQ6_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_TCSR_MSSQ6_HALTREQ_ADDR, m)
#define HWIO_TCSR_MSSQ6_HALTREQ_OUT(v)      \
        out_dword(HWIO_TCSR_MSSQ6_HALTREQ_ADDR,v)
#define HWIO_TCSR_MSSQ6_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MSSQ6_HALTREQ_ADDR,m,v,HWIO_TCSR_MSSQ6_HALTREQ_IN)
#define HWIO_TCSR_MSSQ6_HALTREQ_MSSQ6_HALTREQ_BMSK                                                                               0x1
#define HWIO_TCSR_MSSQ6_HALTREQ_MSSQ6_HALTREQ_SHFT                                                                               0x0

#define HWIO_TCSR_MSSQ6_HALTACK_ADDR                                                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x00018004)
#define HWIO_TCSR_MSSQ6_HALTACK_RMSK                                                                                             0x1
#define HWIO_TCSR_MSSQ6_HALTACK_IN          \
        in_dword_masked(HWIO_TCSR_MSSQ6_HALTACK_ADDR, HWIO_TCSR_MSSQ6_HALTACK_RMSK)
#define HWIO_TCSR_MSSQ6_HALTACK_INM(m)      \
        in_dword_masked(HWIO_TCSR_MSSQ6_HALTACK_ADDR, m)
#define HWIO_TCSR_MSSQ6_HALTACK_MSSQ6_HALTACK_BMSK                                                                               0x1
#define HWIO_TCSR_MSSQ6_HALTACK_MSSQ6_HALTACK_SHFT                                                                               0x0

#define HWIO_TCSR_MSSQ6_MASTER_IDLE_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00018008)
#define HWIO_TCSR_MSSQ6_MASTER_IDLE_RMSK                                                                                         0x1
#define HWIO_TCSR_MSSQ6_MASTER_IDLE_IN          \
        in_dword_masked(HWIO_TCSR_MSSQ6_MASTER_IDLE_ADDR, HWIO_TCSR_MSSQ6_MASTER_IDLE_RMSK)
#define HWIO_TCSR_MSSQ6_MASTER_IDLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_MSSQ6_MASTER_IDLE_ADDR, m)
#define HWIO_TCSR_MSSQ6_MASTER_IDLE_MSSQ6_MASTER_IDLE_BMSK                                                                       0x1
#define HWIO_TCSR_MSSQ6_MASTER_IDLE_MSSQ6_MASTER_IDLE_SHFT                                                                       0x0

#define HWIO_TCSR_MSSQ6_POWER_STATE_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00018010)
#define HWIO_TCSR_MSSQ6_POWER_STATE_RMSK                                                                                         0x1
#define HWIO_TCSR_MSSQ6_POWER_STATE_IN          \
        in_dword_masked(HWIO_TCSR_MSSQ6_POWER_STATE_ADDR, HWIO_TCSR_MSSQ6_POWER_STATE_RMSK)
#define HWIO_TCSR_MSSQ6_POWER_STATE_INM(m)      \
        in_dword_masked(HWIO_TCSR_MSSQ6_POWER_STATE_ADDR, m)
#define HWIO_TCSR_MSSQ6_POWER_STATE_MSSQ6_POWER_STATE_BMSK                                                                       0x1
#define HWIO_TCSR_MSSQ6_POWER_STATE_MSSQ6_POWER_STATE_SHFT                                                                       0x0

#define HWIO_TCSR_MSS_MODEM_HALTREQ_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00019000)
#define HWIO_TCSR_MSS_MODEM_HALTREQ_RMSK                                                                                         0x1
#define HWIO_TCSR_MSS_MODEM_HALTREQ_IN          \
        in_dword_masked(HWIO_TCSR_MSS_MODEM_HALTREQ_ADDR, HWIO_TCSR_MSS_MODEM_HALTREQ_RMSK)
#define HWIO_TCSR_MSS_MODEM_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_TCSR_MSS_MODEM_HALTREQ_ADDR, m)
#define HWIO_TCSR_MSS_MODEM_HALTREQ_OUT(v)      \
        out_dword(HWIO_TCSR_MSS_MODEM_HALTREQ_ADDR,v)
#define HWIO_TCSR_MSS_MODEM_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MSS_MODEM_HALTREQ_ADDR,m,v,HWIO_TCSR_MSS_MODEM_HALTREQ_IN)
#define HWIO_TCSR_MSS_MODEM_HALTREQ_MSS_MODEM_HALTREQ_BMSK                                                                       0x1
#define HWIO_TCSR_MSS_MODEM_HALTREQ_MSS_MODEM_HALTREQ_SHFT                                                                       0x0

#define HWIO_TCSR_MSS_MODEM_HALTACK_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00019004)
#define HWIO_TCSR_MSS_MODEM_HALTACK_RMSK                                                                                         0x1
#define HWIO_TCSR_MSS_MODEM_HALTACK_IN          \
        in_dword_masked(HWIO_TCSR_MSS_MODEM_HALTACK_ADDR, HWIO_TCSR_MSS_MODEM_HALTACK_RMSK)
#define HWIO_TCSR_MSS_MODEM_HALTACK_INM(m)      \
        in_dword_masked(HWIO_TCSR_MSS_MODEM_HALTACK_ADDR, m)
#define HWIO_TCSR_MSS_MODEM_HALTACK_MSS_MODEM_HALTACK_BMSK                                                                       0x1
#define HWIO_TCSR_MSS_MODEM_HALTACK_MSS_MODEM_HALTACK_SHFT                                                                       0x0

#define HWIO_TCSR_MSS_MODEM_MASTER_IDLE_ADDR                                                                              (TCSR_TCSR_REGS_REG_BASE      + 0x00019008)
#define HWIO_TCSR_MSS_MODEM_MASTER_IDLE_RMSK                                                                                     0x1
#define HWIO_TCSR_MSS_MODEM_MASTER_IDLE_IN          \
        in_dword_masked(HWIO_TCSR_MSS_MODEM_MASTER_IDLE_ADDR, HWIO_TCSR_MSS_MODEM_MASTER_IDLE_RMSK)
#define HWIO_TCSR_MSS_MODEM_MASTER_IDLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_MSS_MODEM_MASTER_IDLE_ADDR, m)
#define HWIO_TCSR_MSS_MODEM_MASTER_IDLE_MSS_MODEM_MASTER_IDLE_BMSK                                                               0x1
#define HWIO_TCSR_MSS_MODEM_MASTER_IDLE_MSS_MODEM_MASTER_IDLE_SHFT                                                               0x0

#define HWIO_TCSR_MSS_NC_HALTREQ_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x0001a000)
#define HWIO_TCSR_MSS_NC_HALTREQ_RMSK                                                                                            0x1
#define HWIO_TCSR_MSS_NC_HALTREQ_IN          \
        in_dword_masked(HWIO_TCSR_MSS_NC_HALTREQ_ADDR, HWIO_TCSR_MSS_NC_HALTREQ_RMSK)
#define HWIO_TCSR_MSS_NC_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_TCSR_MSS_NC_HALTREQ_ADDR, m)
#define HWIO_TCSR_MSS_NC_HALTREQ_OUT(v)      \
        out_dword(HWIO_TCSR_MSS_NC_HALTREQ_ADDR,v)
#define HWIO_TCSR_MSS_NC_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MSS_NC_HALTREQ_ADDR,m,v,HWIO_TCSR_MSS_NC_HALTREQ_IN)
#define HWIO_TCSR_MSS_NC_HALTREQ_MSS_NC_HALTREQ_BMSK                                                                             0x1
#define HWIO_TCSR_MSS_NC_HALTREQ_MSS_NC_HALTREQ_SHFT                                                                             0x0

#define HWIO_TCSR_MSS_NC_HALTACK_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x0001a004)
#define HWIO_TCSR_MSS_NC_HALTACK_RMSK                                                                                            0x1
#define HWIO_TCSR_MSS_NC_HALTACK_IN          \
        in_dword_masked(HWIO_TCSR_MSS_NC_HALTACK_ADDR, HWIO_TCSR_MSS_NC_HALTACK_RMSK)
#define HWIO_TCSR_MSS_NC_HALTACK_INM(m)      \
        in_dword_masked(HWIO_TCSR_MSS_NC_HALTACK_ADDR, m)
#define HWIO_TCSR_MSS_NC_HALTACK_MSS_NC_HALTACK_BMSK                                                                             0x1
#define HWIO_TCSR_MSS_NC_HALTACK_MSS_NC_HALTACK_SHFT                                                                             0x0

#define HWIO_TCSR_MSS_NC_MASTER_IDLE_ADDR                                                                                 (TCSR_TCSR_REGS_REG_BASE      + 0x0001a008)
#define HWIO_TCSR_MSS_NC_MASTER_IDLE_RMSK                                                                                        0x1
#define HWIO_TCSR_MSS_NC_MASTER_IDLE_IN          \
        in_dword_masked(HWIO_TCSR_MSS_NC_MASTER_IDLE_ADDR, HWIO_TCSR_MSS_NC_MASTER_IDLE_RMSK)
#define HWIO_TCSR_MSS_NC_MASTER_IDLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_MSS_NC_MASTER_IDLE_ADDR, m)
#define HWIO_TCSR_MSS_NC_MASTER_IDLE_MSS_NC_MASTER_IDLE_BMSK                                                                     0x1
#define HWIO_TCSR_MSS_NC_MASTER_IDLE_MSS_NC_MASTER_IDLE_SHFT                                                                     0x0

#define HWIO_TCSR_WCSS_HALTREQ_ADDR                                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x0001b000)
#define HWIO_TCSR_WCSS_HALTREQ_RMSK                                                                                              0x1
#define HWIO_TCSR_WCSS_HALTREQ_IN          \
        in_dword_masked(HWIO_TCSR_WCSS_HALTREQ_ADDR, HWIO_TCSR_WCSS_HALTREQ_RMSK)
#define HWIO_TCSR_WCSS_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_TCSR_WCSS_HALTREQ_ADDR, m)
#define HWIO_TCSR_WCSS_HALTREQ_OUT(v)      \
        out_dword(HWIO_TCSR_WCSS_HALTREQ_ADDR,v)
#define HWIO_TCSR_WCSS_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_WCSS_HALTREQ_ADDR,m,v,HWIO_TCSR_WCSS_HALTREQ_IN)
#define HWIO_TCSR_WCSS_HALTREQ_WCSS_HALTREQ_BMSK                                                                                 0x1
#define HWIO_TCSR_WCSS_HALTREQ_WCSS_HALTREQ_SHFT                                                                                 0x0

#define HWIO_TCSR_WCSS_HALTACK_ADDR                                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x0001b004)
#define HWIO_TCSR_WCSS_HALTACK_RMSK                                                                                              0x1
#define HWIO_TCSR_WCSS_HALTACK_IN          \
        in_dword_masked(HWIO_TCSR_WCSS_HALTACK_ADDR, HWIO_TCSR_WCSS_HALTACK_RMSK)
#define HWIO_TCSR_WCSS_HALTACK_INM(m)      \
        in_dword_masked(HWIO_TCSR_WCSS_HALTACK_ADDR, m)
#define HWIO_TCSR_WCSS_HALTACK_WCSS_HALTACK_BMSK                                                                                 0x1
#define HWIO_TCSR_WCSS_HALTACK_WCSS_HALTACK_SHFT                                                                                 0x0

#define HWIO_TCSR_WCSS_MASTER_IDLE_ADDR                                                                                   (TCSR_TCSR_REGS_REG_BASE      + 0x0001b008)
#define HWIO_TCSR_WCSS_MASTER_IDLE_RMSK                                                                                          0x1
#define HWIO_TCSR_WCSS_MASTER_IDLE_IN          \
        in_dword_masked(HWIO_TCSR_WCSS_MASTER_IDLE_ADDR, HWIO_TCSR_WCSS_MASTER_IDLE_RMSK)
#define HWIO_TCSR_WCSS_MASTER_IDLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_WCSS_MASTER_IDLE_ADDR, m)
#define HWIO_TCSR_WCSS_MASTER_IDLE_WCSS_MASTER_IDLE_BMSK                                                                         0x1
#define HWIO_TCSR_WCSS_MASTER_IDLE_WCSS_MASTER_IDLE_SHFT                                                                         0x0

#define HWIO_TCSR_SDC1_HALTREQ_ADDR                                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x0001c000)
#define HWIO_TCSR_SDC1_HALTREQ_RMSK                                                                                              0x1
#define HWIO_TCSR_SDC1_HALTREQ_IN          \
        in_dword_masked(HWIO_TCSR_SDC1_HALTREQ_ADDR, HWIO_TCSR_SDC1_HALTREQ_RMSK)
#define HWIO_TCSR_SDC1_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_TCSR_SDC1_HALTREQ_ADDR, m)
#define HWIO_TCSR_SDC1_HALTREQ_OUT(v)      \
        out_dword(HWIO_TCSR_SDC1_HALTREQ_ADDR,v)
#define HWIO_TCSR_SDC1_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SDC1_HALTREQ_ADDR,m,v,HWIO_TCSR_SDC1_HALTREQ_IN)
#define HWIO_TCSR_SDC1_HALTREQ_SDC1_HALTREQ_BMSK                                                                                 0x1
#define HWIO_TCSR_SDC1_HALTREQ_SDC1_HALTREQ_SHFT                                                                                 0x0

#define HWIO_TCSR_SDC1_HALTACK_ADDR                                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x0001c004)
#define HWIO_TCSR_SDC1_HALTACK_RMSK                                                                                              0x1
#define HWIO_TCSR_SDC1_HALTACK_IN          \
        in_dword_masked(HWIO_TCSR_SDC1_HALTACK_ADDR, HWIO_TCSR_SDC1_HALTACK_RMSK)
#define HWIO_TCSR_SDC1_HALTACK_INM(m)      \
        in_dword_masked(HWIO_TCSR_SDC1_HALTACK_ADDR, m)
#define HWIO_TCSR_SDC1_HALTACK_SDC1_HALTACK_BMSK                                                                                 0x1
#define HWIO_TCSR_SDC1_HALTACK_SDC1_HALTACK_SHFT                                                                                 0x0

#define HWIO_TCSR_SDC1_MASTER_IDLE_ADDR                                                                                   (TCSR_TCSR_REGS_REG_BASE      + 0x0001c008)
#define HWIO_TCSR_SDC1_MASTER_IDLE_RMSK                                                                                          0x1
#define HWIO_TCSR_SDC1_MASTER_IDLE_IN          \
        in_dword_masked(HWIO_TCSR_SDC1_MASTER_IDLE_ADDR, HWIO_TCSR_SDC1_MASTER_IDLE_RMSK)
#define HWIO_TCSR_SDC1_MASTER_IDLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_SDC1_MASTER_IDLE_ADDR, m)
#define HWIO_TCSR_SDC1_MASTER_IDLE_SDC1_MASTER_IDLE_BMSK                                                                         0x1
#define HWIO_TCSR_SDC1_MASTER_IDLE_SDC1_MASTER_IDLE_SHFT                                                                         0x0

#define HWIO_TCSR_SDC2_HALTREQ_ADDR                                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x0001d000)
#define HWIO_TCSR_SDC2_HALTREQ_RMSK                                                                                              0x1
#define HWIO_TCSR_SDC2_HALTREQ_IN          \
        in_dword_masked(HWIO_TCSR_SDC2_HALTREQ_ADDR, HWIO_TCSR_SDC2_HALTREQ_RMSK)
#define HWIO_TCSR_SDC2_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_TCSR_SDC2_HALTREQ_ADDR, m)
#define HWIO_TCSR_SDC2_HALTREQ_OUT(v)      \
        out_dword(HWIO_TCSR_SDC2_HALTREQ_ADDR,v)
#define HWIO_TCSR_SDC2_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SDC2_HALTREQ_ADDR,m,v,HWIO_TCSR_SDC2_HALTREQ_IN)
#define HWIO_TCSR_SDC2_HALTREQ_SDC2_HALTREQ_BMSK                                                                                 0x1
#define HWIO_TCSR_SDC2_HALTREQ_SDC2_HALTREQ_SHFT                                                                                 0x0

#define HWIO_TCSR_SDC2_HALTACK_ADDR                                                                                       (TCSR_TCSR_REGS_REG_BASE      + 0x0001d004)
#define HWIO_TCSR_SDC2_HALTACK_RMSK                                                                                              0x1
#define HWIO_TCSR_SDC2_HALTACK_IN          \
        in_dword_masked(HWIO_TCSR_SDC2_HALTACK_ADDR, HWIO_TCSR_SDC2_HALTACK_RMSK)
#define HWIO_TCSR_SDC2_HALTACK_INM(m)      \
        in_dword_masked(HWIO_TCSR_SDC2_HALTACK_ADDR, m)
#define HWIO_TCSR_SDC2_HALTACK_SDC2_HALTACK_BMSK                                                                                 0x1
#define HWIO_TCSR_SDC2_HALTACK_SDC2_HALTACK_SHFT                                                                                 0x0

#define HWIO_TCSR_SDC2_MASTER_IDLE_ADDR                                                                                   (TCSR_TCSR_REGS_REG_BASE      + 0x0001d008)
#define HWIO_TCSR_SDC2_MASTER_IDLE_RMSK                                                                                          0x1
#define HWIO_TCSR_SDC2_MASTER_IDLE_IN          \
        in_dword_masked(HWIO_TCSR_SDC2_MASTER_IDLE_ADDR, HWIO_TCSR_SDC2_MASTER_IDLE_RMSK)
#define HWIO_TCSR_SDC2_MASTER_IDLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_SDC2_MASTER_IDLE_ADDR, m)
#define HWIO_TCSR_SDC2_MASTER_IDLE_SDC2_MASTER_IDLE_BMSK                                                                         0x1
#define HWIO_TCSR_SDC2_MASTER_IDLE_SDC2_MASTER_IDLE_SHFT                                                                         0x0

#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_ADDR                                                                           (TCSR_TCSR_REGS_REG_BASE      + 0x0001e000)
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_RMSK                                                                                  0x1
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_IN          \
        in_dword_masked(HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_ADDR, HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_RMSK)
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_INM(m)      \
        in_dword_masked(HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_ADDR, m)
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_OUT(v)      \
        out_dword(HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_ADDR,v)
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_ADDR,m,v,HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_IN)
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_ULT_AUDIO_CORE_ABORT_REQ_BMSK                                                         0x1
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_REQ_ULT_AUDIO_CORE_ABORT_REQ_SHFT                                                         0x0

#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_ACK_ADDR                                                                           (TCSR_TCSR_REGS_REG_BASE      + 0x0001e004)
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_ACK_RMSK                                                                                  0x1
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_ACK_IN          \
        in_dword_masked(HWIO_TCSR_ULT_AUDIO_CORE_ABORT_ACK_ADDR, HWIO_TCSR_ULT_AUDIO_CORE_ABORT_ACK_RMSK)
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_ACK_INM(m)      \
        in_dword_masked(HWIO_TCSR_ULT_AUDIO_CORE_ABORT_ACK_ADDR, m)
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_ACK_ULT_AUDIO_CORE_ABORT_ACK_BMSK                                                         0x1
#define HWIO_TCSR_ULT_AUDIO_CORE_ABORT_ACK_ULT_AUDIO_CORE_ABORT_ACK_SHFT                                                         0x0

#define HWIO_TCSR_SPARE_REG0_ADDR                                                                                         (TCSR_TCSR_REGS_REG_BASE      + 0x0001f000)
#define HWIO_TCSR_SPARE_REG0_RMSK                                                                                         0xffffffff
#define HWIO_TCSR_SPARE_REG0_IN          \
        in_dword_masked(HWIO_TCSR_SPARE_REG0_ADDR, HWIO_TCSR_SPARE_REG0_RMSK)
#define HWIO_TCSR_SPARE_REG0_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPARE_REG0_ADDR, m)
#define HWIO_TCSR_SPARE_REG0_OUT(v)      \
        out_dword(HWIO_TCSR_SPARE_REG0_ADDR,v)
#define HWIO_TCSR_SPARE_REG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPARE_REG0_ADDR,m,v,HWIO_TCSR_SPARE_REG0_IN)
#define HWIO_TCSR_SPARE_REG0_SPARE_REG0_BMSK                                                                              0xffffffff
#define HWIO_TCSR_SPARE_REG0_SPARE_REG0_SHFT                                                                                     0x0

#define HWIO_TCSR_SPARE_REG1_ADDR                                                                                         (TCSR_TCSR_REGS_REG_BASE      + 0x0001f004)
#define HWIO_TCSR_SPARE_REG1_RMSK                                                                                         0xffffffff
#define HWIO_TCSR_SPARE_REG1_IN          \
        in_dword_masked(HWIO_TCSR_SPARE_REG1_ADDR, HWIO_TCSR_SPARE_REG1_RMSK)
#define HWIO_TCSR_SPARE_REG1_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPARE_REG1_ADDR, m)
#define HWIO_TCSR_SPARE_REG1_OUT(v)      \
        out_dword(HWIO_TCSR_SPARE_REG1_ADDR,v)
#define HWIO_TCSR_SPARE_REG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPARE_REG1_ADDR,m,v,HWIO_TCSR_SPARE_REG1_IN)
#define HWIO_TCSR_SPARE_REG1_SPARE_REG1_BMSK                                                                              0xffffffff
#define HWIO_TCSR_SPARE_REG1_SPARE_REG1_SHFT                                                                                     0x0

#define HWIO_TCSR_SPARE_REG2_ADDR                                                                                         (TCSR_TCSR_REGS_REG_BASE      + 0x0001f008)
#define HWIO_TCSR_SPARE_REG2_RMSK                                                                                         0xffffffff
#define HWIO_TCSR_SPARE_REG2_IN          \
        in_dword_masked(HWIO_TCSR_SPARE_REG2_ADDR, HWIO_TCSR_SPARE_REG2_RMSK)
#define HWIO_TCSR_SPARE_REG2_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPARE_REG2_ADDR, m)
#define HWIO_TCSR_SPARE_REG2_OUT(v)      \
        out_dword(HWIO_TCSR_SPARE_REG2_ADDR,v)
#define HWIO_TCSR_SPARE_REG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPARE_REG2_ADDR,m,v,HWIO_TCSR_SPARE_REG2_IN)
#define HWIO_TCSR_SPARE_REG2_SPARE_REG2_BMSK                                                                              0xffffffff
#define HWIO_TCSR_SPARE_REG2_SPARE_REG2_SHFT                                                                                     0x0

#define HWIO_TCSR_SPARE_REG3_ADDR                                                                                         (TCSR_TCSR_REGS_REG_BASE      + 0x0001f00c)
#define HWIO_TCSR_SPARE_REG3_RMSK                                                                                         0xffffffff
#define HWIO_TCSR_SPARE_REG3_IN          \
        in_dword_masked(HWIO_TCSR_SPARE_REG3_ADDR, HWIO_TCSR_SPARE_REG3_RMSK)
#define HWIO_TCSR_SPARE_REG3_INM(m)      \
        in_dword_masked(HWIO_TCSR_SPARE_REG3_ADDR, m)
#define HWIO_TCSR_SPARE_REG3_OUT(v)      \
        out_dword(HWIO_TCSR_SPARE_REG3_ADDR,v)
#define HWIO_TCSR_SPARE_REG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_SPARE_REG3_ADDR,m,v,HWIO_TCSR_SPARE_REG3_IN)
#define HWIO_TCSR_SPARE_REG3_SPARE_REG3_BMSK                                                                              0xffffffff
#define HWIO_TCSR_SPARE_REG3_SPARE_REG3_SHFT                                                                                     0x0

#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_ADDR                                                                                  (TCSR_TCSR_REGS_REG_BASE      + 0x00020000)
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_RMSK                                                                                       0x11f
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_IN          \
        in_dword_masked(HWIO_TCSR_PNOC_SNOC_MEMTYPE_ADDR, HWIO_TCSR_PNOC_SNOC_MEMTYPE_RMSK)
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_INM(m)      \
        in_dword_masked(HWIO_TCSR_PNOC_SNOC_MEMTYPE_ADDR, m)
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_OUT(v)      \
        out_dword(HWIO_TCSR_PNOC_SNOC_MEMTYPE_ADDR,v)
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_PNOC_SNOC_MEMTYPE_ADDR,m,v,HWIO_TCSR_PNOC_SNOC_MEMTYPE_IN)
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_MEM_TYPE_SEL_BMSK                                                                          0x100
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_MEM_TYPE_SEL_SHFT                                                                            0x8
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_INNERSHARED_VALUE_BMSK                                                                      0x10
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_INNERSHARED_VALUE_SHFT                                                                       0x4
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_SHARED_VALUE_BMSK                                                                            0x8
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_SHARED_VALUE_SHFT                                                                            0x3
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_MEM_TYPE_VALUE_BMSK                                                                          0x7
#define HWIO_TCSR_PNOC_SNOC_MEMTYPE_MEM_TYPE_VALUE_SHFT                                                                          0x0


#endif /* __CHIPINFOHWIO_H__ */
