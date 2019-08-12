#ifndef __CPR_QFPROM_H__
#define __CPR_QFPROM_H__
/*
===========================================================================
*/
/**
  @file cpr_qfprom.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8996 (Istari) [istari_v1.0_p2q1r8.1.7_MTO]
 
  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE

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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/cpr/common/target/8996/cpr_qfprom.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/
#ifndef WINSIM
#include "msmhwiobase.h"
#endif

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                        (SECURITY_CONTROL_BASE      + 0x00000000)

#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_ADDR(n)                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000000 + 0x4 * (n))
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_MAXn                                                          71
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_ADDR(n), HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_RMSK)
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_CRI_CM_PRIVATE_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_CRI_CM_PRIVATE_SHFT                                          0x0

#define HWIO_QFPROM_RAW_ACC_PRIVATE_FEAT_PROVn_ADDR(n)                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000120 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATE_FEAT_PROVn_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATE_FEAT_PROVn_MAXn                                                    3
#define HWIO_QFPROM_RAW_ACC_PRIVATE_FEAT_PROVn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATE_FEAT_PROVn_ADDR(n), HWIO_QFPROM_RAW_ACC_PRIVATE_FEAT_PROVn_RMSK)
#define HWIO_QFPROM_RAW_ACC_PRIVATE_FEAT_PROVn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATE_FEAT_PROVn_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ACC_PRIVATE_FEAT_PROVn_ACC_PRIVATE_FEAT_PROV_BMSK                     0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATE_FEAT_PROVn_ACC_PRIVATE_FEAT_PROV_SHFT                            0x0

#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000130)
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_PTE_DATA0_BMSK                                           0xe0000000
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_PTE_DATA0_SHFT                                                 0x1d
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_MACCHIATO_EN_BMSK                                        0x10000000
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_MACCHIATO_EN_SHFT                                              0x1c
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_FEATURE_ID_BMSK                                           0xff00000
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_FEATURE_ID_SHFT                                                0x14
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_JTAG_ID_BMSK                                                0xfffff
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_JTAG_ID_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000134)
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_PTE_DATA1_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_PTE_DATA1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000138)
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_SERIAL_NUM_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_SERIAL_NUM_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000013c)
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_PTE_DATA1_BMSK                                           0xffff0000
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_PTE_DATA1_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_CHIP_ID_BMSK                                                 0xffff
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_CHIP_ID_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000140)
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_PTE_DATA0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_PTE_DATA0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000144)
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_PTE_DATA1_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_PTE_DATA1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000148)
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_PTE_DATA0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_PTE_DATA0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000014c)
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_PTE_DATA1_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_PTE_DATA1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000150)
#define HWIO_QFPROM_RAW_RD_PERM_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_RD_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR, HWIO_QFPROM_RAW_RD_PERM_LSB_RMSK)
#define HWIO_QFPROM_RAW_RD_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_PERM_LSB_IN)
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG31_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG31_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG30_BMSK                                      0x40000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG30_SHFT                                            0x1e
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG29_BMSK                                      0x20000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG29_SHFT                                            0x1d
#define HWIO_QFPROM_RAW_RD_PERM_LSB_HDCP_KEY_BMSK                                             0x10000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_HDCP_KEY_SHFT                                                   0x1c
#define HWIO_QFPROM_RAW_RD_PERM_LSB_BOOT_ROM_PATCH_BMSK                                        0x8000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_BOOT_ROM_PATCH_SHFT                                             0x1b
#define HWIO_QFPROM_RAW_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                0x4000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                     0x1a
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                0x2000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                     0x19
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SEC_BOOT_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SEC_BOOT_SHFT                                               0x18
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_BMSK                                     0x800000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_SHFT                                         0x17
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_IMAGE_ENCR_KEY_BMSK                                      0x400000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_IMAGE_ENCR_KEY_SHFT                                          0x16
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG21_BMSK                                         0x200000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG21_SHFT                                             0x15
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG20_BMSK                                         0x100000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG20_SHFT                                             0x14
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG19_BMSK                                          0x80000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG19_SHFT                                             0x13
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG18_BMSK                                          0x40000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG18_SHFT                                             0x12
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG17_BMSK                                          0x20000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG17_SHFT                                             0x11
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG16_BMSK                                          0x10000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG16_SHFT                                             0x10
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG15_BMSK                                           0x8000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG15_SHFT                                              0xf
#define HWIO_QFPROM_RAW_RD_PERM_LSB_MEM_CONFIG_BMSK                                               0x4000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_MEM_CONFIG_SHFT                                                  0xe
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CALIB_BMSK                                                    0x2000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CALIB_SHFT                                                       0xd
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PK_HASH0_BMSK                                                 0x1000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PK_HASH0_SHFT                                                    0xc
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEAT_CONFIG_BMSK                                               0x800
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEAT_CONFIG_SHFT                                                 0xb
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_CONFIG_BMSK                                                0x400
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_CONFIG_SHFT                                                  0xa
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_4_BMSK                                           0x200
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_4_SHFT                                             0x9
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                           0x100
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                             0x8
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                            0x80
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                             0x7
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                            0x40
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                             0x6
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEC_EN_BMSK                                                     0x20
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEC_EN_SHFT                                                      0x5
#define HWIO_QFPROM_RAW_RD_PERM_LSB_WR_PERM_BMSK                                                    0x10
#define HWIO_QFPROM_RAW_RD_PERM_LSB_WR_PERM_SHFT                                                     0x4
#define HWIO_QFPROM_RAW_RD_PERM_LSB_RD_PERM_BMSK                                                     0x8
#define HWIO_QFPROM_RAW_RD_PERM_LSB_RD_PERM_SHFT                                                     0x3
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PTE_BMSK                                                         0x4
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PTE_SHFT                                                         0x2
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ACC_PRIVATE_FEAT_PROV_BMSK                                       0x2
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ACC_PRIVATE_FEAT_PROV_SHFT                                       0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CRI_CM_PRIVATE_BMSK                                              0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CRI_CM_PRIVATE_SHFT                                              0x0

#define HWIO_QFPROM_RAW_RD_PERM_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000154)
#define HWIO_QFPROM_RAW_RD_PERM_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_RD_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_PERM_MSB_ADDR, HWIO_QFPROM_RAW_RD_PERM_MSB_RMSK)
#define HWIO_QFPROM_RAW_RD_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_PERM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_PERM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_PERM_MSB_IN)
#define HWIO_QFPROM_RAW_RD_PERM_MSB_RSVD0_BMSK                                                0xffff8000
#define HWIO_QFPROM_RAW_RD_PERM_MSB_RSVD0_SHFT                                                       0xf
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG46_BMSK                                          0x4000
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG46_SHFT                                             0xe
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG45_BMSK                                          0x2000
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG45_SHFT                                             0xd
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG44_BMSK                                          0x1000
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG44_SHFT                                             0xc
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG43_BMSK                                           0x800
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG43_SHFT                                             0xb
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG42_BMSK                                           0x400
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG42_SHFT                                             0xa
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG41_BMSK                                           0x200
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG41_SHFT                                             0x9
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG40_BMSK                                           0x100
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG40_SHFT                                             0x8
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG39_BMSK                                            0x80
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG39_SHFT                                             0x7
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG38_BMSK                                            0x40
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG38_SHFT                                             0x6
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG37_BMSK                                            0x20
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG37_SHFT                                             0x5
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG36_BMSK                                            0x10
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG36_SHFT                                             0x4
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG35_BMSK                                             0x8
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG35_SHFT                                             0x3
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG34_BMSK                                             0x4
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG34_SHFT                                             0x2
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG33_BMSK                                             0x2
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG33_SHFT                                             0x1
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG32_BMSK                                             0x1
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OEM_SPARE_REG32_SHFT                                             0x0

#define HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000158)
#define HWIO_QFPROM_RAW_WR_PERM_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR, HWIO_QFPROM_RAW_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_RAW_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_WR_PERM_LSB_IN)
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG31_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG31_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG30_BMSK                                      0x40000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG30_SHFT                                            0x1e
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG29_BMSK                                      0x20000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG29_SHFT                                            0x1d
#define HWIO_QFPROM_RAW_WR_PERM_LSB_HDCP_KEY_BMSK                                             0x10000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_HDCP_KEY_SHFT                                                   0x1c
#define HWIO_QFPROM_RAW_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                        0x8000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                             0x1b
#define HWIO_QFPROM_RAW_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                0x4000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                     0x1a
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                0x2000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                     0x19
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                               0x18
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_BMSK                                     0x800000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_SHFT                                         0x17
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_IMAGE_ENCR_KEY_BMSK                                      0x400000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_IMAGE_ENCR_KEY_SHFT                                          0x16
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG21_BMSK                                         0x200000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG21_SHFT                                             0x15
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG20_BMSK                                         0x100000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG20_SHFT                                             0x14
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG19_BMSK                                          0x80000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG19_SHFT                                             0x13
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG18_BMSK                                          0x40000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG18_SHFT                                             0x12
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG17_BMSK                                          0x20000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG17_SHFT                                             0x11
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG16_BMSK                                          0x10000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG16_SHFT                                             0x10
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG15_BMSK                                           0x8000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG15_SHFT                                              0xf
#define HWIO_QFPROM_RAW_WR_PERM_LSB_MEM_CONFIG_BMSK                                               0x4000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_MEM_CONFIG_SHFT                                                  0xe
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CALIB_BMSK                                                    0x2000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CALIB_SHFT                                                       0xd
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PK_HASH0_BMSK                                                 0x1000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PK_HASH0_SHFT                                                    0xc
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEAT_CONFIG_BMSK                                               0x800
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEAT_CONFIG_SHFT                                                 0xb
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_CONFIG_BMSK                                                0x400
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_CONFIG_SHFT                                                  0xa
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_4_BMSK                                           0x200
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_4_SHFT                                             0x9
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                           0x100
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                             0x8
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                            0x80
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                             0x7
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                            0x40
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                             0x6
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEC_EN_BMSK                                                     0x20
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEC_EN_SHFT                                                      0x5
#define HWIO_QFPROM_RAW_WR_PERM_LSB_WR_PERM_BMSK                                                    0x10
#define HWIO_QFPROM_RAW_WR_PERM_LSB_WR_PERM_SHFT                                                     0x4
#define HWIO_QFPROM_RAW_WR_PERM_LSB_RD_PERM_BMSK                                                     0x8
#define HWIO_QFPROM_RAW_WR_PERM_LSB_RD_PERM_SHFT                                                     0x3
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PTE_BMSK                                                         0x4
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PTE_SHFT                                                         0x2
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ACC_PRIVATE_FEAT_PROV_BMSK                                       0x2
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ACC_PRIVATE_FEAT_PROV_SHFT                                       0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CRI_CM_PRIVATE_BMSK                                              0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CRI_CM_PRIVATE_SHFT                                              0x0

#define HWIO_QFPROM_RAW_WR_PERM_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000015c)
#define HWIO_QFPROM_RAW_WR_PERM_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_WR_PERM_MSB_ADDR, HWIO_QFPROM_RAW_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_RAW_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_WR_PERM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_WR_PERM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_WR_PERM_MSB_IN)
#define HWIO_QFPROM_RAW_WR_PERM_MSB_RSVD0_BMSK                                                0xffff8000
#define HWIO_QFPROM_RAW_WR_PERM_MSB_RSVD0_SHFT                                                       0xf
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG46_BMSK                                          0x4000
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG46_SHFT                                             0xe
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG45_BMSK                                          0x2000
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG45_SHFT                                             0xd
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG44_BMSK                                          0x1000
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG44_SHFT                                             0xc
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG43_BMSK                                           0x800
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG43_SHFT                                             0xb
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG42_BMSK                                           0x400
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG42_SHFT                                             0xa
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG41_BMSK                                           0x200
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG41_SHFT                                             0x9
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG40_BMSK                                           0x100
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG40_SHFT                                             0x8
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG39_BMSK                                            0x80
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG39_SHFT                                             0x7
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG38_BMSK                                            0x40
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG38_SHFT                                             0x6
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG37_BMSK                                            0x20
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG37_SHFT                                             0x5
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG36_BMSK                                            0x10
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG36_SHFT                                             0x4
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG35_BMSK                                             0x8
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG35_SHFT                                             0x3
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG34_BMSK                                             0x4
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG34_SHFT                                             0x2
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG33_BMSK                                             0x2
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG33_SHFT                                             0x1
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG32_BMSK                                             0x1
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OEM_SPARE_REG32_SHFT                                             0x0

#define HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000160)
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

#define HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000164)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_MSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_RSVD0_BMSK                                                 0xffff8000
#define HWIO_QFPROM_RAW_FEC_EN_MSB_RSVD0_SHFT                                                        0xf
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION46_FEC_EN_BMSK                                           0x4000
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION46_FEC_EN_SHFT                                              0xe
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION45_FEC_EN_BMSK                                           0x2000
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION45_FEC_EN_SHFT                                              0xd
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION44_FEC_EN_BMSK                                           0x1000
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION44_FEC_EN_SHFT                                              0xc
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION43_FEC_EN_BMSK                                            0x800
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION43_FEC_EN_SHFT                                              0xb
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION42_FEC_EN_BMSK                                            0x400
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION42_FEC_EN_SHFT                                              0xa
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION41_FEC_EN_BMSK                                            0x200
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION41_FEC_EN_SHFT                                              0x9
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION40_FEC_EN_BMSK                                            0x100
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION40_FEC_EN_SHFT                                              0x8
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION39_FEC_EN_BMSK                                             0x80
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION39_FEC_EN_SHFT                                              0x7
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION38_FEC_EN_BMSK                                             0x40
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION38_FEC_EN_SHFT                                              0x6
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION37_FEC_EN_BMSK                                             0x20
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION37_FEC_EN_SHFT                                              0x5
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION36_FEC_EN_BMSK                                             0x10
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION36_FEC_EN_SHFT                                              0x4
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION35_FEC_EN_BMSK                                              0x8
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION35_FEC_EN_SHFT                                              0x3
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION34_FEC_EN_BMSK                                              0x4
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION34_FEC_EN_SHFT                                              0x2
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION33_FEC_EN_BMSK                                              0x2
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION33_FEC_EN_SHFT                                              0x1
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION32_FEC_EN_BMSK                                              0x1
#define HWIO_QFPROM_RAW_FEC_EN_MSB_REGION32_FEC_EN_SHFT                                              0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000168)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_XBL0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_XBL0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000016c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_XBL1_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_XBL1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000170)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_PIL_SUBSYSTEM_31_0_BMSK                           0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_PIL_SUBSYSTEM_31_0_SHFT                                  0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000174)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RSVD0_BMSK                                        0xfe000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RSVD0_SHFT                                              0x19
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_BMSK                                           0x1fe0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_SHFT                                                0x11
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_TZ_BMSK                                              0x1ffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_TZ_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000178)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RSVD1_BMSK                                        0xc0000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RSVD1_SHFT                                              0x1e
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_TQS_HASH_ACTIVE_BMSK                              0x3e000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_TQS_HASH_ACTIVE_SHFT                                    0x19
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_BMSK                          0x1000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_SHFT                               0x18
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_PIL_SUBSYSTEM_47_32_BMSK                            0xffff00
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_PIL_SUBSYSTEM_47_32_SHFT                                 0x8
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RSVD0_BMSK                                              0xff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RSVD0_SHFT                                               0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000017c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_BMSK               0xfffff000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_SHFT                      0xc
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_HYPERVISOR_BMSK                                        0xfff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_HYPERVISOR_SHFT                                          0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000180)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_MSS_BMSK                                          0xffff0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_MSS_SHFT                                                0x10
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_MBA_BMSK                                              0xffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_MBA_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000184)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_BMSK               0xffffff00
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_SHFT                      0x8
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_RSVD0_BMSK                                              0xff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_RSVD0_SHFT                                               0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000188)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_BMSK                               0x80000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_SHFT                                     0x1f
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_BMSK                               0x40000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_SHFT                                     0x1e
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_BMSK                            0x20000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_SHFT                                  0x1d
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_BMSK                         0x10000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_SHFT                               0x1c
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD0_BMSK                                         0xe000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD0_SHFT                                              0x19
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_HASH_INTEGRITY_CHECK_DISABLE_BMSK              0x1000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                   0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_BMSK              0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                  0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_USB_SS_DISABLE_BMSK                                 0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_USB_SS_DISABLE_SHFT                                     0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_BMSK                          0x200000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_SHFT                              0x15
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_BMSK                           0x100000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_SHFT                               0x14
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_BMSK                         0x80000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_SHFT                            0x13
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC1_SCM_FORCE_EFUSE_KEY_BMSK                        0x40000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC1_SCM_FORCE_EFUSE_KEY_SHFT                           0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_UFS_SCM_FORCE_EFUSE_KEY_BMSK                         0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_UFS_SCM_FORCE_EFUSE_KEY_SHFT                            0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC2_SCM_FORCE_EFUSE_KEY_BMSK                        0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC2_SCM_FORCE_EFUSE_KEY_SHFT                           0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                                  0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                     0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WDOG_EN_BMSK                                          0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WDOG_EN_SHFT                                             0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_BMSK                                 0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_SHFT                                    0xd
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SW_FUSE_PROG_DISABLE_BMSK                             0x1000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SW_FUSE_PROG_DISABLE_SHFT                                0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                     0x800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                       0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_BMSK                               0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_SHFT                                 0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                         0x3e0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                           0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                 0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_BMSK                              0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_SHFT                              0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_BMSK                                 0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_SHFT                                 0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_BMSK                                        0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_SHFT                                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                     0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                     0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000018c)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS0_SPNIDEN_DISABLE_BMSK                        0x80000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS0_SPNIDEN_DISABLE_SHFT                              0x1f
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_BMSK                            0x40000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_SHFT                                  0x1e
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_GSS_A5_NIDEN_DISABLE_BMSK                         0x20000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_GSS_A5_NIDEN_DISABLE_SHFT                               0x1d
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SSC_NIDEN_DISABLE_BMSK                            0x10000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SSC_NIDEN_DISABLE_SHFT                                  0x1c
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_PIMEM_NIDEN_DISABLE_BMSK                           0x8000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_PIMEM_NIDEN_DISABLE_SHFT                                0x1b
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_NIDEN_DISABLE_BMSK                             0x4000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_NIDEN_DISABLE_SHFT                                  0x1a
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WCSS_NIDEN_DISABLE_BMSK                            0x2000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WCSS_NIDEN_DISABLE_SHFT                                 0x19
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_LPASS_NIDEN_DISABLE_BMSK                           0x1000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_LPASS_NIDEN_DISABLE_SHFT                                0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                              0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                  0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS1_NIDEN_DISABLE_BMSK                            0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS1_NIDEN_DISABLE_SHFT                                0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS0_NIDEN_DISABLE_BMSK                            0x200000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS0_NIDEN_DISABLE_SHFT                                0x15
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_BMSK                              0x100000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_SHFT                                  0x14
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_GSS_A5_DBGEN_DISABLE_BMSK                            0x80000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_GSS_A5_DBGEN_DISABLE_SHFT                               0x13
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_A5X_ISDB_DBGEN_DISABLE_BMSK                          0x40000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_A5X_ISDB_DBGEN_DISABLE_SHFT                             0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_BMSK                           0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_SHFT                              0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SSC_Q6_DBGEN_DISABLE_BMSK                            0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SSC_Q6_DBGEN_DISABLE_SHFT                               0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SSC_DBGEN_DISABLE_BMSK                                0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SSC_DBGEN_DISABLE_SHFT                                   0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_PIMEM_DBGEN_DISABLE_BMSK                              0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_PIMEM_DBGEN_DISABLE_SHFT                                 0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_BMSK                                0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_SHFT                                   0xd
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_BMSK                               0x1000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_SHFT                                  0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_LPASS_DBGEN_DISABLE_BMSK                               0x800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_LPASS_DBGEN_DISABLE_SHFT                                 0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                                 0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                   0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS1_DBGEN_DISABLE_BMSK                               0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS1_DBGEN_DISABLE_SHFT                                 0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS0_DBGEN_DISABLE_BMSK                               0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS0_DBGEN_DISABLE_SHFT                                 0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                               0x80
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                                0x7
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_BMSK                                  0x40
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_SHFT                                   0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SSC_Q6_ETM_DISABLE_BMSK                                 0x20
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SSC_Q6_ETM_DISABLE_SHFT                                  0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE6_DISABLE_BMSK                                     0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE6_DISABLE_SHFT                                      0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE5_DISABLE_BMSK                                      0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE5_DISABLE_SHFT                                      0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE4_DISABLE_BMSK                                      0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE4_DISABLE_SHFT                                      0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE3_DISABLE_BMSK                                      0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE3_DISABLE_SHFT                                      0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE2_DISABLE_BMSK                                      0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE2_DISABLE_SHFT                                      0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000190)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG46_SECURE_BMSK                       0x80000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG46_SECURE_SHFT                             0x1f
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG45_SECURE_BMSK                       0x40000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG45_SECURE_SHFT                             0x1e
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG44_SECURE_BMSK                       0x20000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG44_SECURE_SHFT                             0x1d
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG43_SECURE_BMSK                       0x10000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG43_SECURE_SHFT                             0x1c
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG42_SECURE_BMSK                        0x8000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG42_SECURE_SHFT                             0x1b
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG41_SECURE_BMSK                        0x4000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG41_SECURE_SHFT                             0x1a
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG40_SECURE_BMSK                        0x2000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG40_SECURE_SHFT                             0x19
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG39_SECURE_BMSK                        0x1000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG39_SECURE_SHFT                             0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG38_SECURE_BMSK                         0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG38_SECURE_SHFT                             0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG37_SECURE_BMSK                         0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG37_SECURE_SHFT                             0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG36_SECURE_BMSK                         0x200000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG36_SECURE_SHFT                             0x15
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG35_SECURE_BMSK                         0x100000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG35_SECURE_SHFT                             0x14
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG34_SECURE_BMSK                          0x80000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG34_SECURE_SHFT                             0x13
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG33_SECURE_BMSK                          0x40000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG33_SECURE_SHFT                             0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG32_SECURE_BMSK                          0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG32_SECURE_SHFT                             0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG31_SECURE_BMSK                          0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG31_SECURE_SHFT                             0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG30_SECURE_BMSK                           0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG30_SECURE_SHFT                              0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG29_SECURE_BMSK                           0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG29_SECURE_SHFT                              0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RSVD0_BMSK                                            0x3800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RSVD0_SHFT                                               0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_GSS_A5_SPIDEN_DISABLE_BMSK                             0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_GSS_A5_SPIDEN_DISABLE_SHFT                               0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_SSC_SPIDEN_DISABLE_BMSK                                0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_SSC_SPIDEN_DISABLE_SHFT                                  0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_PIMEM_SPIDEN_DISABLE_BMSK                              0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_PIMEM_SPIDEN_DISABLE_SHFT                                0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_DAP_SPIDEN_DISABLE_BMSK                                 0x80
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_DAP_SPIDEN_DISABLE_SHFT                                  0x7
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_APPS1_SPIDEN_DISABLE_BMSK                               0x40
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_APPS1_SPIDEN_DISABLE_SHFT                                0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_APPS0_SPIDEN_DISABLE_BMSK                               0x20
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_APPS0_SPIDEN_DISABLE_SHFT                                0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_GSS_A5_SPNIDEN_DISABLE_BMSK                             0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_GSS_A5_SPNIDEN_DISABLE_SHFT                              0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_SSC_SPNIDEN_DISABLE_BMSK                                 0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_SSC_SPNIDEN_DISABLE_SHFT                                 0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_PIMEM_SPNIDEN_DISABLE_BMSK                               0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_PIMEM_SPNIDEN_DISABLE_SHFT                               0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_DAP_SPNIDEN_DISABLE_BMSK                                 0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_DAP_SPNIDEN_DISABLE_SHFT                                 0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_APPS1_SPNIDEN_DISABLE_BMSK                               0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_APPS1_SPNIDEN_DISABLE_SHFT                               0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000194)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_BMSK                               0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_SHFT                                     0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_HW_ID_BMSK                                        0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_HW_ID_SHFT                                           0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000198)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_PERIPH_VID_BMSK                                   0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_PERIPH_VID_SHFT                                         0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_PERIPH_PID_BMSK                                       0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_PERIPH_PID_SHFT                                          0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000019c)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_RSVD0_BMSK                                        0xffffff00
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_RSVD0_SHFT                                               0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                           0xff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                            0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a0)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_UFS_SW_CONTROL_DISABLE_BMSK                      0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_UFS_SW_CONTROL_DISABLE_SHFT                            0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SATA_DISABLE_BMSK                                0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SATA_DISABLE_SHFT                                      0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SECURE_CHANNEL_DISABLE_BMSK                      0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SECURE_CHANNEL_DISABLE_SHFT                            0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SCM_DISABLE_BMSK                                 0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SCM_DISABLE_SHFT                                       0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ICE_FORCE_HW_KEY1_BMSK                            0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ICE_FORCE_HW_KEY1_SHFT                                 0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ICE_FORCE_HW_KEY0_BMSK                            0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ICE_FORCE_HW_KEY0_SHFT                                 0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ICE_DISABLE_BMSK                                  0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ICE_DISABLE_SHFT                                       0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RICA_DISABLE_BMSK                                 0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RICA_DISABLE_SHFT                                      0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_BMSK                             0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_SHFT                                 0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_BMSK                              0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_SHFT                                  0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_SEL1_BMSK                              0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_SEL1_SHFT                                  0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_SEL0_BMSK                              0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_SEL0_SHFT                                  0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_DISABLE_BMSK                            0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_DISABLE_SHFT                               0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_ENCODE_DISABLE_BMSK                      0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_ENCODE_DISABLE_SHFT                         0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_DECODE_DISABLE_BMSK                      0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_DECODE_DISABLE_SHFT                         0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VENUS_4K_DISABLE_BMSK                               0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VENUS_4K_DISABLE_SHFT                                  0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                             0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                                0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                             0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                                0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_BMSK                                    0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_SHFT                                       0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_BMSK                                    0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_SHFT                                       0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MDP_APICAL_LTC_DISABLE_BMSK                           0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MDP_APICAL_LTC_DISABLE_SHFT                             0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_BMSK                                      0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_SHFT                                        0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                                    0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                      0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_BMSK                                    0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_SHFT                                      0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_FD_DISABLE_BMSK                                        0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_FD_DISABLE_SHFT                                         0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_DPCM_14_DISABLE_BMSK                              0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_DPCM_14_DISABLE_SHFT                               0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_BMSK                                     0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_SHFT                                      0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_BMSK                                    0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_SHFT                                     0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_BMSK                                     0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_SHFT                                     0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_BMSK                             0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_SHFT                             0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a4)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD0_BMSK                                       0xff800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD0_SHFT                                             0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APS_RESET_DISABLE_BMSK                             0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APS_RESET_DISABLE_SHFT                                 0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_HVX_DISABLE_BMSK                                   0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_HVX_DISABLE_SHFT                                       0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APB2JTAG_LIFECYCLE1_DIS_BMSK                       0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APB2JTAG_LIFECYCLE1_DIS_SHFT                           0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APB2JTAG_LIFECYCLE0_EN_BMSK                         0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APB2JTAG_LIFECYCLE0_EN_SHFT                            0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_STACKED_MEMORY_ID_BMSK                              0x7c000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_STACKED_MEMORY_ID_SHFT                                  0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PRNG_TESTMODE_DISABLE_BMSK                           0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PRNG_TESTMODE_DISABLE_SHFT                              0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_DCC_DISABLE_BMSK                                     0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_DCC_DISABLE_SHFT                                        0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MOCHA_PART_BMSK                                       0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MOCHA_PART_SHFT                                         0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_NIDNT_DISABLE_BMSK                                    0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_NIDNT_DISABLE_SHFT                                      0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SMMU_DISABLE_BMSK                                     0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SMMU_DISABLE_SHFT                                       0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_BMSK                                      0x1e0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_SHFT                                        0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                       0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                        0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_NAV_DISABLE_BMSK                                        0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_NAV_DISABLE_SHFT                                        0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_2_DISABLE_BMSK                                     0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_2_DISABLE_SHFT                                     0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_1_DISABLE_BMSK                                     0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_1_DISABLE_SHFT                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_0_DISABLE_BMSK                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_0_DISABLE_SHFT                                     0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a8)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RSVD0_BMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RSVD0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001ac)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RSVD0_BMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RSVD0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b0)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_DAP_NIDEN_DISABLE_BMSK                        0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_DAP_NIDEN_DISABLE_SHFT                              0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_APPS1_NIDEN_DISABLE_BMSK                      0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_APPS1_NIDEN_DISABLE_SHFT                            0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_APPS0_NIDEN_DISABLE_BMSK                      0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_APPS0_NIDEN_DISABLE_SHFT                            0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_MSS_DBGEN_DISABLE_BMSK                        0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_MSS_DBGEN_DISABLE_SHFT                              0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_GSS_A5_DBGEN_DISABLE_BMSK                      0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_GSS_A5_DBGEN_DISABLE_SHFT                           0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_A5X_ISDB_DBGEN_DISABLE_BMSK                    0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_A5X_ISDB_DBGEN_DISABLE_SHFT                         0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                     0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                          0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SSC_Q6_DBGEN_DISABLE_BMSK                      0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SSC_Q6_DBGEN_DISABLE_SHFT                           0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SSC_DBGEN_DISABLE_BMSK                          0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SSC_DBGEN_DISABLE_SHFT                              0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_PIMEM_DBGEN_DISABLE_BMSK                        0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_PIMEM_DBGEN_DISABLE_SHFT                            0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_RPM_DBGEN_DISABLE_BMSK                          0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_RPM_DBGEN_DISABLE_SHFT                              0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                         0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                             0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_LPASS_DBGEN_DISABLE_BMSK                         0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_LPASS_DBGEN_DISABLE_SHFT                            0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_DAP_DBGEN_DISABLE_BMSK                           0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_DAP_DBGEN_DISABLE_SHFT                              0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_APPS1_DBGEN_DISABLE_BMSK                         0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_APPS1_DBGEN_DISABLE_SHFT                            0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_APPS0_DBGEN_DISABLE_BMSK                         0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_APPS0_DBGEN_DISABLE_SHFT                            0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                         0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                            0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_RPM_DAPEN_DISABLE_BMSK                            0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_RPM_DAPEN_DISABLE_SHFT                               0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SSC_Q6_ETM_DISABLE_BMSK                           0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SSC_Q6_ETM_DISABLE_SHFT                              0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE6_DISABLE_BMSK                               0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE6_DISABLE_SHFT                                  0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE5_DISABLE_BMSK                                0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE5_DISABLE_SHFT                                  0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE4_DISABLE_BMSK                                0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE4_DISABLE_SHFT                                  0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE3_DISABLE_BMSK                                0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE3_DISABLE_SHFT                                  0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE2_DISABLE_BMSK                                0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE2_DISABLE_SHFT                                  0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE1_DISABLE_BMSK                                 0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE1_DISABLE_SHFT                                  0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE0_DISABLE_BMSK                                 0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_SPARE0_DISABLE_SHFT                                  0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_GPMU_NIDEN_DISABLE_BMSK                             0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_GPMU_NIDEN_DISABLE_SHFT                              0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_GPMU_DBGEN_DISABLE_BMSK                             0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_GPMU_DBGEN_DISABLE_SHFT                              0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_GPMU_DAPEN_DISABLE_BMSK                              0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QC_GPMU_DAPEN_DISABLE_SHFT                              0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QDI_SPMI_DISABLE_BMSK                                   0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_QDI_SPMI_DISABLE_SHFT                                   0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_SM_BIST_DISABLE_BMSK                                    0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_SM_BIST_DISABLE_SHFT                                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TIC_DISABLE_BMSK                                        0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TIC_DISABLE_SHFT                                        0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b4)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGCRYPTODISABLE_BMSK                 0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGCRYPTODISABLE_SHFT                       0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3V_BMSK                  0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3V_SHFT                        0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3TE_BMSK                 0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3TE_SHFT                       0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3EE_BMSK                 0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3EE_SHFT                       0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS1_CFGCPUPRESENT_N_BMSK                        0xc000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS1_CFGCPUPRESENT_N_SHFT                             0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS0_CFGCPUPRESENT_N_BMSK                        0x3000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS0_CFGCPUPRESENT_N_SHFT                             0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                         0xf80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                             0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_SPIDEN_DISABLE_BMSK                       0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_SPIDEN_DISABLE_SHFT                          0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_SSC_SPIDEN_DISABLE_BMSK                          0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_SSC_SPIDEN_DISABLE_SHFT                             0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_SPIDEN_DISABLE_BMSK                        0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_SPIDEN_DISABLE_SHFT                           0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPIDEN_DISABLE_BMSK                           0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPIDEN_DISABLE_SHFT                              0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_APPS1_SPIDEN_DISABLE_BMSK                         0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_APPS1_SPIDEN_DISABLE_SHFT                            0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_APPS0_SPIDEN_DISABLE_BMSK                         0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_APPS0_SPIDEN_DISABLE_SHFT                            0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_SPNIDEN_DISABLE_BMSK                       0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_SPNIDEN_DISABLE_SHFT                          0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_SSC_SPNIDEN_DISABLE_BMSK                           0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_SSC_SPNIDEN_DISABLE_SHFT                             0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_SPNIDEN_DISABLE_BMSK                         0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_SPNIDEN_DISABLE_SHFT                           0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPNIDEN_DISABLE_BMSK                           0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPNIDEN_DISABLE_SHFT                             0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_APPS1_SPNIDEN_DISABLE_BMSK                         0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_APPS1_SPNIDEN_DISABLE_SHFT                           0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_APPS0_SPNIDEN_DISABLE_BMSK                          0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_APPS0_SPNIDEN_DISABLE_SHFT                           0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_MSS_NIDEN_DISABLE_BMSK                              0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_MSS_NIDEN_DISABLE_SHFT                               0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_NIDEN_DISABLE_BMSK                           0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_NIDEN_DISABLE_SHFT                            0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_SSC_NIDEN_DISABLE_BMSK                              0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_SSC_NIDEN_DISABLE_SHFT                               0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_NIDEN_DISABLE_BMSK                             0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_NIDEN_DISABLE_SHFT                             0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_RPM_NIDEN_DISABLE_BMSK                               0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_RPM_NIDEN_DISABLE_SHFT                               0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_WCSS_NIDEN_DISABLE_BMSK                              0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_WCSS_NIDEN_DISABLE_SHFT                              0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_LPASS_NIDEN_DISABLE_BMSK                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_QC_LPASS_NIDEN_DISABLE_SHFT                             0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b8)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_TAP_GEN_SPARE_INSTR_DISABLE_13_0_BMSK            0xfffc0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_TAP_GEN_SPARE_INSTR_DISABLE_13_0_SHFT                  0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_TAP_INSTR_DISABLE_BMSK                              0x3ffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_TAP_INSTR_DISABLE_SHFT                                  0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001bc)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                      0xfffc0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                            0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_TAP_GEN_SPARE_INSTR_DISABLE_31_14_BMSK              0x3ffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_TAP_GEN_SPARE_INSTR_DISABLE_31_14_SHFT                  0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001c0)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_MODEM_PBL_PATCH_VERSION_BMSK                     0xf8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_MODEM_PBL_PATCH_VERSION_SHFT                           0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_APPS_PBL_PATCH_VERSION_BMSK                       0x7c00000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_APPS_PBL_PATCH_VERSION_SHFT                            0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_RSVD0_BMSK                                         0x3c0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_RSVD0_SHFT                                             0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_APPS_PBL_BOOT_SPEED_BMSK                            0x30000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_APPS_PBL_BOOT_SPEED_SHFT                               0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_RPM_PBL_BOOT_SPEED_BMSK                              0xc000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_RPM_PBL_BOOT_SPEED_SHFT                                 0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_APPS_BOOT_FROM_ROM_BMSK                              0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_APPS_BOOT_FROM_ROM_SHFT                                 0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_RPM_BOOT_FROM_ROM_BMSK                               0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_RPM_BOOT_FROM_ROM_SHFT                                  0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_MODEM_BOOT_FROM_ROM_BMSK                              0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_MODEM_BOOT_FROM_ROM_SHFT                                0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_MSA_ENA_BMSK                                          0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_MSA_ENA_SHFT                                            0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_FORCE_MSA_AUTH_EN_BMSK                                0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_FORCE_MSA_AUTH_EN_SHFT                                  0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_ARM_CE_DISABLE_USAGE_BMSK                             0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_ARM_CE_DISABLE_USAGE_SHFT                               0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_BOOT_ROM_CFG_BMSK                                      0xff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_LSB_BOOT_ROM_CFG_SHFT                                       0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001c4)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_RSVD0_BMSK                                       0xffffc000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_RSVD0_SHFT                                              0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_FOUNDRY_ID_BMSK                                      0x3c00
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_FOUNDRY_ID_SHFT                                         0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_PLL_CFG_BMSK                                          0x3f0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_PLL_CFG_SHFT                                            0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_APPS_PBL_PLL_CTRL_BMSK                                  0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW4_MSB_APPS_PBL_PLL_CTRL_SHFT                                  0x0

#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001c8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_MAXn                                                         3
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_HASH_DATA0_BMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_HASH_DATA0_SHFT                                            0x0

#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001cc + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_MAXn                                                         3
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_HASH_DATA1_BMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_HASH_DATA1_SHFT                                            0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001e8)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_APPS_APC0_LDO_VREF_FUNC_TRIM_BMSK                      0xf8000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_APPS_APC0_LDO_VREF_FUNC_TRIM_SHFT                            0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_APPS_APC0_LDO_VREF_RET_TRIM_BMSK                        0x7c00000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_APPS_APC0_LDO_VREF_RET_TRIM_SHFT                             0x16
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_GP_HW_CALIB_BMSK                                         0x3c0000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_GP_HW_CALIB_SHFT                                             0x12
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_Q6SS1_LDO_VREF_TRIM_BMSK                                  0x3fc00
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_Q6SS1_LDO_VREF_TRIM_SHFT                                      0xa
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_Q6SS1_LDO_ENABLE_BMSK                                       0x200
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_Q6SS1_LDO_ENABLE_SHFT                                         0x9
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_Q6SS0_LDO_VREF_TRIM_BMSK                                    0x1fe
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_Q6SS0_LDO_VREF_TRIM_SHFT                                      0x1
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_Q6SS0_LDO_ENABLE_BMSK                                         0x1
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_Q6SS0_LDO_ENABLE_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001ec)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_SPARE0_BMSK                                            0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_SPARE0_SHFT                                                  0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC0_CPU1_IMEAS_BHS_MODE_ACC_TRIM_BMSK            0x3e000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC0_CPU1_IMEAS_BHS_MODE_ACC_TRIM_SHFT                  0x19
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC0_CPU0_IMEAS_BHS_MODE_ACC_TRIM_BMSK             0x1f00000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC0_CPU0_IMEAS_BHS_MODE_ACC_TRIM_SHFT                  0x14
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC1_L2_IMEAS_BHS_MODE_ACC_TRIM_BMSK                 0xf8000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC1_L2_IMEAS_BHS_MODE_ACC_TRIM_SHFT                     0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC0_L2_IMEAS_BHS_MODE_ACC_TRIM_BMSK                  0x7c00
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC0_L2_IMEAS_BHS_MODE_ACC_TRIM_SHFT                     0xa
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC1_LDO_VREF_FUNC_TRIM_BMSK                           0x3e0
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC1_LDO_VREF_FUNC_TRIM_SHFT                             0x5
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC1_LDO_VREF_RET_TRIM_BMSK                             0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_APPS_APC1_LDO_VREF_RET_TRIM_SHFT                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f0)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_SPARE0_BMSK                                            0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_SPARE0_SHFT                                                  0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC0_CPU1_IMEAS_LDO_MODE_ACC_TRIM_BMSK            0x3e000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC0_CPU1_IMEAS_LDO_MODE_ACC_TRIM_SHFT                  0x19
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC0_CPU0_IMEAS_LDO_MODE_ACC_TRIM_BMSK             0x1f00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC0_CPU0_IMEAS_LDO_MODE_ACC_TRIM_SHFT                  0x14
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC1_ODCM_DAC_VREF_BMSK                              0xc0000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC1_ODCM_DAC_VREF_SHFT                                 0x12
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC1_MEMCELL_VMIN_VREF_TRIM_BMSK                     0x38000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC1_MEMCELL_VMIN_VREF_TRIM_SHFT                         0xf
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC0_ODCM_DAC_VREF_BMSK                               0x6000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC0_ODCM_DAC_VREF_SHFT                                  0xd
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC0_MEMCELL_VMIN_VREF_TRIM_BMSK                      0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC0_MEMCELL_VMIN_VREF_TRIM_SHFT                         0xa
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC1_CPU1_IMEAS_BHS_MODE_ACC_TRIM_BMSK                 0x3e0
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC1_CPU1_IMEAS_BHS_MODE_ACC_TRIM_SHFT                   0x5
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC1_CPU0_IMEAS_BHS_MODE_ACC_TRIM_BMSK                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS_APC1_CPU0_IMEAS_BHS_MODE_ACC_TRIM_SHFT                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f4)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SPARE0_BMSK                                            0xf8000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SPARE0_SHFT                                                  0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_BANDGAP_TRIM_BMSK                                       0x7f00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_BANDGAP_TRIM_SHFT                                            0x14
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_APPS_APC1_L2_IMEAS_LDO_MODE_ACC_TRIM_BMSK                 0xf8000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_APPS_APC1_L2_IMEAS_LDO_MODE_ACC_TRIM_SHFT                     0xf
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_APPS_APC0_L2_IMEAS_LDO_MODE_ACC_TRIM_BMSK                  0x7c00
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_APPS_APC0_L2_IMEAS_LDO_MODE_ACC_TRIM_SHFT                     0xa
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_APPS_APC1_CPU1_IMEAS_LDO_MODE_ACC_TRIM_BMSK                 0x3e0
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_APPS_APC1_CPU1_IMEAS_LDO_MODE_ACC_TRIM_SHFT                   0x5
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_APPS_APC1_CPU0_IMEAS_LDO_MODE_ACC_TRIM_BMSK                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_APPS_APC1_CPU0_IMEAS_LDO_MODE_ACC_TRIM_SHFT                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f8)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR1_TARG_VOLT_SVS_1_0_BMSK                            0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR1_TARG_VOLT_SVS_1_0_SHFT                                  0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR1_TARG_VOLT_NOM_BMSK                                0x3e000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR1_TARG_VOLT_NOM_SHFT                                      0x19
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR1_TARG_VOLT_TUR_BMSK                                 0x1f00000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR1_TARG_VOLT_TUR_SHFT                                      0x14
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS2_BMSK                                  0xf8000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS2_SHFT                                      0xf
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS_BMSK                                    0x7c00
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS_SHFT                                       0xa
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_NOM_BMSK                                     0x3e0
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_NOM_SHFT                                       0x5
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_TUR_BMSK                                      0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_TUR_SHFT                                       0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001fc)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR3_TARG_VOLT_TUR_3_0_BMSK                            0xf0000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR3_TARG_VOLT_TUR_3_0_SHFT                                  0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR2_TARG_VOLT_SVS2_BMSK                                0xf800000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR2_TARG_VOLT_SVS2_SHFT                                     0x17
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR2_TARG_VOLT_SVS_BMSK                                  0x7c0000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR2_TARG_VOLT_SVS_SHFT                                      0x12
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR2_TARG_VOLT_NOM_BMSK                                   0x3e000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR2_TARG_VOLT_NOM_SHFT                                       0xd
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR2_TARG_VOLT_TUR_BMSK                                    0x1f00
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR2_TARG_VOLT_TUR_SHFT                                       0x8
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS2_BMSK                                     0xf8
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS2_SHFT                                      0x3
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS_4_2_BMSK                                   0x7
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS_4_2_SHFT                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000200)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR5_TARG_VOLT_NOM_0_BMSK                              0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR5_TARG_VOLT_NOM_0_SHFT                                    0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR4_TARG_VOLT_SUTUR_BMSK                              0x7c000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR4_TARG_VOLT_SUTUR_SHFT                                    0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR4_TARG_VOLT_NOM_BMSK                                 0x3e00000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR4_TARG_VOLT_NOM_SHFT                                      0x15
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR4_TARG_VOLT_TUR_BMSK                                  0x1f0000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR4_TARG_VOLT_TUR_SHFT                                      0x10
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR3_TARG_VOLT_SVS2_BMSK                                   0xf800
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR3_TARG_VOLT_SVS2_SHFT                                      0xb
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR3_TARG_VOLT_SVS_BMSK                                     0x7c0
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR3_TARG_VOLT_SVS_SHFT                                       0x6
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR3_TARG_VOLT_NOM_BMSK                                      0x3e
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR3_TARG_VOLT_NOM_SHFT                                       0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR3_TARG_VOLT_TUR_4_BMSK                                     0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_CPR3_TARG_VOLT_TUR_4_SHFT                                     0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000204)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR6_TARG_VOLT_TUR_1_0_BMSK                            0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR6_TARG_VOLT_TUR_1_0_SHFT                                  0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR6_SVS2_ROSEL_BMSK                                   0x3c000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR6_SVS2_ROSEL_SHFT                                         0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR6_SVS_ROSEL_BMSK                                     0x3c00000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR6_SVS_ROSEL_SHFT                                          0x16
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR6_NOMINAL_ROSEL_BMSK                                  0x3c0000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR6_NOMINAL_ROSEL_SHFT                                      0x12
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR6_TURBO_ROSEL_BMSK                                     0x3c000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR6_TURBO_ROSEL_SHFT                                         0xe
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR5_TARG_VOLT_SVS2_BMSK                                   0x3e00
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR5_TARG_VOLT_SVS2_SHFT                                      0x9
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR5_TARG_VOLT_SVS_BMSK                                     0x1f0
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR5_TARG_VOLT_SVS_SHFT                                       0x4
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR5_TARG_VOLT_NOM_4_1_BMSK                                   0xf
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR5_TARG_VOLT_NOM_4_1_SHFT                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000208)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CPR6_TURBO_QUOT_VMIN_9_0_BMSK                          0xffc00000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CPR6_TURBO_QUOT_VMIN_9_0_SHFT                                0x16
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CPR6_TARG_VOLT_SVS2_BMSK                                 0x3f0000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CPR6_TARG_VOLT_SVS2_SHFT                                     0x10
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CPR6_TARG_VOLT_SVS_BMSK                                    0xfc00
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CPR6_TARG_VOLT_SVS_SHFT                                       0xa
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CPR6_TARG_VOLT_NOM_BMSK                                     0x3f0
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CPR6_TARG_VOLT_NOM_SHFT                                       0x4
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CPR6_TARG_VOLT_TUR_5_2_BMSK                                   0xf
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CPR6_TARG_VOLT_TUR_5_2_SHFT                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000020c)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CPR6_SVS2_QUOT_VMIN_5_0_BMSK                           0xfc000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CPR6_SVS2_QUOT_VMIN_5_0_SHFT                                 0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CPR6_SVS_QUOT_VMIN_BMSK                                 0x3ffc000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CPR6_SVS_QUOT_VMIN_SHFT                                       0xe
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CPR6_NOMINAL_QUOT_VMIN_BMSK                                0x3ffc
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CPR6_NOMINAL_QUOT_VMIN_SHFT                                   0x2
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CPR6_TURBO_QUOT_VMIN_11_10_BMSK                               0x3
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CPR6_TURBO_QUOT_VMIN_11_10_SHFT                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000210)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR7_TURBO_ROSEL_1_0_BMSK                              0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR7_TURBO_ROSEL_1_0_SHFT                                    0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR6_SVS_QUOT_OFFSET_BMSK                              0x3fc00000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR6_SVS_QUOT_OFFSET_SHFT                                    0x16
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR6_NOMINAL_QUOT_OFFSET_BMSK                            0x3fc000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR6_NOMINAL_QUOT_OFFSET_SHFT                                 0xe
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR6_TURBO_QUOT_OFFSET_BMSK                                0x3fc0
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR6_TURBO_QUOT_OFFSET_SHFT                                   0x6
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR6_SVS2_QUOT_VMIN_11_6_BMSK                                0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR6_SVS2_QUOT_VMIN_11_6_SHFT                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000214)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_TARG_VOLT_SVS_BMSK                                0xfc000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_TARG_VOLT_SVS_SHFT                                      0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_TARG_VOLT_NOM_BMSK                                 0x3f00000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_TARG_VOLT_NOM_SHFT                                      0x14
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_TARG_VOLT_TUR_BMSK                                   0xfc000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_TARG_VOLT_TUR_SHFT                                       0xe
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_SVS2_ROSEL_BMSK                                       0x3c00
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_SVS2_ROSEL_SHFT                                          0xa
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_SVS_ROSEL_BMSK                                         0x3c0
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_SVS_ROSEL_SHFT                                           0x6
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_NOMINAL_ROSEL_BMSK                                      0x3c
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_NOMINAL_ROSEL_SHFT                                       0x2
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_TURBO_ROSEL_3_2_BMSK                                     0x3
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR7_TURBO_ROSEL_3_2_SHFT                                     0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000218)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR7_SVS_QUOT_VMIN_1_0_BMSK                            0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR7_SVS_QUOT_VMIN_1_0_SHFT                                  0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR7_NOMINAL_QUOT_VMIN_BMSK                            0x3ffc0000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR7_NOMINAL_QUOT_VMIN_SHFT                                  0x12
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR7_TURBO_QUOT_VMIN_BMSK                                 0x3ffc0
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR7_TURBO_QUOT_VMIN_SHFT                                     0x6
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR7_TARG_VOLT_SVS2_BMSK                                     0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR7_TARG_VOLT_SVS2_SHFT                                      0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000021c)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CPR7_NOMINAL_QUOT_OFFSET_1_0_BMSK                      0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CPR7_NOMINAL_QUOT_OFFSET_1_0_SHFT                            0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CPR7_TURBO_QUOT_OFFSET_BMSK                            0x3fc00000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CPR7_TURBO_QUOT_OFFSET_SHFT                                  0x16
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CPR7_SVS2_QUOT_VMIN_BMSK                                 0x3ffc00
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CPR7_SVS2_QUOT_VMIN_SHFT                                      0xa
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CPR7_SVS_QUOT_VMIN_11_2_BMSK                                0x3ff
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CPR7_SVS_QUOT_VMIN_11_2_SHFT                                  0x0

#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000220)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW7_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_GFX_AGING_RO_DELTA_BMSK                                0xfc000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_GFX_AGING_RO_DELTA_SHFT                                      0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CX_AGING_RO_DELTA_BMSK                                  0x3f00000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CX_AGING_RO_DELTA_SHFT                                       0x14
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_APC_AGING_RO_DELTA_BMSK                                   0xfc000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_APC_AGING_RO_DELTA_SHFT                                       0xe
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR7_SVS_QUOT_OFFSET_BMSK                                  0x3fc0
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR7_SVS_QUOT_OFFSET_SHFT                                     0x6
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR7_NOMINAL_QUOT_OFFSET_7_2_BMSK                            0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR7_NOMINAL_QUOT_OFFSET_7_2_SHFT                             0x0

#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000224)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW7_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_PH_B0M0_BMSK                                           0xe0000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_PH_B0M0_SHFT                                                 0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_G_B0_BMSK                                              0x1c000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_G_B0_SHFT                                                    0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_SAR_B0_BMSK                                             0x3000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_SAR_B0_SHFT                                                  0x18
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_APPS_APC_ODCM_CAL_SEL_BMSK                               0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_APPS_APC_ODCM_CAL_SEL_SHFT                                   0x15
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD0_BMSK                                               0x1c0000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD0_SHFT                                                   0x12
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_SW_CAL_REDUN_273_256_BMSK                                 0x3ffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_SW_CAL_REDUN_273_256_SHFT                                     0x0

#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000228)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW8_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW8_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_VREF_B1_BMSK                                           0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_VREF_B1_SHFT                                                 0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B1M3_BMSK                                           0x38000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B1M3_SHFT                                                 0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B1M2_BMSK                                            0x7000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B1M2_SHFT                                                 0x18
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B1M1_BMSK                                             0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B1M1_SHFT                                                 0x15
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B1M0_BMSK                                             0x1c0000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B1M0_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_G_B1_BMSK                                                 0x38000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_G_B1_SHFT                                                     0xf
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_SAR_B1_BMSK                                                0x6000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_SAR_B1_SHFT                                                   0xd
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CLK_B0_BMSK                                                0x1800
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CLK_B0_SHFT                                                   0xb
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_VREF_B0_BMSK                                                0x600
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_VREF_B0_SHFT                                                  0x9
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B0M3_BMSK                                                0x1c0
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B0M3_SHFT                                                  0x6
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B0M2_BMSK                                                 0x38
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B0M2_SHFT                                                  0x3
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B0M1_BMSK                                                  0x7
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_PH_B0M1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000022c)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW8_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD0_BMSK                                             0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD0_SHFT                                                   0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_PH_B3M0_BMSK                                           0x70000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_PH_B3M0_SHFT                                                 0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_G_B3_BMSK                                               0xe000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_G_B3_SHFT                                                    0x19
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_SAR_B3_BMSK                                             0x1800000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_SAR_B3_SHFT                                                  0x17
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CLK_B2_BMSK                                              0x600000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CLK_B2_SHFT                                                  0x15
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_VREF_B2_BMSK                                             0x180000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_VREF_B2_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_PH_B2M3_BMSK                                              0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_PH_B2M3_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_PH_B2M2_BMSK                                               0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_PH_B2M2_SHFT                                                  0xd
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_PH_B2M1_BMSK                                               0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_PH_B2M1_SHFT                                                  0xa
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_PH_B2M0_BMSK                                                0x380
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_PH_B2M0_SHFT                                                  0x7
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_G_B2_BMSK                                                    0x70
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_G_B2_SHFT                                                     0x4
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_SAR_B2_BMSK                                                   0xc
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_SAR_B2_SHFT                                                   0x2
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CLK_B1_BMSK                                                   0x3
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CLK_B1_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000230)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW9_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_VREF_B4_BMSK                                           0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_VREF_B4_SHFT                                                 0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B4M3_BMSK                                           0x38000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B4M3_SHFT                                                 0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B4M2_BMSK                                            0x7000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B4M2_SHFT                                                 0x18
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B4M1_BMSK                                             0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B4M1_SHFT                                                 0x15
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B4M0_BMSK                                             0x1c0000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B4M0_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_G_B4_BMSK                                                 0x38000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_G_B4_SHFT                                                     0xf
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_SAR_B4_BMSK                                                0x6000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_SAR_B4_SHFT                                                   0xd
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CLK_B3_BMSK                                                0x1800
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CLK_B3_SHFT                                                   0xb
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_VREF_B3_BMSK                                                0x600
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_VREF_B3_SHFT                                                  0x9
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B3M3_BMSK                                                0x1c0
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B3M3_SHFT                                                  0x6
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B3M2_BMSK                                                 0x38
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B3M2_SHFT                                                  0x3
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B3M1_BMSK                                                  0x7
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_PH_B3M1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000234)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW9_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD0_BMSK                                             0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD0_SHFT                                                   0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_PH_B6M0_BMSK                                           0x70000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_PH_B6M0_SHFT                                                 0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_G_B6_BMSK                                               0xe000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_G_B6_SHFT                                                    0x19
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_SAR_B6_BMSK                                             0x1800000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_SAR_B6_SHFT                                                  0x17
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CLK_B5_BMSK                                              0x600000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CLK_B5_SHFT                                                  0x15
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_VREF_B5_BMSK                                             0x180000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_VREF_B5_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_PH_B5M3_BMSK                                              0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_PH_B5M3_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_PH_B5M2_BMSK                                               0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_PH_B5M2_SHFT                                                  0xd
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_PH_B5M1_BMSK                                               0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_PH_B5M1_SHFT                                                  0xa
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_PH_B5M0_BMSK                                                0x380
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_PH_B5M0_SHFT                                                  0x7
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_G_B5_BMSK                                                    0x70
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_G_B5_SHFT                                                     0x4
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_SAR_B5_BMSK                                                   0xc
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_SAR_B5_SHFT                                                   0x2
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CLK_B4_BMSK                                                   0x3
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CLK_B4_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000238)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW10_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_TSENS0_BASE1_5_0_BMSK                                 0xfc000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_TSENS0_BASE1_5_0_SHFT                                       0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_TSENS0_BASE0_BMSK                                      0x3ff0000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_TSENS0_BASE0_SHFT                                           0x10
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_TSENS_CAL_SEL_BMSK                                        0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_TSENS_CAL_SEL_SHFT                                           0xd
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CLK_B6_BMSK                                               0x1800
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CLK_B6_SHFT                                                  0xb
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_VREF_B6_BMSK                                               0x600
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_VREF_B6_SHFT                                                 0x9
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_PH_B6M3_BMSK                                               0x1c0
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_PH_B6M3_SHFT                                                 0x6
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_PH_B6M2_BMSK                                                0x38
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_PH_B6M2_SHFT                                                 0x3
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_PH_B6M1_BMSK                                                 0x7
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_PH_B6M1_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000023c)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW10_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TSENS1_OFFSET_BMSK                                    0xf0000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TSENS1_OFFSET_SHFT                                          0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TSENS0_OFFSET_BMSK                                     0xf000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TSENS0_OFFSET_SHFT                                          0x18
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TSENS1_BASE1_BMSK                                       0xffc000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TSENS1_BASE1_SHFT                                            0xe
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TSENS1_BASE0_BMSK                                         0x3ff0
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TSENS1_BASE0_SHFT                                            0x4
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TSENS0_BASE1_9_6_BMSK                                        0xf
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TSENS0_BASE1_9_6_SHFT                                        0x0

#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000240)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS9_OFFSET_BMSK                                    0xf0000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS9_OFFSET_SHFT                                          0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS8_OFFSET_BMSK                                     0xf000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS8_OFFSET_SHFT                                          0x18
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS7_OFFSET_BMSK                                      0xf00000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS7_OFFSET_SHFT                                          0x14
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS6_OFFSET_BMSK                                       0xf0000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS6_OFFSET_SHFT                                          0x10
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS5_OFFSET_BMSK                                        0xf000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS5_OFFSET_SHFT                                           0xc
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS4_OFFSET_BMSK                                         0xf00
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS4_OFFSET_SHFT                                           0x8
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS3_OFFSET_BMSK                                          0xf0
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS3_OFFSET_SHFT                                           0x4
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS2_OFFSET_BMSK                                           0xf
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_TSENS2_OFFSET_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000244)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS17_OFFSET_BMSK                                   0xf0000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS17_OFFSET_SHFT                                         0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS16_OFFSET_BMSK                                    0xf000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS16_OFFSET_SHFT                                         0x18
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS15_OFFSET_BMSK                                     0xf00000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS15_OFFSET_SHFT                                         0x14
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS14_OFFSET_BMSK                                      0xf0000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS14_OFFSET_SHFT                                         0x10
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS13_OFFSET_BMSK                                       0xf000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS13_OFFSET_SHFT                                          0xc
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS12_OFFSET_BMSK                                        0xf00
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS12_OFFSET_SHFT                                          0x8
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS11_OFFSET_BMSK                                         0xf0
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS11_OFFSET_SHFT                                          0x4
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS10_OFFSET_BMSK                                          0xf
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_TSENS10_OFFSET_SHFT                                          0x0

#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000248)
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW12_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW12_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW12_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW12_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW12_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW12_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC0_CAL_AVG_ERR_BMSK                               0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC0_CAL_AVG_ERR_SHFT                                     0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC_0_1_FLAG_BMSK                                   0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC_0_1_FLAG_SHFT                                         0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC1_CAL_OVFLOW_BMSK                                0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC1_CAL_OVFLOW_SHFT                                      0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC0_CAL_OVFLOW_BMSK                                0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC0_CAL_OVFLOW_SHFT                                      0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC1_CAL_BMSK                                        0xff00000
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC1_CAL_SHFT                                             0x14
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC0_CAL_BMSK                                          0xff000
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TXDAC0_CAL_SHFT                                              0xc
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TSENS20_OFFSET_BMSK                                        0xf00
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TSENS20_OFFSET_SHFT                                          0x8
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TSENS19_OFFSET_BMSK                                         0xf0
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TSENS19_OFFSET_SHFT                                          0x4
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TSENS18_OFFSET_BMSK                                          0xf
#define HWIO_QFPROM_RAW_CALIB_ROW12_LSB_TSENS18_OFFSET_SHFT                                          0x0

#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000024c)
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW12_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW12_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW12_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW12_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW12_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW12_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_SW_CAL_REDUN_SEL_BMSK                                 0xe0000000
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_SW_CAL_REDUN_SEL_SHFT                                       0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_PORT1_HSTX_TRIM_LSB_BMSK                              0x1e000000
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_PORT1_HSTX_TRIM_LSB_SHFT                                    0x19
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_PORT0_HSTX_TRIM_LSB_BMSK                               0x1e00000
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_PORT0_HSTX_TRIM_LSB_SHFT                                    0x15
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_VOLTAGE_SENSOR_CALIB_BMSK                               0x1fff80
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_VOLTAGE_SENSOR_CALIB_SHFT                                    0x7
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_GNSS_ADC_VCM_BMSK                                           0x60
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_GNSS_ADC_VCM_SHFT                                            0x5
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_GNSS_ADC_LDO_BMSK                                           0x18
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_GNSS_ADC_LDO_SHFT                                            0x3
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_GNSS_ADC_VREF_BMSK                                           0x6
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_GNSS_ADC_VREF_SHFT                                           0x1
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_TXDAC1_CAL_AVG_ERR_BMSK                                      0x1
#define HWIO_QFPROM_RAW_CALIB_ROW12_MSB_TXDAC1_CAL_AVG_ERR_SHFT                                      0x0

#define HWIO_QFPROM_RAW_CALIB_ROW13_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000250)
#define HWIO_QFPROM_RAW_CALIB_ROW13_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW13_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW13_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW13_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW13_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW13_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW13_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW13_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW13_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW13_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW13_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW13_LSB_SW_CAL_REDUN_31_0_BMSK                                0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW13_LSB_SW_CAL_REDUN_31_0_SHFT                                       0x0

#define HWIO_QFPROM_RAW_CALIB_ROW13_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000254)
#define HWIO_QFPROM_RAW_CALIB_ROW13_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW13_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW13_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW13_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW13_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW13_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW13_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW13_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW13_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW13_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW13_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW13_MSB_SW_CAL_REDUN_63_32_BMSK                               0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW13_MSB_SW_CAL_REDUN_63_32_SHFT                                      0x0

#define HWIO_QFPROM_RAW_CALIB_ROW14_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000258)
#define HWIO_QFPROM_RAW_CALIB_ROW14_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW14_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW14_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW14_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW14_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW14_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW14_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW14_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW14_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW14_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW14_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW14_LSB_SW_CAL_REDUN_95_64_BMSK                               0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW14_LSB_SW_CAL_REDUN_95_64_SHFT                                      0x0

#define HWIO_QFPROM_RAW_CALIB_ROW14_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000025c)
#define HWIO_QFPROM_RAW_CALIB_ROW14_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW14_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW14_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW14_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW14_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW14_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW14_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW14_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW14_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW14_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW14_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW14_MSB_SW_CAL_REDUN_127_96_BMSK                              0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW14_MSB_SW_CAL_REDUN_127_96_SHFT                                     0x0

#define HWIO_QFPROM_RAW_CALIB_ROW15_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000260)
#define HWIO_QFPROM_RAW_CALIB_ROW15_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW15_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW15_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW15_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW15_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW15_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW15_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW15_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW15_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW15_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW15_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW15_LSB_SW_CAL_REDUN_159_128_BMSK                             0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW15_LSB_SW_CAL_REDUN_159_128_SHFT                                    0x0

#define HWIO_QFPROM_RAW_CALIB_ROW15_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000264)
#define HWIO_QFPROM_RAW_CALIB_ROW15_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW15_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW15_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW15_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW15_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW15_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW15_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW15_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW15_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW15_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW15_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW15_MSB_SW_CAL_REDUN_191_160_BMSK                             0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW15_MSB_SW_CAL_REDUN_191_160_SHFT                                    0x0

#define HWIO_QFPROM_RAW_CALIB_ROW16_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000268)
#define HWIO_QFPROM_RAW_CALIB_ROW16_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW16_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW16_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW16_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW16_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW16_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW16_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW16_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW16_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW16_LSB_SW_CAL_REDUN_223_192_BMSK                             0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW16_LSB_SW_CAL_REDUN_223_192_SHFT                                    0x0

#define HWIO_QFPROM_RAW_CALIB_ROW16_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000026c)
#define HWIO_QFPROM_RAW_CALIB_ROW16_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW16_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW16_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW16_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW16_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW16_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW16_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW16_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW16_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW16_MSB_SW_CAL_REDUN_255_224_BMSK                             0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW16_MSB_SW_CAL_REDUN_255_224_SHFT                                    0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000270 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_MAXn                                                      19
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

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000274 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_MAXn                                                      19
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                          0x0

#define HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000310)
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_ADDR, HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_ADDR,m,v,HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_IN)
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_QC_SPARE_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_LSB_QC_SPARE_SHFT                                             0x0

#define HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000314)
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_ADDR, HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_ADDR,m,v,HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_IN)
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_QC_SPARE_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REG15_MSB_QC_SPARE_SHFT                                             0x0

#define HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000318)
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_ADDR, HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_ADDR,m,v,HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_IN)
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_QC_SPARE_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_LSB_QC_SPARE_SHFT                                             0x0

#define HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000031c)
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_ADDR, HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_ADDR,m,v,HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_IN)
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_QC_SPARE_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REG16_MSB_QC_SPARE_SHFT                                             0x0

#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000298 + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_MAXn                                                        21
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_ADDR(n), HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_INI(n))
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_QC_SPARE_BMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_QC_SPARE_SHFT                                              0x0

#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000029c + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_MAXn                                                        21
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_ADDR(n), HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_INI(n))
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_RSVD0_BMSK                                          0x80000000
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_RSVD0_SHFT                                                0x1f
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_QC_SPARE_BMSK                                         0xffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_QC_SPARE_SHFT                                              0x0

#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n)                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000348 + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_RMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_MAXn                                                1
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_BMSK                             0xffffffff
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_SHFT                                    0x0

#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n)                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000034c + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_RMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_MAXn                                                1
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_RSVD1_BMSK                                 0x80000000
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_RSVD1_SHFT                                       0x1f
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_FEC_VALUE_BMSK                             0x7f000000
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_FEC_VALUE_SHFT                                   0x18
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_BMSK                               0xffffff
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_SHFT                                    0x0

#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_ADDR                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000358)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_RMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_BMSK                                 0xffff0000
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_SHFT                                       0x10
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_BMSK                                 0xffff
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_SHFT                                    0x0

#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_ADDR                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000035c)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_RMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_RSVD1_BMSK                                 0x80000000
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_RSVD1_SHFT                                       0x1f
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_FEC_VALUE_BMSK                             0x7f000000
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_FEC_VALUE_SHFT                                   0x18
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_BMSK                                   0xffffff
#define HWIO_QFPROM_RAW_QC_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_SHFT                                        0x0

#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n)                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000360 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_RMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_MAXn                                               1
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_BMSK                            0xffffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_SHFT                                   0x0

#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n)                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000364 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_MAXn                                               1
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RSVD1_BMSK                                0x80000000
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RSVD1_SHFT                                      0x1f
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_FEC_VALUE_BMSK                            0x7f000000
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_FEC_VALUE_SHFT                                  0x18
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_BMSK                              0xffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_SHFT                                   0x0

#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000370)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_BMSK                                0xffff0000
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_SHFT                                      0x10
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_BMSK                                0xffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_SHFT                                   0x0

#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000374)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD1_BMSK                                0x80000000
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD1_SHFT                                      0x1f
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_FEC_VALUE_BMSK                            0x7f000000
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_FEC_VALUE_SHFT                                  0x18
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_BMSK                                  0xffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_SHFT                                       0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n)                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000378 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_MAXn                                                     1
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                  0xff000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                        0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                    0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                        0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                      0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                         0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                        0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                         0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n)                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000037c + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_MAXn                                                     1
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RSVD1_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RSVD1_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_FEC_VALUE_BMSK                                  0x7f000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_FEC_VALUE_SHFT                                        0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                    0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                        0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                      0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                         0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                        0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                         0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000388 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000038c + 0x8 * (n))
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
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RSVD1_BMSK                            0x80000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RSVD1_SHFT                                  0x1f
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_BMSK                        0x7f000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_SHFT                              0x18
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                          0xffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                               0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003a8)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                        0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                               0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003ac)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD1_BMSK                            0x80000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD1_SHFT                                  0x1f
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_BMSK                        0x7f000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_SHFT                              0x18
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_BMSK                              0xffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_SHFT                                   0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003b0 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003b4 + 0x8 * (n))
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
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RSVD1_BMSK                            0x80000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RSVD1_SHFT                                  0x1f
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_BMSK                        0x7f000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_SHFT                              0x18
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                          0xffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                               0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003d0)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                        0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                               0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003d4)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD1_BMSK                            0x80000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD1_SHFT                                  0x1f
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_BMSK                        0x7f000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_SHFT                              0x18
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_BMSK                              0xffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_SHFT                                   0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003d8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_MAXn                                                       55
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

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003dc + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_MAXn                                                       55
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RSVD1_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RSVD1_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RSVD0_BMSK                                           0xfe0000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RSVD0_SHFT                                               0x11
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_ADDR_BMSK                                       0x1fffe
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_ADDR_SHFT                                           0x1
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                             0x1
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                             0x0

#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000598)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_LSB_ADDR, HWIO_QFPROM_RAW_HDCP_KSV_LSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_KSV0_BMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_KSV0_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000059c)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_MSB_ADDR, HWIO_QFPROM_RAW_HDCP_KSV_MSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_RSVD1_BMSK                                               0x80000000
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_RSVD1_SHFT                                                     0x1f
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_FEC_VALUE_BMSK                                           0x7f000000
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_FEC_VALUE_SHFT                                                 0x18
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_RSVD0_BMSK                                                 0xffff00
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_RSVD0_SHFT                                                      0x8
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_KSV1_BMSK                                                      0xff
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_KSV1_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000598 + 0x8 * (n))
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_MAXn                                                        40
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_KEY_DATA_BMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_KEY_DATA_SHFT                                              0x0

#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000059c + 0x8 * (n))
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_MAXn                                                        40
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_RSVD1_BMSK                                          0x80000000
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_RSVD1_SHFT                                                0x1f
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_KEY_DATA_BMSK                                         0xffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_KEY_DATA_SHFT                                              0x0

#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000510 + 0x10 * (n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_MAXn                                                  46
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_OEM_SPARE_BMSK                                0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_OEM_SPARE_SHFT                                       0x0

#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000514 + 0x10 * (n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_MAXn                                                  46
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_OEM_SPARE_BMSK                                0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_OEM_SPARE_SHFT                                       0x0

#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000518 + 0x10 * (n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_MAXn                                                  46
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_OEM_SPARE_BMSK                                0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_OEM_SPARE_SHFT                                       0x0

#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000051c + 0x10 * (n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_MAXn                                                  46
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_OEM_SPARE_BMSK                                0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_OEM_SPARE_SHFT                                       0x0

#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n)                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000800 + 0x4 * (n))
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

#define HWIO_QFPROM0_MATCH_STATUS_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002050)
#define HWIO_QFPROM0_MATCH_STATUS_RMSK                                                        0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_ADDR, HWIO_QFPROM0_MATCH_STATUS_RMSK)
#define HWIO_QFPROM0_MATCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_ADDR, m)
#define HWIO_QFPROM0_MATCH_STATUS_FLAG_BMSK                                                   0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_FLAG_SHFT                                                          0x0

#define HWIO_QFPROM1_MATCH_STATUS_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002054)
#define HWIO_QFPROM1_MATCH_STATUS_RMSK                                                        0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_ADDR, HWIO_QFPROM1_MATCH_STATUS_RMSK)
#define HWIO_QFPROM1_MATCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_ADDR, m)
#define HWIO_QFPROM1_MATCH_STATUS_FLAG_BMSK                                                   0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_FLAG_SHFT                                                          0x0

#define HWIO_FEC_ESR_ADDR                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002058)
#define HWIO_FEC_ESR_RMSK                                                                         0x3fff
#define HWIO_FEC_ESR_IN          \
        in_dword_masked(HWIO_FEC_ESR_ADDR, HWIO_FEC_ESR_RMSK)
#define HWIO_FEC_ESR_INM(m)      \
        in_dword_masked(HWIO_FEC_ESR_ADDR, m)
#define HWIO_FEC_ESR_OUT(v)      \
        out_dword(HWIO_FEC_ESR_ADDR,v)
#define HWIO_FEC_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FEC_ESR_ADDR,m,v,HWIO_FEC_ESR_IN)
#define HWIO_FEC_ESR_CORR_SW_ACC_BMSK                                                             0x2000
#define HWIO_FEC_ESR_CORR_SW_ACC_SHFT                                                                0xd
#define HWIO_FEC_ESR_CORR_HDCP_BMSK                                                               0x1000
#define HWIO_FEC_ESR_CORR_HDCP_SHFT                                                                  0xc
#define HWIO_FEC_ESR_CORR_SECURE_CHANNEL_BMSK                                                      0x800
#define HWIO_FEC_ESR_CORR_SECURE_CHANNEL_SHFT                                                        0xb
#define HWIO_FEC_ESR_CORR_BOOT_ROM_BMSK                                                            0x400
#define HWIO_FEC_ESR_CORR_BOOT_ROM_SHFT                                                              0xa
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_BMSK                                                          0x200
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_SHFT                                                            0x9
#define HWIO_FEC_ESR_CORR_MULT_BMSK                                                                0x100
#define HWIO_FEC_ESR_CORR_MULT_SHFT                                                                  0x8
#define HWIO_FEC_ESR_CORR_SEEN_BMSK                                                                 0x80
#define HWIO_FEC_ESR_CORR_SEEN_SHFT                                                                  0x7
#define HWIO_FEC_ESR_ERR_SW_ACC_BMSK                                                                0x40
#define HWIO_FEC_ESR_ERR_SW_ACC_SHFT                                                                 0x6
#define HWIO_FEC_ESR_ERR_HDCP_BMSK                                                                  0x20
#define HWIO_FEC_ESR_ERR_HDCP_SHFT                                                                   0x5
#define HWIO_FEC_ESR_ERR_SECURE_CHANNEL_BMSK                                                        0x10
#define HWIO_FEC_ESR_ERR_SECURE_CHANNEL_SHFT                                                         0x4
#define HWIO_FEC_ESR_ERR_BOOT_ROM_BMSK                                                               0x8
#define HWIO_FEC_ESR_ERR_BOOT_ROM_SHFT                                                               0x3
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_BMSK                                                             0x4
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_SHFT                                                             0x2
#define HWIO_FEC_ESR_ERR_MULT_BMSK                                                                   0x2
#define HWIO_FEC_ESR_ERR_MULT_SHFT                                                                   0x1
#define HWIO_FEC_ESR_ERR_SEEN_BMSK                                                                   0x1
#define HWIO_FEC_ESR_ERR_SEEN_SHFT                                                                   0x0

#define HWIO_FEC_EAR_ADDR                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000205c)
#define HWIO_FEC_EAR_RMSK                                                                     0xffffffff
#define HWIO_FEC_EAR_IN          \
        in_dword_masked(HWIO_FEC_EAR_ADDR, HWIO_FEC_EAR_RMSK)
#define HWIO_FEC_EAR_INM(m)      \
        in_dword_masked(HWIO_FEC_EAR_ADDR, m)
#define HWIO_FEC_EAR_CORR_ADDR_BMSK                                                           0xffff0000
#define HWIO_FEC_EAR_CORR_ADDR_SHFT                                                                 0x10
#define HWIO_FEC_EAR_ERR_ADDR_BMSK                                                                0xffff
#define HWIO_FEC_EAR_ERR_ADDR_SHFT                                                                   0x0

#define HWIO_QFPROM_BIST_CTRL_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002060)
#define HWIO_QFPROM_BIST_CTRL_RMSK                                                                  0xff
#define HWIO_QFPROM_BIST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, HWIO_QFPROM_BIST_CTRL_RMSK)
#define HWIO_QFPROM_BIST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, m)
#define HWIO_QFPROM_BIST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_BIST_CTRL_ADDR,v)
#define HWIO_QFPROM_BIST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BIST_CTRL_ADDR,m,v,HWIO_QFPROM_BIST_CTRL_IN)
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_BMSK                                                      0xfc
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_SHFT                                                       0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_BMSK                                                        0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_SHFT                                                        0x1
#define HWIO_QFPROM_BIST_CTRL_START_BMSK                                                             0x1
#define HWIO_QFPROM_BIST_CTRL_START_SHFT                                                             0x0

#define HWIO_QFPROM_BIST_ERROR0_ADDR                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002064)
#define HWIO_QFPROM_BIST_ERROR0_RMSK                                                          0xffffffff
#define HWIO_QFPROM_BIST_ERROR0_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR0_ADDR, HWIO_QFPROM_BIST_ERROR0_RMSK)
#define HWIO_QFPROM_BIST_ERROR0_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR0_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR0_ERROR_BMSK                                                    0xffffffff
#define HWIO_QFPROM_BIST_ERROR0_ERROR_SHFT                                                           0x0

#define HWIO_QFPROM_BIST_ERROR1_ADDR                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002068)
#define HWIO_QFPROM_BIST_ERROR1_RMSK                                                          0xffffffff
#define HWIO_QFPROM_BIST_ERROR1_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR1_ADDR, HWIO_QFPROM_BIST_ERROR1_RMSK)
#define HWIO_QFPROM_BIST_ERROR1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR1_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR1_ERROR_BMSK                                                    0xffffffff
#define HWIO_QFPROM_BIST_ERROR1_ERROR_SHFT                                                           0x0

#define HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000206c + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_RMSK                                                      0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_MAXn                                                               7
#define HWIO_QFPROM_HASH_SIGNATUREn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), HWIO_QFPROM_HASH_SIGNATUREn_RMSK)
#define HWIO_QFPROM_HASH_SIGNATUREn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), mask)
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_BMSK                                           0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_SHFT                                                  0x0

#define HWIO_HW_KEY_STATUS_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000208c)
#define HWIO_HW_KEY_STATUS_RMSK                                                                     0x7f
#define HWIO_HW_KEY_STATUS_IN          \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, HWIO_HW_KEY_STATUS_RMSK)
#define HWIO_HW_KEY_STATUS_INM(m)      \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, m)
#define HWIO_HW_KEY_STATUS_FUSE_SENSE_DONE_BMSK                                                     0x40
#define HWIO_HW_KEY_STATUS_FUSE_SENSE_DONE_SHFT                                                      0x6
#define HWIO_HW_KEY_STATUS_CRI_CM_BOOT_DONE_BMSK                                                    0x20
#define HWIO_HW_KEY_STATUS_CRI_CM_BOOT_DONE_SHFT                                                     0x5
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

#define HWIO_RESET_JDR_STATUS_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002090)
#define HWIO_RESET_JDR_STATUS_RMSK                                                                   0x3
#define HWIO_RESET_JDR_STATUS_IN          \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, HWIO_RESET_JDR_STATUS_RMSK)
#define HWIO_RESET_JDR_STATUS_INM(m)      \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, m)
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_BMSK                                                       0x2
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_SHFT                                                       0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_BMSK                                              0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_SHFT                                              0x0

#define HWIO_FEAT_PROV_OUTn_ADDR(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002094 + 0x4 * (n))
#define HWIO_FEAT_PROV_OUTn_RMSK                                                              0xffffffff
#define HWIO_FEAT_PROV_OUTn_MAXn                                                                       3
#define HWIO_FEAT_PROV_OUTn_INI(n)        \
        in_dword_masked(HWIO_FEAT_PROV_OUTn_ADDR(n), HWIO_FEAT_PROV_OUTn_RMSK)
#define HWIO_FEAT_PROV_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_FEAT_PROV_OUTn_ADDR(n), mask)
#define HWIO_FEAT_PROV_OUTn_FEAT_PROV_OUT_VALUE_BMSK                                          0xffffffff
#define HWIO_FEAT_PROV_OUTn_FEAT_PROV_OUT_VALUE_SHFT                                                 0x0

#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000020a4 + 0x4 * (n))
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_RMSK                                                0xffffffff
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_MAXn                                                         3
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_INI(n)        \
        in_dword_masked(HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_ADDR(n), HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_RMSK)
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_ADDR(n), mask)
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_SEC_CTRL_MISC_CONFIG_STATUS_VALUE_BMSK              0xffffffff
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_SEC_CTRL_MISC_CONFIG_STATUS_VALUE_SHFT                     0x0

#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004000 + 0x4 * (n))
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_MAXn                                                         71
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_ADDR(n), HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_RMSK)
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_CRI_CM_PRIVATE_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_CRI_CM_PRIVATE_SHFT                                         0x0

#define HWIO_QFPROM_CORR_ACC_PRIVATE_FEAT_PROVn_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004120 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATE_FEAT_PROVn_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATE_FEAT_PROVn_MAXn                                                   3
#define HWIO_QFPROM_CORR_ACC_PRIVATE_FEAT_PROVn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATE_FEAT_PROVn_ADDR(n), HWIO_QFPROM_CORR_ACC_PRIVATE_FEAT_PROVn_RMSK)
#define HWIO_QFPROM_CORR_ACC_PRIVATE_FEAT_PROVn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATE_FEAT_PROVn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ACC_PRIVATE_FEAT_PROVn_ACC_PRIVATE_FEAT_PROV_BMSK                    0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATE_FEAT_PROVn_ACC_PRIVATE_FEAT_PROV_SHFT                           0x0

#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004130)
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_PTE_DATA0_BMSK                                          0xe0000000
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_PTE_DATA0_SHFT                                                0x1d
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_MACCHIATO_EN_BMSK                                       0x10000000
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_MACCHIATO_EN_SHFT                                             0x1c
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_FEATURE_ID_BMSK                                          0xff00000
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_FEATURE_ID_SHFT                                               0x14
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_JTAG_ID_BMSK                                               0xfffff
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_JTAG_ID_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004134)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_PTE_DATA1_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_PTE_DATA1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004138)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_SERIAL_NUM_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_SERIAL_NUM_SHFT                                                0x0

#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000413c)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_PTE_DATA1_BMSK                                          0xffff0000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_PTE_DATA1_SHFT                                                0x10
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_CHIP_ID_BMSK                                                0xffff
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_CHIP_ID_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004140)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_PTE_DATA0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_PTE_DATA0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004144)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_PTE_DATA1_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_PTE_DATA1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004148)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_PTE_DATA0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_PTE_DATA0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000414c)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_PTE_DATA1_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_PTE_DATA1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004150)
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_RD_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR, HWIO_QFPROM_CORR_RD_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_RD_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG31_BMSK                                     0x80000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG31_SHFT                                           0x1f
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG30_BMSK                                     0x40000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG30_SHFT                                           0x1e
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG29_BMSK                                     0x20000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG29_SHFT                                           0x1d
#define HWIO_QFPROM_CORR_RD_PERM_LSB_HDCP_KEY_BMSK                                            0x10000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_HDCP_KEY_SHFT                                                  0x1c
#define HWIO_QFPROM_CORR_RD_PERM_LSB_BOOT_ROM_PATCH_BMSK                                       0x8000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_BOOT_ROM_PATCH_SHFT                                            0x1b
#define HWIO_QFPROM_CORR_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                               0x4000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                    0x1a
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                               0x2000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                    0x19
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SEC_BOOT_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SEC_BOOT_SHFT                                              0x18
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_BMSK                                    0x800000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_SHFT                                        0x17
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_IMAGE_ENCR_KEY_BMSK                                     0x400000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_IMAGE_ENCR_KEY_SHFT                                         0x16
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG21_BMSK                                        0x200000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG21_SHFT                                            0x15
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG20_BMSK                                        0x100000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG20_SHFT                                            0x14
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG19_BMSK                                         0x80000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG19_SHFT                                            0x13
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG18_BMSK                                         0x40000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG18_SHFT                                            0x12
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG17_BMSK                                         0x20000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG17_SHFT                                            0x11
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG16_BMSK                                         0x10000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG16_SHFT                                            0x10
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG15_BMSK                                          0x8000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG15_SHFT                                             0xf
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MEM_CONFIG_BMSK                                              0x4000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MEM_CONFIG_SHFT                                                 0xe
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CALIB_BMSK                                                   0x2000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CALIB_SHFT                                                      0xd
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PK_HASH0_BMSK                                                0x1000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PK_HASH0_SHFT                                                   0xc
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEAT_CONFIG_BMSK                                              0x800
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEAT_CONFIG_SHFT                                                0xb
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_CONFIG_BMSK                                               0x400
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_CONFIG_SHFT                                                 0xa
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_4_BMSK                                          0x200
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_4_SHFT                                            0x9
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                          0x100
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                            0x8
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                           0x80
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                            0x7
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                           0x40
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                            0x6
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEC_EN_BMSK                                                    0x20
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEC_EN_SHFT                                                     0x5
#define HWIO_QFPROM_CORR_RD_PERM_LSB_WR_PERM_BMSK                                                   0x10
#define HWIO_QFPROM_CORR_RD_PERM_LSB_WR_PERM_SHFT                                                    0x4
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RD_PERM_BMSK                                                    0x8
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RD_PERM_SHFT                                                    0x3
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PTE_BMSK                                                        0x4
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PTE_SHFT                                                        0x2
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ACC_PRIVATE_FEAT_PROV_BMSK                                      0x2
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ACC_PRIVATE_FEAT_PROV_SHFT                                      0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CRI_CM_PRIVATE_BMSK                                             0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CRI_CM_PRIVATE_SHFT                                             0x0

#define HWIO_QFPROM_CORR_RD_PERM_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004154)
#define HWIO_QFPROM_CORR_RD_PERM_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_RD_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_MSB_ADDR, HWIO_QFPROM_CORR_RD_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_RD_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_PERM_MSB_RSVD0_BMSK                                               0xffff8000
#define HWIO_QFPROM_CORR_RD_PERM_MSB_RSVD0_SHFT                                                      0xf
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG46_BMSK                                         0x4000
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG46_SHFT                                            0xe
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG45_BMSK                                         0x2000
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG45_SHFT                                            0xd
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG44_BMSK                                         0x1000
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG44_SHFT                                            0xc
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG43_BMSK                                          0x800
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG43_SHFT                                            0xb
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG42_BMSK                                          0x400
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG42_SHFT                                            0xa
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG41_BMSK                                          0x200
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG41_SHFT                                            0x9
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG40_BMSK                                          0x100
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG40_SHFT                                            0x8
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG39_BMSK                                           0x80
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG39_SHFT                                            0x7
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG38_BMSK                                           0x40
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG38_SHFT                                            0x6
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG37_BMSK                                           0x20
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG37_SHFT                                            0x5
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG36_BMSK                                           0x10
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG36_SHFT                                            0x4
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG35_BMSK                                            0x8
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG35_SHFT                                            0x3
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG34_BMSK                                            0x4
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG34_SHFT                                            0x2
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG33_BMSK                                            0x2
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG33_SHFT                                            0x1
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG32_BMSK                                            0x1
#define HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SPARE_REG32_SHFT                                            0x0

#define HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004158)
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR, HWIO_QFPROM_CORR_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG31_BMSK                                     0x80000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG31_SHFT                                           0x1f
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG30_BMSK                                     0x40000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG30_SHFT                                           0x1e
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG29_BMSK                                     0x20000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG29_SHFT                                           0x1d
#define HWIO_QFPROM_CORR_WR_PERM_LSB_HDCP_KEY_BMSK                                            0x10000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_HDCP_KEY_SHFT                                                  0x1c
#define HWIO_QFPROM_CORR_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                       0x8000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                            0x1b
#define HWIO_QFPROM_CORR_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                               0x4000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                    0x1a
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                               0x2000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                    0x19
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                              0x18
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_BMSK                                    0x800000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_SHFT                                        0x17
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_IMAGE_ENCR_KEY_BMSK                                     0x400000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_IMAGE_ENCR_KEY_SHFT                                         0x16
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG21_BMSK                                        0x200000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG21_SHFT                                            0x15
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG20_BMSK                                        0x100000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG20_SHFT                                            0x14
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG19_BMSK                                         0x80000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG19_SHFT                                            0x13
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG18_BMSK                                         0x40000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG18_SHFT                                            0x12
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG17_BMSK                                         0x20000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG17_SHFT                                            0x11
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG16_BMSK                                         0x10000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG16_SHFT                                            0x10
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG15_BMSK                                          0x8000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG15_SHFT                                             0xf
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MEM_CONFIG_BMSK                                              0x4000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MEM_CONFIG_SHFT                                                 0xe
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CALIB_BMSK                                                   0x2000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CALIB_SHFT                                                      0xd
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PK_HASH0_BMSK                                                0x1000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PK_HASH0_SHFT                                                   0xc
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEAT_CONFIG_BMSK                                              0x800
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEAT_CONFIG_SHFT                                                0xb
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_CONFIG_BMSK                                               0x400
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_CONFIG_SHFT                                                 0xa
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_4_BMSK                                          0x200
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_4_SHFT                                            0x9
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                          0x100
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                            0x8
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                           0x80
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                            0x7
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                           0x40
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                            0x6
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEC_EN_BMSK                                                    0x20
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEC_EN_SHFT                                                     0x5
#define HWIO_QFPROM_CORR_WR_PERM_LSB_WR_PERM_BMSK                                                   0x10
#define HWIO_QFPROM_CORR_WR_PERM_LSB_WR_PERM_SHFT                                                    0x4
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RD_PERM_BMSK                                                    0x8
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RD_PERM_SHFT                                                    0x3
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PTE_BMSK                                                        0x4
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PTE_SHFT                                                        0x2
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ACC_PRIVATE_FEAT_PROV_BMSK                                      0x2
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ACC_PRIVATE_FEAT_PROV_SHFT                                      0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CRI_CM_PRIVATE_BMSK                                             0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CRI_CM_PRIVATE_SHFT                                             0x0

#define HWIO_QFPROM_CORR_WR_PERM_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000415c)
#define HWIO_QFPROM_CORR_WR_PERM_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_MSB_ADDR, HWIO_QFPROM_CORR_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_WR_PERM_MSB_RSVD0_BMSK                                               0xffff8000
#define HWIO_QFPROM_CORR_WR_PERM_MSB_RSVD0_SHFT                                                      0xf
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG46_BMSK                                         0x4000
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG46_SHFT                                            0xe
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG45_BMSK                                         0x2000
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG45_SHFT                                            0xd
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG44_BMSK                                         0x1000
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG44_SHFT                                            0xc
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG43_BMSK                                          0x800
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG43_SHFT                                            0xb
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG42_BMSK                                          0x400
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG42_SHFT                                            0xa
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG41_BMSK                                          0x200
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG41_SHFT                                            0x9
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG40_BMSK                                          0x100
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG40_SHFT                                            0x8
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG39_BMSK                                           0x80
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG39_SHFT                                            0x7
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG38_BMSK                                           0x40
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG38_SHFT                                            0x6
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG37_BMSK                                           0x20
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG37_SHFT                                            0x5
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG36_BMSK                                           0x10
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG36_SHFT                                            0x4
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG35_BMSK                                            0x8
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG35_SHFT                                            0x3
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG34_BMSK                                            0x4
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG34_SHFT                                            0x2
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG33_BMSK                                            0x2
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG33_SHFT                                            0x1
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG32_BMSK                                            0x1
#define HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SPARE_REG32_SHFT                                            0x0

#define HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004160)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, m)
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

#define HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004164)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RSVD0_BMSK                                                0xffff8000
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RSVD0_SHFT                                                       0xf
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION46_FEC_EN_BMSK                                          0x4000
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION46_FEC_EN_SHFT                                             0xe
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION45_FEC_EN_BMSK                                          0x2000
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION45_FEC_EN_SHFT                                             0xd
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION44_FEC_EN_BMSK                                          0x1000
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION44_FEC_EN_SHFT                                             0xc
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION43_FEC_EN_BMSK                                           0x800
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION43_FEC_EN_SHFT                                             0xb
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION42_FEC_EN_BMSK                                           0x400
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION42_FEC_EN_SHFT                                             0xa
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION41_FEC_EN_BMSK                                           0x200
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION41_FEC_EN_SHFT                                             0x9
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION40_FEC_EN_BMSK                                           0x100
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION40_FEC_EN_SHFT                                             0x8
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION39_FEC_EN_BMSK                                            0x80
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION39_FEC_EN_SHFT                                             0x7
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION38_FEC_EN_BMSK                                            0x40
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION38_FEC_EN_SHFT                                             0x6
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION37_FEC_EN_BMSK                                            0x20
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION37_FEC_EN_SHFT                                             0x5
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION36_FEC_EN_BMSK                                            0x10
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION36_FEC_EN_SHFT                                             0x4
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION35_FEC_EN_BMSK                                             0x8
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION35_FEC_EN_SHFT                                             0x3
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION34_FEC_EN_BMSK                                             0x4
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION34_FEC_EN_SHFT                                             0x2
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION33_FEC_EN_BMSK                                             0x2
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION33_FEC_EN_SHFT                                             0x1
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION32_FEC_EN_BMSK                                             0x1
#define HWIO_QFPROM_CORR_FEC_EN_MSB_REGION32_FEC_EN_SHFT                                             0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004168)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_XBL0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_XBL0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000416c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_XBL1_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_XBL1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004170)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_PIL_SUBSYSTEM_31_0_BMSK                          0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_PIL_SUBSYSTEM_31_0_SHFT                                 0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004174)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RSVD0_BMSK                                       0xfe000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RSVD0_SHFT                                             0x19
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_BMSK                                          0x1fe0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_SHFT                                               0x11
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_TZ_BMSK                                             0x1ffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_TZ_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004178)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RSVD1_BMSK                                       0xc0000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RSVD1_SHFT                                             0x1e
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_TQS_HASH_ACTIVE_BMSK                             0x3e000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_TQS_HASH_ACTIVE_SHFT                                   0x19
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_BMSK                         0x1000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_SHFT                              0x18
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_PIL_SUBSYSTEM_47_32_BMSK                           0xffff00
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_PIL_SUBSYSTEM_47_32_SHFT                                0x8
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RSVD0_BMSK                                             0xff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RSVD0_SHFT                                              0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000417c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_BMSK             0xfffff000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SHFT                    0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_HYPERVISOR_BMSK                                       0xfff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_HYPERVISOR_SHFT                                         0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004180)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MSS_BMSK                                         0xffff0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MSS_SHFT                                               0x10
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MBA_BMSK                                             0xffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MBA_SHFT                                                0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004184)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_BMSK             0xffffff00
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_SHFT                    0x8
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RSVD0_BMSK                                             0xff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RSVD0_SHFT                                              0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004188)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_BMSK                              0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_SHFT                                    0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_BMSK                              0x40000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_SHFT                                    0x1e
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_BMSK                           0x20000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_SHFT                                 0x1d
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_BMSK                        0x10000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_SHFT                              0x1c
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD0_BMSK                                        0xe000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD0_SHFT                                             0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_HASH_INTEGRITY_CHECK_DISABLE_BMSK             0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                  0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_BMSK             0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                 0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_USB_SS_DISABLE_BMSK                                0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_USB_SS_DISABLE_SHFT                                    0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_BMSK                         0x200000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_SHFT                             0x15
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_BMSK                          0x100000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_SHFT                              0x14
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_BMSK                        0x80000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_SHFT                           0x13
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC1_SCM_FORCE_EFUSE_KEY_BMSK                       0x40000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC1_SCM_FORCE_EFUSE_KEY_SHFT                          0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_UFS_SCM_FORCE_EFUSE_KEY_BMSK                        0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_UFS_SCM_FORCE_EFUSE_KEY_SHFT                           0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC2_SCM_FORCE_EFUSE_KEY_BMSK                       0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC2_SCM_FORCE_EFUSE_KEY_SHFT                          0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                                 0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                    0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_BMSK                                         0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_SHFT                                            0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_BMSK                                0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_SHFT                                   0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_FUSE_PROG_DISABLE_BMSK                            0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_FUSE_PROG_DISABLE_SHFT                               0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                    0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                      0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_BMSK                              0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_SHFT                                0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                        0x3e0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                          0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                               0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_BMSK                             0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_SHFT                             0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_BMSK                                0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_SHFT                                0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_BMSK                                       0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_SHFT                                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                    0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                    0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000418c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS0_SPNIDEN_DISABLE_BMSK                       0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS0_SPNIDEN_DISABLE_SHFT                             0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_BMSK                           0x40000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_SHFT                                 0x1e
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_GSS_A5_NIDEN_DISABLE_BMSK                        0x20000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_GSS_A5_NIDEN_DISABLE_SHFT                              0x1d
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_NIDEN_DISABLE_BMSK                           0x10000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_NIDEN_DISABLE_SHFT                                 0x1c
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_PIMEM_NIDEN_DISABLE_BMSK                          0x8000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_PIMEM_NIDEN_DISABLE_SHFT                               0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_NIDEN_DISABLE_BMSK                            0x4000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_NIDEN_DISABLE_SHFT                                 0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_NIDEN_DISABLE_BMSK                           0x2000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_NIDEN_DISABLE_SHFT                                0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_NIDEN_DISABLE_BMSK                          0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_NIDEN_DISABLE_SHFT                               0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                             0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                 0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS1_NIDEN_DISABLE_BMSK                           0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS1_NIDEN_DISABLE_SHFT                               0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS0_NIDEN_DISABLE_BMSK                           0x200000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS0_NIDEN_DISABLE_SHFT                               0x15
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_BMSK                             0x100000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_SHFT                                 0x14
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_GSS_A5_DBGEN_DISABLE_BMSK                           0x80000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_GSS_A5_DBGEN_DISABLE_SHFT                              0x13
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_A5X_ISDB_DBGEN_DISABLE_BMSK                         0x40000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_A5X_ISDB_DBGEN_DISABLE_SHFT                            0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_BMSK                          0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_SHFT                             0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_Q6_DBGEN_DISABLE_BMSK                           0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_Q6_DBGEN_DISABLE_SHFT                              0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_DBGEN_DISABLE_BMSK                               0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_DBGEN_DISABLE_SHFT                                  0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_PIMEM_DBGEN_DISABLE_BMSK                             0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_PIMEM_DBGEN_DISABLE_SHFT                                0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_BMSK                               0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_SHFT                                  0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_BMSK                              0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_SHFT                                 0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_DBGEN_DISABLE_BMSK                              0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_DBGEN_DISABLE_SHFT                                0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                                0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                  0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS1_DBGEN_DISABLE_BMSK                              0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS1_DBGEN_DISABLE_SHFT                                0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS0_DBGEN_DISABLE_BMSK                              0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS0_DBGEN_DISABLE_SHFT                                0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                              0x80
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                               0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_BMSK                                 0x40
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_SHFT                                  0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_Q6_ETM_DISABLE_BMSK                                0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SSC_Q6_ETM_DISABLE_SHFT                                 0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE6_DISABLE_BMSK                                    0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE6_DISABLE_SHFT                                     0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE5_DISABLE_BMSK                                     0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE5_DISABLE_SHFT                                     0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE4_DISABLE_BMSK                                     0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE4_DISABLE_SHFT                                     0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE3_DISABLE_BMSK                                     0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE3_DISABLE_SHFT                                     0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE2_DISABLE_BMSK                                     0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE2_DISABLE_SHFT                                     0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004190)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG46_SECURE_BMSK                      0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG46_SECURE_SHFT                            0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG45_SECURE_BMSK                      0x40000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG45_SECURE_SHFT                            0x1e
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG44_SECURE_BMSK                      0x20000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG44_SECURE_SHFT                            0x1d
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG43_SECURE_BMSK                      0x10000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG43_SECURE_SHFT                            0x1c
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG42_SECURE_BMSK                       0x8000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG42_SECURE_SHFT                            0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG41_SECURE_BMSK                       0x4000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG41_SECURE_SHFT                            0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG40_SECURE_BMSK                       0x2000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG40_SECURE_SHFT                            0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG39_SECURE_BMSK                       0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG39_SECURE_SHFT                            0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG38_SECURE_BMSK                        0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG38_SECURE_SHFT                            0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG37_SECURE_BMSK                        0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG37_SECURE_SHFT                            0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG36_SECURE_BMSK                        0x200000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG36_SECURE_SHFT                            0x15
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG35_SECURE_BMSK                        0x100000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG35_SECURE_SHFT                            0x14
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG34_SECURE_BMSK                         0x80000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG34_SECURE_SHFT                            0x13
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG33_SECURE_BMSK                         0x40000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG33_SECURE_SHFT                            0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG32_SECURE_BMSK                         0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG32_SECURE_SHFT                            0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG31_SECURE_BMSK                         0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG31_SECURE_SHFT                            0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG30_SECURE_BMSK                          0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG30_SECURE_SHFT                             0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG29_SECURE_BMSK                          0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG29_SECURE_SHFT                             0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD0_BMSK                                           0x3800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD0_SHFT                                              0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_GSS_A5_SPIDEN_DISABLE_BMSK                            0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_GSS_A5_SPIDEN_DISABLE_SHFT                              0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_SSC_SPIDEN_DISABLE_BMSK                               0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_SSC_SPIDEN_DISABLE_SHFT                                 0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_PIMEM_SPIDEN_DISABLE_BMSK                             0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_PIMEM_SPIDEN_DISABLE_SHFT                               0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_DAP_SPIDEN_DISABLE_BMSK                                0x80
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_DAP_SPIDEN_DISABLE_SHFT                                 0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_APPS1_SPIDEN_DISABLE_BMSK                              0x40
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_APPS1_SPIDEN_DISABLE_SHFT                               0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_APPS0_SPIDEN_DISABLE_BMSK                              0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_APPS0_SPIDEN_DISABLE_SHFT                               0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_GSS_A5_SPNIDEN_DISABLE_BMSK                            0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_GSS_A5_SPNIDEN_DISABLE_SHFT                             0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_SSC_SPNIDEN_DISABLE_BMSK                                0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_SSC_SPNIDEN_DISABLE_SHFT                                0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_PIMEM_SPNIDEN_DISABLE_BMSK                              0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_PIMEM_SPNIDEN_DISABLE_SHFT                              0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_DAP_SPNIDEN_DISABLE_BMSK                                0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_DAP_SPNIDEN_DISABLE_SHFT                                0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_APPS1_SPNIDEN_DISABLE_BMSK                              0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_APPS1_SPNIDEN_DISABLE_SHFT                              0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004194)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_BMSK                              0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_SHFT                                    0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_HW_ID_BMSK                                       0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_HW_ID_SHFT                                          0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004198)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_VID_BMSK                                  0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_VID_SHFT                                        0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_PID_BMSK                                      0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_PID_SHFT                                         0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000419c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RSVD0_BMSK                                       0xffffff00
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RSVD0_SHFT                                              0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                          0xff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                           0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_UFS_SW_CONTROL_DISABLE_BMSK                     0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_UFS_SW_CONTROL_DISABLE_SHFT                           0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SATA_DISABLE_BMSK                               0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SATA_DISABLE_SHFT                                     0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SECURE_CHANNEL_DISABLE_BMSK                     0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SECURE_CHANNEL_DISABLE_SHFT                           0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SCM_DISABLE_BMSK                                0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SCM_DISABLE_SHFT                                      0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ICE_FORCE_HW_KEY1_BMSK                           0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ICE_FORCE_HW_KEY1_SHFT                                0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ICE_FORCE_HW_KEY0_BMSK                           0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ICE_FORCE_HW_KEY0_SHFT                                0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ICE_DISABLE_BMSK                                 0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ICE_DISABLE_SHFT                                      0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RICA_DISABLE_BMSK                                0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RICA_DISABLE_SHFT                                     0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_BMSK                            0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_SHFT                                0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_BMSK                             0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_SHFT                                 0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_SEL1_BMSK                             0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_SEL1_SHFT                                 0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_SEL0_BMSK                             0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_SEL0_SHFT                                 0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_DISABLE_BMSK                           0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GFX3D_TURBO_DISABLE_SHFT                              0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_ENCODE_DISABLE_BMSK                     0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_ENCODE_DISABLE_SHFT                        0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_DECODE_DISABLE_BMSK                     0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_HEVC_DECODE_DISABLE_SHFT                        0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_4K_DISABLE_BMSK                              0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_4K_DISABLE_SHFT                                 0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                            0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                               0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                            0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                               0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_BMSK                                   0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_SHFT                                      0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_BMSK                                   0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_SHFT                                      0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDP_APICAL_LTC_DISABLE_BMSK                          0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDP_APICAL_LTC_DISABLE_SHFT                            0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_BMSK                                     0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_SHFT                                       0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                                   0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                     0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_BMSK                                   0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_SHFT                                     0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_FD_DISABLE_BMSK                                       0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_FD_DISABLE_SHFT                                        0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_DPCM_14_DISABLE_BMSK                             0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_DPCM_14_DISABLE_SHFT                              0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_BMSK                                    0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_SHFT                                     0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_BMSK                                   0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_SHFT                                    0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_BMSK                                    0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_SHFT                                    0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_BMSK                            0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_SHFT                            0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD0_BMSK                                      0xff800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD0_SHFT                                            0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APS_RESET_DISABLE_BMSK                            0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APS_RESET_DISABLE_SHFT                                0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_HVX_DISABLE_BMSK                                  0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_HVX_DISABLE_SHFT                                      0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APB2JTAG_LIFECYCLE1_DIS_BMSK                      0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APB2JTAG_LIFECYCLE1_DIS_SHFT                          0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APB2JTAG_LIFECYCLE0_EN_BMSK                        0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APB2JTAG_LIFECYCLE0_EN_SHFT                           0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_STACKED_MEMORY_ID_BMSK                             0x7c000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_STACKED_MEMORY_ID_SHFT                                 0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PRNG_TESTMODE_DISABLE_BMSK                          0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PRNG_TESTMODE_DISABLE_SHFT                             0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_DCC_DISABLE_BMSK                                    0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_DCC_DISABLE_SHFT                                       0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MOCHA_PART_BMSK                                      0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MOCHA_PART_SHFT                                        0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_NIDNT_DISABLE_BMSK                                   0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_NIDNT_DISABLE_SHFT                                     0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SMMU_DISABLE_BMSK                                    0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SMMU_DISABLE_SHFT                                      0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_BMSK                                     0x1e0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_SHFT                                       0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                      0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                       0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_NAV_DISABLE_BMSK                                       0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_NAV_DISABLE_SHFT                                       0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_2_DISABLE_BMSK                                    0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_2_DISABLE_SHFT                                    0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_1_DISABLE_BMSK                                    0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_1_DISABLE_SHFT                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_0_DISABLE_BMSK                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_0_DISABLE_SHFT                                    0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RSVD0_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RSVD0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ac)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RSVD0_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RSVD0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_NIDEN_DISABLE_BMSK                       0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_NIDEN_DISABLE_SHFT                             0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS1_NIDEN_DISABLE_BMSK                     0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS1_NIDEN_DISABLE_SHFT                           0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS0_NIDEN_DISABLE_BMSK                     0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS0_NIDEN_DISABLE_SHFT                           0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_MSS_DBGEN_DISABLE_BMSK                       0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_MSS_DBGEN_DISABLE_SHFT                             0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GSS_A5_DBGEN_DISABLE_BMSK                     0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GSS_A5_DBGEN_DISABLE_SHFT                          0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_A5X_ISDB_DBGEN_DISABLE_BMSK                   0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_A5X_ISDB_DBGEN_DISABLE_SHFT                        0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                    0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                         0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SSC_Q6_DBGEN_DISABLE_BMSK                     0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SSC_Q6_DBGEN_DISABLE_SHFT                          0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SSC_DBGEN_DISABLE_BMSK                         0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SSC_DBGEN_DISABLE_SHFT                             0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_PIMEM_DBGEN_DISABLE_BMSK                       0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_PIMEM_DBGEN_DISABLE_SHFT                           0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_RPM_DBGEN_DISABLE_BMSK                         0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_RPM_DBGEN_DISABLE_SHFT                             0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                        0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                            0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_LPASS_DBGEN_DISABLE_BMSK                        0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_LPASS_DBGEN_DISABLE_SHFT                           0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_DBGEN_DISABLE_BMSK                          0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_DBGEN_DISABLE_SHFT                             0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS1_DBGEN_DISABLE_BMSK                        0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS1_DBGEN_DISABLE_SHFT                           0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS0_DBGEN_DISABLE_BMSK                        0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_APPS0_DBGEN_DISABLE_SHFT                           0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                        0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                           0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_RPM_DAPEN_DISABLE_BMSK                           0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_RPM_DAPEN_DISABLE_SHFT                              0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SSC_Q6_ETM_DISABLE_BMSK                          0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SSC_Q6_ETM_DISABLE_SHFT                             0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE6_DISABLE_BMSK                              0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE6_DISABLE_SHFT                                 0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE5_DISABLE_BMSK                               0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE5_DISABLE_SHFT                                 0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE4_DISABLE_BMSK                               0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE4_DISABLE_SHFT                                 0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE3_DISABLE_BMSK                               0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE3_DISABLE_SHFT                                 0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE2_DISABLE_BMSK                               0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE2_DISABLE_SHFT                                 0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE1_DISABLE_BMSK                                0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE1_DISABLE_SHFT                                 0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE0_DISABLE_BMSK                                0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_SPARE0_DISABLE_SHFT                                 0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPMU_NIDEN_DISABLE_BMSK                            0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPMU_NIDEN_DISABLE_SHFT                             0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPMU_DBGEN_DISABLE_BMSK                            0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPMU_DBGEN_DISABLE_SHFT                             0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPMU_DAPEN_DISABLE_BMSK                             0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QC_GPMU_DAPEN_DISABLE_SHFT                             0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QDI_SPMI_DISABLE_BMSK                                  0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_QDI_SPMI_DISABLE_SHFT                                  0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_SM_BIST_DISABLE_BMSK                                   0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_SM_BIST_DISABLE_SHFT                                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TIC_DISABLE_BMSK                                       0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TIC_DISABLE_SHFT                                       0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGCRYPTODISABLE_BMSK                0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGCRYPTODISABLE_SHFT                      0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3V_BMSK                 0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3V_SHFT                       0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3TE_BMSK                0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3TE_SHFT                      0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3EE_BMSK                0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SYS_APCSCPUCFGRSTSCTLREL3EE_SHFT                      0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS1_CFGCPUPRESENT_N_BMSK                       0xc000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS1_CFGCPUPRESENT_N_SHFT                            0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS0_CFGCPUPRESENT_N_BMSK                       0x3000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS0_CFGCPUPRESENT_N_SHFT                            0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                        0xf80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                            0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_SPIDEN_DISABLE_BMSK                      0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_SPIDEN_DISABLE_SHFT                         0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SSC_SPIDEN_DISABLE_BMSK                         0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SSC_SPIDEN_DISABLE_SHFT                            0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_SPIDEN_DISABLE_BMSK                       0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_SPIDEN_DISABLE_SHFT                          0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPIDEN_DISABLE_BMSK                          0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPIDEN_DISABLE_SHFT                             0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_APPS1_SPIDEN_DISABLE_BMSK                        0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_APPS1_SPIDEN_DISABLE_SHFT                           0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_APPS0_SPIDEN_DISABLE_BMSK                        0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_APPS0_SPIDEN_DISABLE_SHFT                           0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_SPNIDEN_DISABLE_BMSK                      0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_SPNIDEN_DISABLE_SHFT                         0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SSC_SPNIDEN_DISABLE_BMSK                          0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SSC_SPNIDEN_DISABLE_SHFT                            0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_SPNIDEN_DISABLE_BMSK                        0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_SPNIDEN_DISABLE_SHFT                          0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPNIDEN_DISABLE_BMSK                          0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_DAP_SPNIDEN_DISABLE_SHFT                            0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_APPS1_SPNIDEN_DISABLE_BMSK                        0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_APPS1_SPNIDEN_DISABLE_SHFT                          0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_APPS0_SPNIDEN_DISABLE_BMSK                         0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_APPS0_SPNIDEN_DISABLE_SHFT                          0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_MSS_NIDEN_DISABLE_BMSK                             0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_MSS_NIDEN_DISABLE_SHFT                              0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_NIDEN_DISABLE_BMSK                          0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_GSS_A5_NIDEN_DISABLE_SHFT                           0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SSC_NIDEN_DISABLE_BMSK                             0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_SSC_NIDEN_DISABLE_SHFT                              0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_NIDEN_DISABLE_BMSK                            0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_PIMEM_NIDEN_DISABLE_SHFT                            0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_RPM_NIDEN_DISABLE_BMSK                              0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_RPM_NIDEN_DISABLE_SHFT                              0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_WCSS_NIDEN_DISABLE_BMSK                             0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_WCSS_NIDEN_DISABLE_SHFT                             0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_LPASS_NIDEN_DISABLE_BMSK                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_QC_LPASS_NIDEN_DISABLE_SHFT                            0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_TAP_GEN_SPARE_INSTR_DISABLE_13_0_BMSK           0xfffc0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_TAP_GEN_SPARE_INSTR_DISABLE_13_0_SHFT                 0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_TAP_INSTR_DISABLE_BMSK                             0x3ffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_TAP_INSTR_DISABLE_SHFT                                 0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041bc)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                     0xfffc0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                           0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_TAP_GEN_SPARE_INSTR_DISABLE_31_14_BMSK             0x3ffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_TAP_GEN_SPARE_INSTR_DISABLE_31_14_SHFT                 0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_PBL_PATCH_VERSION_BMSK                    0xf8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_PBL_PATCH_VERSION_SHFT                          0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_PBL_PATCH_VERSION_BMSK                      0x7c00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_PBL_PATCH_VERSION_SHFT                           0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RSVD0_BMSK                                        0x3c0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RSVD0_SHFT                                            0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_PBL_BOOT_SPEED_BMSK                           0x30000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_PBL_BOOT_SPEED_SHFT                              0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RPM_PBL_BOOT_SPEED_BMSK                             0xc000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RPM_PBL_BOOT_SPEED_SHFT                                0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_BOOT_FROM_ROM_BMSK                             0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_APPS_BOOT_FROM_ROM_SHFT                                0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RPM_BOOT_FROM_ROM_BMSK                              0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_RPM_BOOT_FROM_ROM_SHFT                                 0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_BOOT_FROM_ROM_BMSK                             0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MODEM_BOOT_FROM_ROM_SHFT                               0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MSA_ENA_BMSK                                         0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_MSA_ENA_SHFT                                           0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_FORCE_MSA_AUTH_EN_BMSK                               0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_FORCE_MSA_AUTH_EN_SHFT                                 0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ARM_CE_DISABLE_USAGE_BMSK                            0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_ARM_CE_DISABLE_USAGE_SHFT                              0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_BOOT_ROM_CFG_BMSK                                     0xff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_LSB_BOOT_ROM_CFG_SHFT                                      0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_RSVD0_BMSK                                      0xffffc000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_RSVD0_SHFT                                             0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_FOUNDRY_ID_BMSK                                     0x3c00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_FOUNDRY_ID_SHFT                                        0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PLL_CFG_BMSK                                         0x3f0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_PLL_CFG_SHFT                                           0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_APPS_PBL_PLL_CTRL_BMSK                                 0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW4_MSB_APPS_PBL_PLL_CTRL_SHFT                                 0x0

#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_MAXn                                                        3
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_HASH_DATA0_BMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_HASH_DATA0_SHFT                                           0x0

#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041cc + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_MAXn                                                        3
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_HASH_DATA1_BMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_HASH_DATA1_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e8)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_APPS_APC0_LDO_VREF_FUNC_TRIM_BMSK                     0xf8000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_APPS_APC0_LDO_VREF_FUNC_TRIM_SHFT                           0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_APPS_APC0_LDO_VREF_RET_TRIM_BMSK                       0x7c00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_APPS_APC0_LDO_VREF_RET_TRIM_SHFT                            0x16
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_GP_HW_CALIB_BMSK                                        0x3c0000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_GP_HW_CALIB_SHFT                                            0x12
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_Q6SS1_LDO_VREF_TRIM_BMSK                                 0x3fc00
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_Q6SS1_LDO_VREF_TRIM_SHFT                                     0xa
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_Q6SS1_LDO_ENABLE_BMSK                                      0x200
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_Q6SS1_LDO_ENABLE_SHFT                                        0x9
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_Q6SS0_LDO_VREF_TRIM_BMSK                                   0x1fe
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_Q6SS0_LDO_VREF_TRIM_SHFT                                     0x1
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_Q6SS0_LDO_ENABLE_BMSK                                        0x1
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_Q6SS0_LDO_ENABLE_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ec)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_SPARE0_BMSK                                           0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_SPARE0_SHFT                                                 0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC0_CPU1_IMEAS_BHS_MODE_ACC_TRIM_BMSK           0x3e000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC0_CPU1_IMEAS_BHS_MODE_ACC_TRIM_SHFT                 0x19
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC0_CPU0_IMEAS_BHS_MODE_ACC_TRIM_BMSK            0x1f00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC0_CPU0_IMEAS_BHS_MODE_ACC_TRIM_SHFT                 0x14
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC1_L2_IMEAS_BHS_MODE_ACC_TRIM_BMSK                0xf8000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC1_L2_IMEAS_BHS_MODE_ACC_TRIM_SHFT                    0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC0_L2_IMEAS_BHS_MODE_ACC_TRIM_BMSK                 0x7c00
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC0_L2_IMEAS_BHS_MODE_ACC_TRIM_SHFT                    0xa
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC1_LDO_VREF_FUNC_TRIM_BMSK                          0x3e0
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC1_LDO_VREF_FUNC_TRIM_SHFT                            0x5
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC1_LDO_VREF_RET_TRIM_BMSK                            0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_APPS_APC1_LDO_VREF_RET_TRIM_SHFT                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f0)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_SPARE0_BMSK                                           0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_SPARE0_SHFT                                                 0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC0_CPU1_IMEAS_LDO_MODE_ACC_TRIM_BMSK           0x3e000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC0_CPU1_IMEAS_LDO_MODE_ACC_TRIM_SHFT                 0x19
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC0_CPU0_IMEAS_LDO_MODE_ACC_TRIM_BMSK            0x1f00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC0_CPU0_IMEAS_LDO_MODE_ACC_TRIM_SHFT                 0x14
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC1_ODCM_DAC_VREF_BMSK                             0xc0000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC1_ODCM_DAC_VREF_SHFT                                0x12
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC1_MEMCELL_VMIN_VREF_TRIM_BMSK                    0x38000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC1_MEMCELL_VMIN_VREF_TRIM_SHFT                        0xf
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC0_ODCM_DAC_VREF_BMSK                              0x6000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC0_ODCM_DAC_VREF_SHFT                                 0xd
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC0_MEMCELL_VMIN_VREF_TRIM_BMSK                     0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC0_MEMCELL_VMIN_VREF_TRIM_SHFT                        0xa
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC1_CPU1_IMEAS_BHS_MODE_ACC_TRIM_BMSK                0x3e0
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC1_CPU1_IMEAS_BHS_MODE_ACC_TRIM_SHFT                  0x5
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC1_CPU0_IMEAS_BHS_MODE_ACC_TRIM_BMSK                 0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS_APC1_CPU0_IMEAS_BHS_MODE_ACC_TRIM_SHFT                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f4)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE0_BMSK                                           0xf8000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE0_SHFT                                                 0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_BANDGAP_TRIM_BMSK                                      0x7f00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_BANDGAP_TRIM_SHFT                                           0x14
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_APPS_APC1_L2_IMEAS_LDO_MODE_ACC_TRIM_BMSK                0xf8000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_APPS_APC1_L2_IMEAS_LDO_MODE_ACC_TRIM_SHFT                    0xf
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_APPS_APC0_L2_IMEAS_LDO_MODE_ACC_TRIM_BMSK                 0x7c00
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_APPS_APC0_L2_IMEAS_LDO_MODE_ACC_TRIM_SHFT                    0xa
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_APPS_APC1_CPU1_IMEAS_LDO_MODE_ACC_TRIM_BMSK                0x3e0
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_APPS_APC1_CPU1_IMEAS_LDO_MODE_ACC_TRIM_SHFT                  0x5
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_APPS_APC1_CPU0_IMEAS_LDO_MODE_ACC_TRIM_BMSK                 0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_APPS_APC1_CPU0_IMEAS_LDO_MODE_ACC_TRIM_SHFT                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f8)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR1_TARG_VOLT_SVS_1_0_BMSK                           0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR1_TARG_VOLT_SVS_1_0_SHFT                                 0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR1_TARG_VOLT_NOM_BMSK                               0x3e000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR1_TARG_VOLT_NOM_SHFT                                     0x19
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR1_TARG_VOLT_TUR_BMSK                                0x1f00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR1_TARG_VOLT_TUR_SHFT                                     0x14
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS2_BMSK                                 0xf8000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS2_SHFT                                     0xf
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS_BMSK                                   0x7c00
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_SVS_SHFT                                      0xa
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_NOM_BMSK                                    0x3e0
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_NOM_SHFT                                      0x5
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_TUR_BMSK                                     0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_TUR_SHFT                                      0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041fc)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR3_TARG_VOLT_TUR_3_0_BMSK                           0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR3_TARG_VOLT_TUR_3_0_SHFT                                 0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR2_TARG_VOLT_SVS2_BMSK                               0xf800000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR2_TARG_VOLT_SVS2_SHFT                                    0x17
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR2_TARG_VOLT_SVS_BMSK                                 0x7c0000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR2_TARG_VOLT_SVS_SHFT                                     0x12
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR2_TARG_VOLT_NOM_BMSK                                  0x3e000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR2_TARG_VOLT_NOM_SHFT                                      0xd
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR2_TARG_VOLT_TUR_BMSK                                   0x1f00
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR2_TARG_VOLT_TUR_SHFT                                      0x8
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS2_BMSK                                    0xf8
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS2_SHFT                                     0x3
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS_4_2_BMSK                                  0x7
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_SVS_4_2_SHFT                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004200)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR5_TARG_VOLT_NOM_0_BMSK                             0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR5_TARG_VOLT_NOM_0_SHFT                                   0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR4_TARG_VOLT_SUTUR_BMSK                             0x7c000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR4_TARG_VOLT_SUTUR_SHFT                                   0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR4_TARG_VOLT_NOM_BMSK                                0x3e00000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR4_TARG_VOLT_NOM_SHFT                                     0x15
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR4_TARG_VOLT_TUR_BMSK                                 0x1f0000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR4_TARG_VOLT_TUR_SHFT                                     0x10
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR3_TARG_VOLT_SVS2_BMSK                                  0xf800
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR3_TARG_VOLT_SVS2_SHFT                                     0xb
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR3_TARG_VOLT_SVS_BMSK                                    0x7c0
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR3_TARG_VOLT_SVS_SHFT                                      0x6
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR3_TARG_VOLT_NOM_BMSK                                     0x3e
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR3_TARG_VOLT_NOM_SHFT                                      0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR3_TARG_VOLT_TUR_4_BMSK                                    0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_CPR3_TARG_VOLT_TUR_4_SHFT                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004204)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR6_TARG_VOLT_TUR_1_0_BMSK                           0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR6_TARG_VOLT_TUR_1_0_SHFT                                 0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR6_SVS2_ROSEL_BMSK                                  0x3c000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR6_SVS2_ROSEL_SHFT                                        0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR6_SVS_ROSEL_BMSK                                    0x3c00000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR6_SVS_ROSEL_SHFT                                         0x16
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR6_NOMINAL_ROSEL_BMSK                                 0x3c0000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR6_NOMINAL_ROSEL_SHFT                                     0x12
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR6_TURBO_ROSEL_BMSK                                    0x3c000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR6_TURBO_ROSEL_SHFT                                        0xe
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR5_TARG_VOLT_SVS2_BMSK                                  0x3e00
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR5_TARG_VOLT_SVS2_SHFT                                     0x9
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR5_TARG_VOLT_SVS_BMSK                                    0x1f0
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR5_TARG_VOLT_SVS_SHFT                                      0x4
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR5_TARG_VOLT_NOM_4_1_BMSK                                  0xf
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR5_TARG_VOLT_NOM_4_1_SHFT                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004208)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR6_TURBO_QUOT_VMIN_9_0_BMSK                         0xffc00000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR6_TURBO_QUOT_VMIN_9_0_SHFT                               0x16
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR6_TARG_VOLT_SVS2_BMSK                                0x3f0000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR6_TARG_VOLT_SVS2_SHFT                                    0x10
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR6_TARG_VOLT_SVS_BMSK                                   0xfc00
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR6_TARG_VOLT_SVS_SHFT                                      0xa
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR6_TARG_VOLT_NOM_BMSK                                    0x3f0
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR6_TARG_VOLT_NOM_SHFT                                      0x4
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR6_TARG_VOLT_TUR_5_2_BMSK                                  0xf
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CPR6_TARG_VOLT_TUR_5_2_SHFT                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000420c)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR6_SVS2_QUOT_VMIN_5_0_BMSK                          0xfc000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR6_SVS2_QUOT_VMIN_5_0_SHFT                                0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR6_SVS_QUOT_VMIN_BMSK                                0x3ffc000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR6_SVS_QUOT_VMIN_SHFT                                      0xe
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR6_NOMINAL_QUOT_VMIN_BMSK                               0x3ffc
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR6_NOMINAL_QUOT_VMIN_SHFT                                  0x2
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR6_TURBO_QUOT_VMIN_11_10_BMSK                              0x3
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CPR6_TURBO_QUOT_VMIN_11_10_SHFT                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004210)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR7_TURBO_ROSEL_1_0_BMSK                             0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR7_TURBO_ROSEL_1_0_SHFT                                   0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_SVS_QUOT_OFFSET_BMSK                             0x3fc00000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_SVS_QUOT_OFFSET_SHFT                                   0x16
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_NOMINAL_QUOT_OFFSET_BMSK                           0x3fc000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_NOMINAL_QUOT_OFFSET_SHFT                                0xe
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_TURBO_QUOT_OFFSET_BMSK                               0x3fc0
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_TURBO_QUOT_OFFSET_SHFT                                  0x6
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_SVS2_QUOT_VMIN_11_6_BMSK                               0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR6_SVS2_QUOT_VMIN_11_6_SHFT                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004214)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_TARG_VOLT_SVS_BMSK                               0xfc000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_TARG_VOLT_SVS_SHFT                                     0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_TARG_VOLT_NOM_BMSK                                0x3f00000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_TARG_VOLT_NOM_SHFT                                     0x14
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_TARG_VOLT_TUR_BMSK                                  0xfc000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_TARG_VOLT_TUR_SHFT                                      0xe
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_SVS2_ROSEL_BMSK                                      0x3c00
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_SVS2_ROSEL_SHFT                                         0xa
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_SVS_ROSEL_BMSK                                        0x3c0
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_SVS_ROSEL_SHFT                                          0x6
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_NOMINAL_ROSEL_BMSK                                     0x3c
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_NOMINAL_ROSEL_SHFT                                      0x2
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_TURBO_ROSEL_3_2_BMSK                                    0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR7_TURBO_ROSEL_3_2_SHFT                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004218)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR7_SVS_QUOT_VMIN_1_0_BMSK                           0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR7_SVS_QUOT_VMIN_1_0_SHFT                                 0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR7_NOMINAL_QUOT_VMIN_BMSK                           0x3ffc0000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR7_NOMINAL_QUOT_VMIN_SHFT                                 0x12
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR7_TURBO_QUOT_VMIN_BMSK                                0x3ffc0
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR7_TURBO_QUOT_VMIN_SHFT                                    0x6
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR7_TARG_VOLT_SVS2_BMSK                                    0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR7_TARG_VOLT_SVS2_SHFT                                     0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000421c)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR7_NOMINAL_QUOT_OFFSET_1_0_BMSK                     0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR7_NOMINAL_QUOT_OFFSET_1_0_SHFT                           0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR7_TURBO_QUOT_OFFSET_BMSK                           0x3fc00000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR7_TURBO_QUOT_OFFSET_SHFT                                 0x16
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR7_SVS2_QUOT_VMIN_BMSK                                0x3ffc00
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR7_SVS2_QUOT_VMIN_SHFT                                     0xa
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR7_SVS_QUOT_VMIN_11_2_BMSK                               0x3ff
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CPR7_SVS_QUOT_VMIN_11_2_SHFT                                 0x0

#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004220)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_GFX_AGING_RO_DELTA_BMSK                               0xfc000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_GFX_AGING_RO_DELTA_SHFT                                     0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CX_AGING_RO_DELTA_BMSK                                 0x3f00000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CX_AGING_RO_DELTA_SHFT                                      0x14
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_APC_AGING_RO_DELTA_BMSK                                  0xfc000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_APC_AGING_RO_DELTA_SHFT                                      0xe
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR7_SVS_QUOT_OFFSET_BMSK                                 0x3fc0
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR7_SVS_QUOT_OFFSET_SHFT                                    0x6
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR7_NOMINAL_QUOT_OFFSET_7_2_BMSK                           0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR7_NOMINAL_QUOT_OFFSET_7_2_SHFT                            0x0

#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004224)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_PH_B0M0_BMSK                                          0xe0000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_PH_B0M0_SHFT                                                0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_G_B0_BMSK                                             0x1c000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_G_B0_SHFT                                                   0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_SAR_B0_BMSK                                            0x3000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_SAR_B0_SHFT                                                 0x18
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_APPS_APC_ODCM_CAL_SEL_BMSK                              0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_APPS_APC_ODCM_CAL_SEL_SHFT                                  0x15
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD0_BMSK                                              0x1c0000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD0_SHFT                                                  0x12
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_SW_CAL_REDUN_273_256_BMSK                                0x3ffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_SW_CAL_REDUN_273_256_SHFT                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004228)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW8_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_VREF_B1_BMSK                                          0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_VREF_B1_SHFT                                                0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B1M3_BMSK                                          0x38000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B1M3_SHFT                                                0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B1M2_BMSK                                           0x7000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B1M2_SHFT                                                0x18
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B1M1_BMSK                                            0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B1M1_SHFT                                                0x15
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B1M0_BMSK                                            0x1c0000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B1M0_SHFT                                                0x12
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_G_B1_BMSK                                                0x38000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_G_B1_SHFT                                                    0xf
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_SAR_B1_BMSK                                               0x6000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_SAR_B1_SHFT                                                  0xd
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CLK_B0_BMSK                                               0x1800
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CLK_B0_SHFT                                                  0xb
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_VREF_B0_BMSK                                               0x600
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_VREF_B0_SHFT                                                 0x9
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B0M3_BMSK                                               0x1c0
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B0M3_SHFT                                                 0x6
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B0M2_BMSK                                                0x38
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B0M2_SHFT                                                 0x3
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B0M1_BMSK                                                 0x7
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_PH_B0M1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000422c)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD0_BMSK                                            0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD0_SHFT                                                  0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_PH_B3M0_BMSK                                          0x70000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_PH_B3M0_SHFT                                                0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_G_B3_BMSK                                              0xe000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_G_B3_SHFT                                                   0x19
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_SAR_B3_BMSK                                            0x1800000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_SAR_B3_SHFT                                                 0x17
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CLK_B2_BMSK                                             0x600000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CLK_B2_SHFT                                                 0x15
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_VREF_B2_BMSK                                            0x180000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_VREF_B2_SHFT                                                0x13
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_PH_B2M3_BMSK                                             0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_PH_B2M3_SHFT                                                0x10
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_PH_B2M2_BMSK                                              0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_PH_B2M2_SHFT                                                 0xd
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_PH_B2M1_BMSK                                              0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_PH_B2M1_SHFT                                                 0xa
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_PH_B2M0_BMSK                                               0x380
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_PH_B2M0_SHFT                                                 0x7
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_G_B2_BMSK                                                   0x70
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_G_B2_SHFT                                                    0x4
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_SAR_B2_BMSK                                                  0xc
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_SAR_B2_SHFT                                                  0x2
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CLK_B1_BMSK                                                  0x3
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CLK_B1_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004230)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW9_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_VREF_B4_BMSK                                          0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_VREF_B4_SHFT                                                0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B4M3_BMSK                                          0x38000000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B4M3_SHFT                                                0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B4M2_BMSK                                           0x7000000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B4M2_SHFT                                                0x18
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B4M1_BMSK                                            0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B4M1_SHFT                                                0x15
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B4M0_BMSK                                            0x1c0000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B4M0_SHFT                                                0x12
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_G_B4_BMSK                                                0x38000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_G_B4_SHFT                                                    0xf
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_SAR_B4_BMSK                                               0x6000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_SAR_B4_SHFT                                                  0xd
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CLK_B3_BMSK                                               0x1800
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CLK_B3_SHFT                                                  0xb
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_VREF_B3_BMSK                                               0x600
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_VREF_B3_SHFT                                                 0x9
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B3M3_BMSK                                               0x1c0
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B3M3_SHFT                                                 0x6
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B3M2_BMSK                                                0x38
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B3M2_SHFT                                                 0x3
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B3M1_BMSK                                                 0x7
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PH_B3M1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004234)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW9_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_RSVD0_BMSK                                            0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_RSVD0_SHFT                                                  0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PH_B6M0_BMSK                                          0x70000000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PH_B6M0_SHFT                                                0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_G_B6_BMSK                                              0xe000000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_G_B6_SHFT                                                   0x19
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_SAR_B6_BMSK                                            0x1800000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_SAR_B6_SHFT                                                 0x17
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CLK_B5_BMSK                                             0x600000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CLK_B5_SHFT                                                 0x15
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_VREF_B5_BMSK                                            0x180000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_VREF_B5_SHFT                                                0x13
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PH_B5M3_BMSK                                             0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PH_B5M3_SHFT                                                0x10
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PH_B5M2_BMSK                                              0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PH_B5M2_SHFT                                                 0xd
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PH_B5M1_BMSK                                              0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PH_B5M1_SHFT                                                 0xa
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PH_B5M0_BMSK                                               0x380
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PH_B5M0_SHFT                                                 0x7
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_G_B5_BMSK                                                   0x70
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_G_B5_SHFT                                                    0x4
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_SAR_B5_BMSK                                                  0xc
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_SAR_B5_SHFT                                                  0x2
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CLK_B4_BMSK                                                  0x3
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CLK_B4_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004238)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW10_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_TSENS0_BASE1_5_0_BMSK                                0xfc000000
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_TSENS0_BASE1_5_0_SHFT                                      0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_TSENS0_BASE0_BMSK                                     0x3ff0000
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_TSENS0_BASE0_SHFT                                          0x10
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_TSENS_CAL_SEL_BMSK                                       0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_TSENS_CAL_SEL_SHFT                                          0xd
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CLK_B6_BMSK                                              0x1800
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CLK_B6_SHFT                                                 0xb
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_VREF_B6_BMSK                                              0x600
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_VREF_B6_SHFT                                                0x9
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_PH_B6M3_BMSK                                              0x1c0
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_PH_B6M3_SHFT                                                0x6
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_PH_B6M2_BMSK                                               0x38
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_PH_B6M2_SHFT                                                0x3
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_PH_B6M1_BMSK                                                0x7
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_PH_B6M1_SHFT                                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000423c)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW10_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TSENS1_OFFSET_BMSK                                   0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TSENS1_OFFSET_SHFT                                         0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TSENS0_OFFSET_BMSK                                    0xf000000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TSENS0_OFFSET_SHFT                                         0x18
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TSENS1_BASE1_BMSK                                      0xffc000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TSENS1_BASE1_SHFT                                           0xe
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TSENS1_BASE0_BMSK                                        0x3ff0
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TSENS1_BASE0_SHFT                                           0x4
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TSENS0_BASE1_9_6_BMSK                                       0xf
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TSENS0_BASE1_9_6_SHFT                                       0x0

#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004240)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW11_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS9_OFFSET_BMSK                                   0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS9_OFFSET_SHFT                                         0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS8_OFFSET_BMSK                                    0xf000000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS8_OFFSET_SHFT                                         0x18
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS7_OFFSET_BMSK                                     0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS7_OFFSET_SHFT                                         0x14
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS6_OFFSET_BMSK                                      0xf0000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS6_OFFSET_SHFT                                         0x10
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS5_OFFSET_BMSK                                       0xf000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS5_OFFSET_SHFT                                          0xc
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS4_OFFSET_BMSK                                        0xf00
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS4_OFFSET_SHFT                                          0x8
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS3_OFFSET_BMSK                                         0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS3_OFFSET_SHFT                                          0x4
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS2_OFFSET_BMSK                                          0xf
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_TSENS2_OFFSET_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004244)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW11_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS17_OFFSET_BMSK                                  0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS17_OFFSET_SHFT                                        0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS16_OFFSET_BMSK                                   0xf000000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS16_OFFSET_SHFT                                        0x18
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS15_OFFSET_BMSK                                    0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS15_OFFSET_SHFT                                        0x14
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS14_OFFSET_BMSK                                     0xf0000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS14_OFFSET_SHFT                                        0x10
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS13_OFFSET_BMSK                                      0xf000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS13_OFFSET_SHFT                                         0xc
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS12_OFFSET_BMSK                                       0xf00
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS12_OFFSET_SHFT                                         0x8
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS11_OFFSET_BMSK                                        0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS11_OFFSET_SHFT                                         0x4
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS10_OFFSET_BMSK                                         0xf
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_TSENS10_OFFSET_SHFT                                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004248)
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW12_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW12_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW12_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC0_CAL_AVG_ERR_BMSK                              0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC0_CAL_AVG_ERR_SHFT                                    0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC_0_1_FLAG_BMSK                                  0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC_0_1_FLAG_SHFT                                        0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC1_CAL_OVFLOW_BMSK                               0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC1_CAL_OVFLOW_SHFT                                     0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC0_CAL_OVFLOW_BMSK                               0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC0_CAL_OVFLOW_SHFT                                     0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC1_CAL_BMSK                                       0xff00000
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC1_CAL_SHFT                                            0x14
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC0_CAL_BMSK                                         0xff000
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TXDAC0_CAL_SHFT                                             0xc
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TSENS20_OFFSET_BMSK                                       0xf00
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TSENS20_OFFSET_SHFT                                         0x8
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TSENS19_OFFSET_BMSK                                        0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TSENS19_OFFSET_SHFT                                         0x4
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TSENS18_OFFSET_BMSK                                         0xf
#define HWIO_QFPROM_CORR_CALIB_ROW12_LSB_TSENS18_OFFSET_SHFT                                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000424c)
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW12_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW12_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW12_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_SW_CAL_REDUN_SEL_BMSK                                0xe0000000
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_SW_CAL_REDUN_SEL_SHFT                                      0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_PORT1_HSTX_TRIM_LSB_BMSK                             0x1e000000
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_PORT1_HSTX_TRIM_LSB_SHFT                                   0x19
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_PORT0_HSTX_TRIM_LSB_BMSK                              0x1e00000
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_PORT0_HSTX_TRIM_LSB_SHFT                                   0x15
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_VOLTAGE_SENSOR_CALIB_BMSK                              0x1fff80
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_VOLTAGE_SENSOR_CALIB_SHFT                                   0x7
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_GNSS_ADC_VCM_BMSK                                          0x60
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_GNSS_ADC_VCM_SHFT                                           0x5
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_GNSS_ADC_LDO_BMSK                                          0x18
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_GNSS_ADC_LDO_SHFT                                           0x3
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_GNSS_ADC_VREF_BMSK                                          0x6
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_GNSS_ADC_VREF_SHFT                                          0x1
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_TXDAC1_CAL_AVG_ERR_BMSK                                     0x1
#define HWIO_QFPROM_CORR_CALIB_ROW12_MSB_TXDAC1_CAL_AVG_ERR_SHFT                                     0x0

#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004250)
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW13_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW13_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW13_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_SW_CAL_REDUN_31_0_BMSK                               0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW13_LSB_SW_CAL_REDUN_31_0_SHFT                                      0x0

#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004254)
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW13_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW13_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW13_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_SW_CAL_REDUN_63_32_BMSK                              0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW13_MSB_SW_CAL_REDUN_63_32_SHFT                                     0x0

#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004258)
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW14_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW14_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW14_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_SW_CAL_REDUN_95_64_BMSK                              0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW14_LSB_SW_CAL_REDUN_95_64_SHFT                                     0x0

#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000425c)
#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW14_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW14_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW14_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_SW_CAL_REDUN_127_96_BMSK                             0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW14_MSB_SW_CAL_REDUN_127_96_SHFT                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004260)
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW15_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW15_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW15_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_SW_CAL_REDUN_159_128_BMSK                            0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW15_LSB_SW_CAL_REDUN_159_128_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004264)
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW15_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW15_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW15_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_SW_CAL_REDUN_191_160_BMSK                            0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW15_MSB_SW_CAL_REDUN_191_160_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004268)
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW16_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW16_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW16_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_SW_CAL_REDUN_223_192_BMSK                            0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW16_LSB_SW_CAL_REDUN_223_192_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000426c)
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW16_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW16_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW16_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_SW_CAL_REDUN_255_224_BMSK                            0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW16_MSB_SW_CAL_REDUN_255_224_SHFT                                   0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004270 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_MAXn                                                     11
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004274 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_MAXn                                                     11
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                         0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REG15_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004310)
#define HWIO_QFPROM_CORR_QC_SPARE_REG15_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REG15_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REG15_LSB_ADDR, HWIO_QFPROM_CORR_QC_SPARE_REG15_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REG15_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REG15_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_QC_SPARE_REG15_LSB_QC_SPARE_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REG15_LSB_QC_SPARE_SHFT                                            0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REG15_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004314)
#define HWIO_QFPROM_CORR_QC_SPARE_REG15_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REG15_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REG15_MSB_ADDR, HWIO_QFPROM_CORR_QC_SPARE_REG15_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REG15_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REG15_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_QC_SPARE_REG15_MSB_QC_SPARE_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REG15_MSB_QC_SPARE_SHFT                                            0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REG16_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004318)
#define HWIO_QFPROM_CORR_QC_SPARE_REG16_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REG16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REG16_LSB_ADDR, HWIO_QFPROM_CORR_QC_SPARE_REG16_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REG16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REG16_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_QC_SPARE_REG16_LSB_QC_SPARE_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REG16_LSB_QC_SPARE_SHFT                                            0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REG16_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000431c)
#define HWIO_QFPROM_CORR_QC_SPARE_REG16_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REG16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REG16_MSB_ADDR, HWIO_QFPROM_CORR_QC_SPARE_REG16_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REG16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REG16_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_QC_SPARE_REG16_MSB_QC_SPARE_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REG16_MSB_QC_SPARE_SHFT                                            0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004298 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_MAXn                                                       21
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n), HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_QC_SPARE_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_QC_SPARE_SHFT                                             0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000429c + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_RMSK                                                 0xffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_MAXn                                                       21
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n), HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_QC_SPARE_BMSK                                        0xffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_QC_SPARE_SHFT                                             0x0

#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n)                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004348 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_LSB_RMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_LSB_MAXn                                               1
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_BMSK                            0xffffffff
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_SHFT                                   0x0

#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n)                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000434c + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_MSB_RMSK                                        0xffffff
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_MSB_MAXn                                               1
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_BMSK                              0xffffff
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_SHFT                                   0x0

#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_ADDR                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004358)
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_RMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_BMSK                                0xffff0000
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_SHFT                                      0x10
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_BMSK                                0xffff
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_SHFT                                   0x0

#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_MSB_ADDR                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000435c)
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_MSB_RMSK                                        0xffffff
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_BMSK                                  0xffffff
#define HWIO_QFPROM_CORR_QC_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_SHFT                                       0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n)                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004360 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_RMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_MAXn                                              1
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_BMSK                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_SHFT                                  0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n)                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004364 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RMSK                                       0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_MAXn                                              1
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_BMSK                             0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_SHFT                                  0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004370)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_BMSK                               0xffff0000
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_SHFT                                     0x10
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_BMSK                               0xffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_SHFT                                  0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004374)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RMSK                                       0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_BMSK                                 0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_SHFT                                      0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n)                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004378 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_MAXn                                                    1
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                 0xff000000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                       0x18
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                   0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                       0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                     0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                        0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                       0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                        0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n)                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000437c + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_RMSK                                             0xffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_MAXn                                                    1
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                   0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                       0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                     0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                        0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                       0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                        0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004388 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                          3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                              0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000438c + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                   0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                          3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                         0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                              0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043a8)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                              0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043ac)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                   0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_BMSK                             0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_SHFT                                  0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                          3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                              0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                   0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                          3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                         0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                              0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043d0)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                              0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043d4)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                   0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_BMSK                             0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_SHFT                                  0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043d8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_MAXn                                                      55
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_SHFT                                          0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043dc + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK                                                0xffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_MAXn                                                      55
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RSVD0_BMSK                                          0xfe0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RSVD0_SHFT                                              0x11
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_ADDR_BMSK                                      0x1fffe
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_ADDR_SHFT                                          0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                            0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                            0x0

#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004598)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_LSB_ADDR, HWIO_QFPROM_CORR_HDCP_KSV_LSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_KSV0_BMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_KSV0_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000459c)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_RMSK                                                      0xffffff
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_MSB_ADDR, HWIO_QFPROM_CORR_HDCP_KSV_MSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_RSVD0_BMSK                                                0xffff00
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_RSVD0_SHFT                                                     0x8
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_KSV1_BMSK                                                     0xff
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_KSV1_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004598 + 0x8 * (n))
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_MAXn                                                       40
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_KEY_DATA_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_KEY_DATA_SHFT                                             0x0

#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000459c + 0x8 * (n))
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_RMSK                                                 0xffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_MAXn                                                       40
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_KEY_DATA_BMSK                                        0xffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_KEY_DATA_SHFT                                             0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_ADDR(n)                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004510 + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_MAXn                                                 46
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_OEM_SPARE_BMSK                               0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_OEM_SPARE_SHFT                                      0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_ADDR(n)                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004514 + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_MAXn                                                 46
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_OEM_SPARE_BMSK                               0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_OEM_SPARE_SHFT                                      0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_ADDR(n)                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004518 + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_MAXn                                                 46
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_OEM_SPARE_BMSK                               0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_OEM_SPARE_SHFT                                      0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_ADDR(n)                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000451c + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_MAXn                                                 46
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_OEM_SPARE_BMSK                               0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_OEM_SPARE_SHFT                                      0x0

#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n)                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004800 + 0x4 * (n))
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
#define HWIO_FEATURE_CONFIG0_UFS_SW_CONTROL_DISABLE_BMSK                                      0x80000000
#define HWIO_FEATURE_CONFIG0_UFS_SW_CONTROL_DISABLE_SHFT                                            0x1f
#define HWIO_FEATURE_CONFIG0_SATA_DISABLE_BMSK                                                0x40000000
#define HWIO_FEATURE_CONFIG0_SATA_DISABLE_SHFT                                                      0x1e
#define HWIO_FEATURE_CONFIG0_SECURE_CHANNEL_DISABLE_BMSK                                      0x20000000
#define HWIO_FEATURE_CONFIG0_SECURE_CHANNEL_DISABLE_SHFT                                            0x1d
#define HWIO_FEATURE_CONFIG0_SCM_DISABLE_BMSK                                                 0x10000000
#define HWIO_FEATURE_CONFIG0_SCM_DISABLE_SHFT                                                       0x1c
#define HWIO_FEATURE_CONFIG0_ICE_FORCE_HW_KEY1_BMSK                                            0x8000000
#define HWIO_FEATURE_CONFIG0_ICE_FORCE_HW_KEY1_SHFT                                                 0x1b
#define HWIO_FEATURE_CONFIG0_ICE_FORCE_HW_KEY0_BMSK                                            0x4000000
#define HWIO_FEATURE_CONFIG0_ICE_FORCE_HW_KEY0_SHFT                                                 0x1a
#define HWIO_FEATURE_CONFIG0_ICE_DISABLE_BMSK                                                  0x2000000
#define HWIO_FEATURE_CONFIG0_ICE_DISABLE_SHFT                                                       0x19
#define HWIO_FEATURE_CONFIG0_RICA_DISABLE_BMSK                                                 0x1000000
#define HWIO_FEATURE_CONFIG0_RICA_DISABLE_SHFT                                                      0x18
#define HWIO_FEATURE_CONFIG0_SPI_SLAVE_DISABLE_BMSK                                             0x800000
#define HWIO_FEATURE_CONFIG0_SPI_SLAVE_DISABLE_SHFT                                                 0x17
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_BMSK                                              0x400000
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_SHFT                                                  0x16
#define HWIO_FEATURE_CONFIG0_GFX3D_TURBO_SEL1_BMSK                                              0x200000
#define HWIO_FEATURE_CONFIG0_GFX3D_TURBO_SEL1_SHFT                                                  0x15
#define HWIO_FEATURE_CONFIG0_GFX3D_TURBO_SEL0_BMSK                                              0x100000
#define HWIO_FEATURE_CONFIG0_GFX3D_TURBO_SEL0_SHFT                                                  0x14
#define HWIO_FEATURE_CONFIG0_GFX3D_TURBO_DISABLE_BMSK                                            0x80000
#define HWIO_FEATURE_CONFIG0_GFX3D_TURBO_DISABLE_SHFT                                               0x13
#define HWIO_FEATURE_CONFIG0_VENUS_HEVC_ENCODE_DISABLE_BMSK                                      0x40000
#define HWIO_FEATURE_CONFIG0_VENUS_HEVC_ENCODE_DISABLE_SHFT                                         0x12
#define HWIO_FEATURE_CONFIG0_VENUS_HEVC_DECODE_DISABLE_BMSK                                      0x20000
#define HWIO_FEATURE_CONFIG0_VENUS_HEVC_DECODE_DISABLE_SHFT                                         0x11
#define HWIO_FEATURE_CONFIG0_VENUS_4K_DISABLE_BMSK                                               0x10000
#define HWIO_FEATURE_CONFIG0_VENUS_4K_DISABLE_SHFT                                                  0x10
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_BMSK                                             0x8000
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_SHFT                                                0xf
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_BMSK                                             0x4000
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_SHFT                                                0xe
#define HWIO_FEATURE_CONFIG0_HDMI_DISABLE_BMSK                                                    0x2000
#define HWIO_FEATURE_CONFIG0_HDMI_DISABLE_SHFT                                                       0xd
#define HWIO_FEATURE_CONFIG0_HDCP_DISABLE_BMSK                                                    0x1000
#define HWIO_FEATURE_CONFIG0_HDCP_DISABLE_SHFT                                                       0xc
#define HWIO_FEATURE_CONFIG0_MDP_APICAL_LTC_DISABLE_BMSK                                           0x800
#define HWIO_FEATURE_CONFIG0_MDP_APICAL_LTC_DISABLE_SHFT                                             0xb
#define HWIO_FEATURE_CONFIG0_EDP_DISABLE_BMSK                                                      0x400
#define HWIO_FEATURE_CONFIG0_EDP_DISABLE_SHFT                                                        0xa
#define HWIO_FEATURE_CONFIG0_DSI_1_DISABLE_BMSK                                                    0x200
#define HWIO_FEATURE_CONFIG0_DSI_1_DISABLE_SHFT                                                      0x9
#define HWIO_FEATURE_CONFIG0_DSI_0_DISABLE_BMSK                                                    0x100
#define HWIO_FEATURE_CONFIG0_DSI_0_DISABLE_SHFT                                                      0x8
#define HWIO_FEATURE_CONFIG0_FD_DISABLE_BMSK                                                        0x80
#define HWIO_FEATURE_CONFIG0_FD_DISABLE_SHFT                                                         0x7
#define HWIO_FEATURE_CONFIG0_CSID_DPCM_14_DISABLE_BMSK                                              0x40
#define HWIO_FEATURE_CONFIG0_CSID_DPCM_14_DISABLE_SHFT                                               0x6
#define HWIO_FEATURE_CONFIG0_ISP_1_DISABLE_BMSK                                                     0x20
#define HWIO_FEATURE_CONFIG0_ISP_1_DISABLE_SHFT                                                      0x5
#define HWIO_FEATURE_CONFIG0_CSID_3_DISABLE_BMSK                                                    0x10
#define HWIO_FEATURE_CONFIG0_CSID_3_DISABLE_SHFT                                                     0x4
#define HWIO_FEATURE_CONFIG0_CSID_2_DISABLE_BMSK                                                     0x8
#define HWIO_FEATURE_CONFIG0_CSID_2_DISABLE_SHFT                                                     0x3
#define HWIO_FEATURE_CONFIG0_BOOT_ROM_PATCH_DISABLE_BMSK                                             0x7
#define HWIO_FEATURE_CONFIG0_BOOT_ROM_PATCH_DISABLE_SHFT                                             0x0

#define HWIO_FEATURE_CONFIG1_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006008)
#define HWIO_FEATURE_CONFIG1_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG1_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, HWIO_FEATURE_CONFIG1_RMSK)
#define HWIO_FEATURE_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, m)
#define HWIO_FEATURE_CONFIG1_RSVD0_BMSK                                                       0xff800000
#define HWIO_FEATURE_CONFIG1_RSVD0_SHFT                                                             0x17
#define HWIO_FEATURE_CONFIG1_APS_RESET_DISABLE_BMSK                                             0x400000
#define HWIO_FEATURE_CONFIG1_APS_RESET_DISABLE_SHFT                                                 0x16
#define HWIO_FEATURE_CONFIG1_HVX_DISABLE_BMSK                                                   0x200000
#define HWIO_FEATURE_CONFIG1_HVX_DISABLE_SHFT                                                       0x15
#define HWIO_FEATURE_CONFIG1_APB2JTAG_LIFECYCLE1_DIS_BMSK                                       0x100000
#define HWIO_FEATURE_CONFIG1_APB2JTAG_LIFECYCLE1_DIS_SHFT                                           0x14
#define HWIO_FEATURE_CONFIG1_APB2JTAG_LIFECYCLE0_EN_BMSK                                         0x80000
#define HWIO_FEATURE_CONFIG1_APB2JTAG_LIFECYCLE0_EN_SHFT                                            0x13
#define HWIO_FEATURE_CONFIG1_STACKED_MEMORY_ID_BMSK                                              0x7c000
#define HWIO_FEATURE_CONFIG1_STACKED_MEMORY_ID_SHFT                                                  0xe
#define HWIO_FEATURE_CONFIG1_PRNG_TESTMODE_DISABLE_BMSK                                           0x2000
#define HWIO_FEATURE_CONFIG1_PRNG_TESTMODE_DISABLE_SHFT                                              0xd
#define HWIO_FEATURE_CONFIG1_DCC_DISABLE_BMSK                                                     0x1000
#define HWIO_FEATURE_CONFIG1_DCC_DISABLE_SHFT                                                        0xc
#define HWIO_FEATURE_CONFIG1_MOCHA_PART_BMSK                                                       0x800
#define HWIO_FEATURE_CONFIG1_MOCHA_PART_SHFT                                                         0xb
#define HWIO_FEATURE_CONFIG1_NIDNT_DISABLE_BMSK                                                    0x400
#define HWIO_FEATURE_CONFIG1_NIDNT_DISABLE_SHFT                                                      0xa
#define HWIO_FEATURE_CONFIG1_SMMU_DISABLE_BMSK                                                     0x200
#define HWIO_FEATURE_CONFIG1_SMMU_DISABLE_SHFT                                                       0x9
#define HWIO_FEATURE_CONFIG1_VENDOR_LOCK_BMSK                                                      0x1e0
#define HWIO_FEATURE_CONFIG1_VENDOR_LOCK_SHFT                                                        0x5
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                       0x10
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                        0x4
#define HWIO_FEATURE_CONFIG1_NAV_DISABLE_BMSK                                                        0x8
#define HWIO_FEATURE_CONFIG1_NAV_DISABLE_SHFT                                                        0x3
#define HWIO_FEATURE_CONFIG1_PCIE_2_DISABLE_BMSK                                                     0x4
#define HWIO_FEATURE_CONFIG1_PCIE_2_DISABLE_SHFT                                                     0x2
#define HWIO_FEATURE_CONFIG1_PCIE_1_DISABLE_BMSK                                                     0x2
#define HWIO_FEATURE_CONFIG1_PCIE_1_DISABLE_SHFT                                                     0x1
#define HWIO_FEATURE_CONFIG1_PCIE_0_DISABLE_BMSK                                                     0x1
#define HWIO_FEATURE_CONFIG1_PCIE_0_DISABLE_SHFT                                                     0x0

#define HWIO_FEATURE_CONFIG2_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, HWIO_FEATURE_CONFIG2_RMSK)
#define HWIO_FEATURE_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, m)
#define HWIO_FEATURE_CONFIG2_RSVD0_BMSK                                                       0xffffffff
#define HWIO_FEATURE_CONFIG2_RSVD0_SHFT                                                              0x0

#define HWIO_FEATURE_CONFIG3_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006010)
#define HWIO_FEATURE_CONFIG3_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG3_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, HWIO_FEATURE_CONFIG3_RMSK)
#define HWIO_FEATURE_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, m)
#define HWIO_FEATURE_CONFIG3_RSVD0_BMSK                                                       0xffffffff
#define HWIO_FEATURE_CONFIG3_RSVD0_SHFT                                                              0x0

#define HWIO_FEATURE_CONFIG4_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006014)
#define HWIO_FEATURE_CONFIG4_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG4_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, HWIO_FEATURE_CONFIG4_RMSK)
#define HWIO_FEATURE_CONFIG4_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, m)
#define HWIO_FEATURE_CONFIG4_QC_DAP_NIDEN_DISABLE_BMSK                                        0x80000000
#define HWIO_FEATURE_CONFIG4_QC_DAP_NIDEN_DISABLE_SHFT                                              0x1f
#define HWIO_FEATURE_CONFIG4_QC_APPS1_NIDEN_DISABLE_BMSK                                      0x40000000
#define HWIO_FEATURE_CONFIG4_QC_APPS1_NIDEN_DISABLE_SHFT                                            0x1e
#define HWIO_FEATURE_CONFIG4_QC_APPS0_NIDEN_DISABLE_BMSK                                      0x20000000
#define HWIO_FEATURE_CONFIG4_QC_APPS0_NIDEN_DISABLE_SHFT                                            0x1d
#define HWIO_FEATURE_CONFIG4_QC_MSS_DBGEN_DISABLE_BMSK                                        0x10000000
#define HWIO_FEATURE_CONFIG4_QC_MSS_DBGEN_DISABLE_SHFT                                              0x1c
#define HWIO_FEATURE_CONFIG4_QC_GSS_A5_DBGEN_DISABLE_BMSK                                      0x8000000
#define HWIO_FEATURE_CONFIG4_QC_GSS_A5_DBGEN_DISABLE_SHFT                                           0x1b
#define HWIO_FEATURE_CONFIG4_QC_A5X_ISDB_DBGEN_DISABLE_BMSK                                    0x4000000
#define HWIO_FEATURE_CONFIG4_QC_A5X_ISDB_DBGEN_DISABLE_SHFT                                         0x1a
#define HWIO_FEATURE_CONFIG4_QC_VENUS_0_DBGEN_DISABLE_BMSK                                     0x2000000
#define HWIO_FEATURE_CONFIG4_QC_VENUS_0_DBGEN_DISABLE_SHFT                                          0x19
#define HWIO_FEATURE_CONFIG4_QC_SSC_Q6_DBGEN_DISABLE_BMSK                                      0x1000000
#define HWIO_FEATURE_CONFIG4_QC_SSC_Q6_DBGEN_DISABLE_SHFT                                           0x18
#define HWIO_FEATURE_CONFIG4_QC_SSC_DBGEN_DISABLE_BMSK                                          0x800000
#define HWIO_FEATURE_CONFIG4_QC_SSC_DBGEN_DISABLE_SHFT                                              0x17
#define HWIO_FEATURE_CONFIG4_QC_PIMEM_DBGEN_DISABLE_BMSK                                        0x400000
#define HWIO_FEATURE_CONFIG4_QC_PIMEM_DBGEN_DISABLE_SHFT                                            0x16
#define HWIO_FEATURE_CONFIG4_QC_RPM_DBGEN_DISABLE_BMSK                                          0x200000
#define HWIO_FEATURE_CONFIG4_QC_RPM_DBGEN_DISABLE_SHFT                                              0x15
#define HWIO_FEATURE_CONFIG4_QC_WCSS_DBGEN_DISABLE_BMSK                                         0x100000
#define HWIO_FEATURE_CONFIG4_QC_WCSS_DBGEN_DISABLE_SHFT                                             0x14
#define HWIO_FEATURE_CONFIG4_QC_LPASS_DBGEN_DISABLE_BMSK                                         0x80000
#define HWIO_FEATURE_CONFIG4_QC_LPASS_DBGEN_DISABLE_SHFT                                            0x13
#define HWIO_FEATURE_CONFIG4_QC_DAP_DBGEN_DISABLE_BMSK                                           0x40000
#define HWIO_FEATURE_CONFIG4_QC_DAP_DBGEN_DISABLE_SHFT                                              0x12
#define HWIO_FEATURE_CONFIG4_QC_APPS1_DBGEN_DISABLE_BMSK                                         0x20000
#define HWIO_FEATURE_CONFIG4_QC_APPS1_DBGEN_DISABLE_SHFT                                            0x11
#define HWIO_FEATURE_CONFIG4_QC_APPS0_DBGEN_DISABLE_BMSK                                         0x10000
#define HWIO_FEATURE_CONFIG4_QC_APPS0_DBGEN_DISABLE_SHFT                                            0x10
#define HWIO_FEATURE_CONFIG4_QC_DAP_DEVICEEN_DISABLE_BMSK                                         0x8000
#define HWIO_FEATURE_CONFIG4_QC_DAP_DEVICEEN_DISABLE_SHFT                                            0xf
#define HWIO_FEATURE_CONFIG4_QC_RPM_DAPEN_DISABLE_BMSK                                            0x4000
#define HWIO_FEATURE_CONFIG4_QC_RPM_DAPEN_DISABLE_SHFT                                               0xe
#define HWIO_FEATURE_CONFIG4_QC_SSC_Q6_ETM_DISABLE_BMSK                                           0x2000
#define HWIO_FEATURE_CONFIG4_QC_SSC_Q6_ETM_DISABLE_SHFT                                              0xd
#define HWIO_FEATURE_CONFIG4_QC_SPARE6_DISABLE_BMSK                                               0x1000
#define HWIO_FEATURE_CONFIG4_QC_SPARE6_DISABLE_SHFT                                                  0xc
#define HWIO_FEATURE_CONFIG4_QC_SPARE5_DISABLE_BMSK                                                0x800
#define HWIO_FEATURE_CONFIG4_QC_SPARE5_DISABLE_SHFT                                                  0xb
#define HWIO_FEATURE_CONFIG4_QC_SPARE4_DISABLE_BMSK                                                0x400
#define HWIO_FEATURE_CONFIG4_QC_SPARE4_DISABLE_SHFT                                                  0xa
#define HWIO_FEATURE_CONFIG4_QC_SPARE3_DISABLE_BMSK                                                0x200
#define HWIO_FEATURE_CONFIG4_QC_SPARE3_DISABLE_SHFT                                                  0x9
#define HWIO_FEATURE_CONFIG4_QC_SPARE2_DISABLE_BMSK                                                0x100
#define HWIO_FEATURE_CONFIG4_QC_SPARE2_DISABLE_SHFT                                                  0x8
#define HWIO_FEATURE_CONFIG4_QC_SPARE1_DISABLE_BMSK                                                 0x80
#define HWIO_FEATURE_CONFIG4_QC_SPARE1_DISABLE_SHFT                                                  0x7
#define HWIO_FEATURE_CONFIG4_QC_SPARE0_DISABLE_BMSK                                                 0x40
#define HWIO_FEATURE_CONFIG4_QC_SPARE0_DISABLE_SHFT                                                  0x6
#define HWIO_FEATURE_CONFIG4_QC_GPMU_NIDEN_DISABLE_BMSK                                             0x20
#define HWIO_FEATURE_CONFIG4_QC_GPMU_NIDEN_DISABLE_SHFT                                              0x5
#define HWIO_FEATURE_CONFIG4_QC_GPMU_DBGEN_DISABLE_BMSK                                             0x10
#define HWIO_FEATURE_CONFIG4_QC_GPMU_DBGEN_DISABLE_SHFT                                              0x4
#define HWIO_FEATURE_CONFIG4_QC_GPMU_DAPEN_DISABLE_BMSK                                              0x8
#define HWIO_FEATURE_CONFIG4_QC_GPMU_DAPEN_DISABLE_SHFT                                              0x3
#define HWIO_FEATURE_CONFIG4_QDI_SPMI_DISABLE_BMSK                                                   0x4
#define HWIO_FEATURE_CONFIG4_QDI_SPMI_DISABLE_SHFT                                                   0x2
#define HWIO_FEATURE_CONFIG4_SM_BIST_DISABLE_BMSK                                                    0x2
#define HWIO_FEATURE_CONFIG4_SM_BIST_DISABLE_SHFT                                                    0x1
#define HWIO_FEATURE_CONFIG4_TIC_DISABLE_BMSK                                                        0x1
#define HWIO_FEATURE_CONFIG4_TIC_DISABLE_SHFT                                                        0x0

#define HWIO_FEATURE_CONFIG5_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006018)
#define HWIO_FEATURE_CONFIG5_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG5_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, HWIO_FEATURE_CONFIG5_RMSK)
#define HWIO_FEATURE_CONFIG5_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, m)
#define HWIO_FEATURE_CONFIG5_SYS_APCSCPUCFGCRYPTODISABLE_BMSK                                 0x80000000
#define HWIO_FEATURE_CONFIG5_SYS_APCSCPUCFGCRYPTODISABLE_SHFT                                       0x1f
#define HWIO_FEATURE_CONFIG5_SYS_APCSCPUCFGRSTSCTLREL3V_BMSK                                  0x40000000
#define HWIO_FEATURE_CONFIG5_SYS_APCSCPUCFGRSTSCTLREL3V_SHFT                                        0x1e
#define HWIO_FEATURE_CONFIG5_SYS_APCSCPUCFGRSTSCTLREL3TE_BMSK                                 0x20000000
#define HWIO_FEATURE_CONFIG5_SYS_APCSCPUCFGRSTSCTLREL3TE_SHFT                                       0x1d
#define HWIO_FEATURE_CONFIG5_SYS_APCSCPUCFGRSTSCTLREL3EE_BMSK                                 0x10000000
#define HWIO_FEATURE_CONFIG5_SYS_APCSCPUCFGRSTSCTLREL3EE_SHFT                                       0x1c
#define HWIO_FEATURE_CONFIG5_APPS1_CFGCPUPRESENT_N_BMSK                                        0xc000000
#define HWIO_FEATURE_CONFIG5_APPS1_CFGCPUPRESENT_N_SHFT                                             0x1a
#define HWIO_FEATURE_CONFIG5_APPS0_CFGCPUPRESENT_N_BMSK                                        0x3000000
#define HWIO_FEATURE_CONFIG5_APPS0_CFGCPUPRESENT_N_SHFT                                             0x18
#define HWIO_FEATURE_CONFIG5_RSVD0_BMSK                                                         0xf80000
#define HWIO_FEATURE_CONFIG5_RSVD0_SHFT                                                             0x13
#define HWIO_FEATURE_CONFIG5_QC_GSS_A5_SPIDEN_DISABLE_BMSK                                       0x40000
#define HWIO_FEATURE_CONFIG5_QC_GSS_A5_SPIDEN_DISABLE_SHFT                                          0x12
#define HWIO_FEATURE_CONFIG5_QC_SSC_SPIDEN_DISABLE_BMSK                                          0x20000
#define HWIO_FEATURE_CONFIG5_QC_SSC_SPIDEN_DISABLE_SHFT                                             0x11
#define HWIO_FEATURE_CONFIG5_QC_PIMEM_SPIDEN_DISABLE_BMSK                                        0x10000
#define HWIO_FEATURE_CONFIG5_QC_PIMEM_SPIDEN_DISABLE_SHFT                                           0x10
#define HWIO_FEATURE_CONFIG5_QC_DAP_SPIDEN_DISABLE_BMSK                                           0x8000
#define HWIO_FEATURE_CONFIG5_QC_DAP_SPIDEN_DISABLE_SHFT                                              0xf
#define HWIO_FEATURE_CONFIG5_QC_APPS1_SPIDEN_DISABLE_BMSK                                         0x4000
#define HWIO_FEATURE_CONFIG5_QC_APPS1_SPIDEN_DISABLE_SHFT                                            0xe
#define HWIO_FEATURE_CONFIG5_QC_APPS0_SPIDEN_DISABLE_BMSK                                         0x2000
#define HWIO_FEATURE_CONFIG5_QC_APPS0_SPIDEN_DISABLE_SHFT                                            0xd
#define HWIO_FEATURE_CONFIG5_QC_GSS_A5_SPNIDEN_DISABLE_BMSK                                       0x1000
#define HWIO_FEATURE_CONFIG5_QC_GSS_A5_SPNIDEN_DISABLE_SHFT                                          0xc
#define HWIO_FEATURE_CONFIG5_QC_SSC_SPNIDEN_DISABLE_BMSK                                           0x800
#define HWIO_FEATURE_CONFIG5_QC_SSC_SPNIDEN_DISABLE_SHFT                                             0xb
#define HWIO_FEATURE_CONFIG5_QC_PIMEM_SPNIDEN_DISABLE_BMSK                                         0x400
#define HWIO_FEATURE_CONFIG5_QC_PIMEM_SPNIDEN_DISABLE_SHFT                                           0xa
#define HWIO_FEATURE_CONFIG5_QC_DAP_SPNIDEN_DISABLE_BMSK                                           0x200
#define HWIO_FEATURE_CONFIG5_QC_DAP_SPNIDEN_DISABLE_SHFT                                             0x9
#define HWIO_FEATURE_CONFIG5_QC_APPS1_SPNIDEN_DISABLE_BMSK                                         0x100
#define HWIO_FEATURE_CONFIG5_QC_APPS1_SPNIDEN_DISABLE_SHFT                                           0x8
#define HWIO_FEATURE_CONFIG5_QC_APPS0_SPNIDEN_DISABLE_BMSK                                          0x80
#define HWIO_FEATURE_CONFIG5_QC_APPS0_SPNIDEN_DISABLE_SHFT                                           0x7
#define HWIO_FEATURE_CONFIG5_QC_MSS_NIDEN_DISABLE_BMSK                                              0x40
#define HWIO_FEATURE_CONFIG5_QC_MSS_NIDEN_DISABLE_SHFT                                               0x6
#define HWIO_FEATURE_CONFIG5_QC_GSS_A5_NIDEN_DISABLE_BMSK                                           0x20
#define HWIO_FEATURE_CONFIG5_QC_GSS_A5_NIDEN_DISABLE_SHFT                                            0x5
#define HWIO_FEATURE_CONFIG5_QC_SSC_NIDEN_DISABLE_BMSK                                              0x10
#define HWIO_FEATURE_CONFIG5_QC_SSC_NIDEN_DISABLE_SHFT                                               0x4
#define HWIO_FEATURE_CONFIG5_QC_PIMEM_NIDEN_DISABLE_BMSK                                             0x8
#define HWIO_FEATURE_CONFIG5_QC_PIMEM_NIDEN_DISABLE_SHFT                                             0x3
#define HWIO_FEATURE_CONFIG5_QC_RPM_NIDEN_DISABLE_BMSK                                               0x4
#define HWIO_FEATURE_CONFIG5_QC_RPM_NIDEN_DISABLE_SHFT                                               0x2
#define HWIO_FEATURE_CONFIG5_QC_WCSS_NIDEN_DISABLE_BMSK                                              0x2
#define HWIO_FEATURE_CONFIG5_QC_WCSS_NIDEN_DISABLE_SHFT                                              0x1
#define HWIO_FEATURE_CONFIG5_QC_LPASS_NIDEN_DISABLE_BMSK                                             0x1
#define HWIO_FEATURE_CONFIG5_QC_LPASS_NIDEN_DISABLE_SHFT                                             0x0

#define HWIO_FEATURE_CONFIG6_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000601c)
#define HWIO_FEATURE_CONFIG6_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG6_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG6_ADDR, HWIO_FEATURE_CONFIG6_RMSK)
#define HWIO_FEATURE_CONFIG6_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG6_ADDR, m)
#define HWIO_FEATURE_CONFIG6_TAP_GEN_SPARE_INSTR_DISABLE_13_0_BMSK                            0xfffc0000
#define HWIO_FEATURE_CONFIG6_TAP_GEN_SPARE_INSTR_DISABLE_13_0_SHFT                                  0x12
#define HWIO_FEATURE_CONFIG6_TAP_INSTR_DISABLE_BMSK                                              0x3ffff
#define HWIO_FEATURE_CONFIG6_TAP_INSTR_DISABLE_SHFT                                                  0x0

#define HWIO_FEATURE_CONFIG7_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006020)
#define HWIO_FEATURE_CONFIG7_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG7_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG7_ADDR, HWIO_FEATURE_CONFIG7_RMSK)
#define HWIO_FEATURE_CONFIG7_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG7_ADDR, m)
#define HWIO_FEATURE_CONFIG7_SEC_TAP_ACCESS_DISABLE_BMSK                                      0xfffc0000
#define HWIO_FEATURE_CONFIG7_SEC_TAP_ACCESS_DISABLE_SHFT                                            0x12
#define HWIO_FEATURE_CONFIG7_TAP_GEN_SPARE_INSTR_DISABLE_31_14_BMSK                              0x3ffff
#define HWIO_FEATURE_CONFIG7_TAP_GEN_SPARE_INSTR_DISABLE_31_14_SHFT                                  0x0

#define HWIO_FEATURE_CONFIG8_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006024)
#define HWIO_FEATURE_CONFIG8_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG8_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG8_ADDR, HWIO_FEATURE_CONFIG8_RMSK)
#define HWIO_FEATURE_CONFIG8_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG8_ADDR, m)
#define HWIO_FEATURE_CONFIG8_MODEM_PBL_PATCH_VERSION_BMSK                                     0xf8000000
#define HWIO_FEATURE_CONFIG8_MODEM_PBL_PATCH_VERSION_SHFT                                           0x1b
#define HWIO_FEATURE_CONFIG8_APPS_PBL_PATCH_VERSION_BMSK                                       0x7c00000
#define HWIO_FEATURE_CONFIG8_APPS_PBL_PATCH_VERSION_SHFT                                            0x16
#define HWIO_FEATURE_CONFIG8_RSVD0_BMSK                                                         0x3c0000
#define HWIO_FEATURE_CONFIG8_RSVD0_SHFT                                                             0x12
#define HWIO_FEATURE_CONFIG8_APPS_PBL_BOOT_SPEED_BMSK                                            0x30000
#define HWIO_FEATURE_CONFIG8_APPS_PBL_BOOT_SPEED_SHFT                                               0x10
#define HWIO_FEATURE_CONFIG8_RPM_PBL_BOOT_SPEED_BMSK                                              0xc000
#define HWIO_FEATURE_CONFIG8_RPM_PBL_BOOT_SPEED_SHFT                                                 0xe
#define HWIO_FEATURE_CONFIG8_APPS_BOOT_FROM_ROM_BMSK                                              0x2000
#define HWIO_FEATURE_CONFIG8_APPS_BOOT_FROM_ROM_SHFT                                                 0xd
#define HWIO_FEATURE_CONFIG8_RPM_BOOT_FROM_ROM_BMSK                                               0x1000
#define HWIO_FEATURE_CONFIG8_RPM_BOOT_FROM_ROM_SHFT                                                  0xc
#define HWIO_FEATURE_CONFIG8_MODEM_BOOT_FROM_ROM_BMSK                                              0x800
#define HWIO_FEATURE_CONFIG8_MODEM_BOOT_FROM_ROM_SHFT                                                0xb
#define HWIO_FEATURE_CONFIG8_MSA_ENA_BMSK                                                          0x400
#define HWIO_FEATURE_CONFIG8_MSA_ENA_SHFT                                                            0xa
#define HWIO_FEATURE_CONFIG8_FORCE_MSA_AUTH_EN_BMSK                                                0x200
#define HWIO_FEATURE_CONFIG8_FORCE_MSA_AUTH_EN_SHFT                                                  0x9
#define HWIO_FEATURE_CONFIG8_ARM_CE_DISABLE_USAGE_BMSK                                             0x100
#define HWIO_FEATURE_CONFIG8_ARM_CE_DISABLE_USAGE_SHFT                                               0x8
#define HWIO_FEATURE_CONFIG8_BOOT_ROM_CFG_BMSK                                                      0xff
#define HWIO_FEATURE_CONFIG8_BOOT_ROM_CFG_SHFT                                                       0x0

#define HWIO_FEATURE_CONFIG9_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006028)
#define HWIO_FEATURE_CONFIG9_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG9_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG9_ADDR, HWIO_FEATURE_CONFIG9_RMSK)
#define HWIO_FEATURE_CONFIG9_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG9_ADDR, m)
#define HWIO_FEATURE_CONFIG9_RSVD0_BMSK                                                       0xffffc000
#define HWIO_FEATURE_CONFIG9_RSVD0_SHFT                                                              0xe
#define HWIO_FEATURE_CONFIG9_FOUNDRY_ID_BMSK                                                      0x3c00
#define HWIO_FEATURE_CONFIG9_FOUNDRY_ID_SHFT                                                         0xa
#define HWIO_FEATURE_CONFIG9_PLL_CFG_BMSK                                                          0x3f0
#define HWIO_FEATURE_CONFIG9_PLL_CFG_SHFT                                                            0x4
#define HWIO_FEATURE_CONFIG9_APPS_PBL_PLL_CTRL_BMSK                                                  0xf
#define HWIO_FEATURE_CONFIG9_APPS_PBL_PLL_CTRL_SHFT                                                  0x0

#define HWIO_OEM_CONFIG0_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000602c)
#define HWIO_OEM_CONFIG0_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG0_IN          \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, HWIO_OEM_CONFIG0_RMSK)
#define HWIO_OEM_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, m)
#define HWIO_OEM_CONFIG0_SPARE1_DISABLE_BMSK                                                  0x80000000
#define HWIO_OEM_CONFIG0_SPARE1_DISABLE_SHFT                                                        0x1f
#define HWIO_OEM_CONFIG0_SPARE0_DISABLE_BMSK                                                  0x40000000
#define HWIO_OEM_CONFIG0_SPARE0_DISABLE_SHFT                                                        0x1e
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_BMSK                                               0x20000000
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_SHFT                                                     0x1d
#define HWIO_OEM_CONFIG0_DEBUG_POLICY_DISABLE_BMSK                                            0x10000000
#define HWIO_OEM_CONFIG0_DEBUG_POLICY_DISABLE_SHFT                                                  0x1c
#define HWIO_OEM_CONFIG0_RSVD0_BMSK                                                            0xe000000
#define HWIO_OEM_CONFIG0_RSVD0_SHFT                                                                 0x19
#define HWIO_OEM_CONFIG0_MSS_HASH_INTEGRITY_CHECK_DISABLE_BMSK                                 0x1000000
#define HWIO_OEM_CONFIG0_MSS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                                      0x18
#define HWIO_OEM_CONFIG0_APPS_HASH_INTEGRITY_CHECK_DISABLE_BMSK                                 0x800000
#define HWIO_OEM_CONFIG0_APPS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                                     0x17
#define HWIO_OEM_CONFIG0_USB_SS_DISABLE_BMSK                                                    0x400000
#define HWIO_OEM_CONFIG0_USB_SS_DISABLE_SHFT                                                        0x16
#define HWIO_OEM_CONFIG0_SW_ROT_USE_SERIAL_NUM_BMSK                                             0x200000
#define HWIO_OEM_CONFIG0_SW_ROT_USE_SERIAL_NUM_SHFT                                                 0x15
#define HWIO_OEM_CONFIG0_DISABLE_ROT_TRANSFER_BMSK                                              0x100000
#define HWIO_OEM_CONFIG0_DISABLE_ROT_TRANSFER_SHFT                                                  0x14
#define HWIO_OEM_CONFIG0_IMAGE_ENCRYPTION_ENABLE_BMSK                                            0x80000
#define HWIO_OEM_CONFIG0_IMAGE_ENCRYPTION_ENABLE_SHFT                                               0x13
#define HWIO_OEM_CONFIG0_SDC1_SCM_FORCE_EFUSE_KEY_BMSK                                           0x40000
#define HWIO_OEM_CONFIG0_SDC1_SCM_FORCE_EFUSE_KEY_SHFT                                              0x12
#define HWIO_OEM_CONFIG0_UFS_SCM_FORCE_EFUSE_KEY_BMSK                                            0x20000
#define HWIO_OEM_CONFIG0_UFS_SCM_FORCE_EFUSE_KEY_SHFT                                               0x11
#define HWIO_OEM_CONFIG0_SDC2_SCM_FORCE_EFUSE_KEY_BMSK                                           0x10000
#define HWIO_OEM_CONFIG0_SDC2_SCM_FORCE_EFUSE_KEY_SHFT                                              0x10
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_BMSK                                                     0x8000
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_SHFT                                                        0xf
#define HWIO_OEM_CONFIG0_WDOG_EN_BMSK                                                             0x4000
#define HWIO_OEM_CONFIG0_WDOG_EN_SHFT                                                                0xe
#define HWIO_OEM_CONFIG0_SPDM_SECURE_MODE_BMSK                                                    0x2000
#define HWIO_OEM_CONFIG0_SPDM_SECURE_MODE_SHFT                                                       0xd
#define HWIO_OEM_CONFIG0_SW_FUSE_PROG_DISABLE_BMSK                                                0x1000
#define HWIO_OEM_CONFIG0_SW_FUSE_PROG_DISABLE_SHFT                                                   0xc
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                        0x800
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                          0xb
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_BMSK                                                  0x400
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_SHFT                                                    0xa
#define HWIO_OEM_CONFIG0_FAST_BOOT_BMSK                                                            0x3e0
#define HWIO_OEM_CONFIG0_FAST_BOOT_SHFT                                                              0x5
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_BMSK                                                   0x10
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_SHFT                                                    0x4
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_BMSK                                                 0x8
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_SHFT                                                 0x3
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_BMSK                                                    0x4
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_SHFT                                                    0x2
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_BMSK                                                           0x2
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_SHFT                                                           0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_BMSK                                                        0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_SHFT                                                        0x0

#define HWIO_OEM_CONFIG1_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006030)
#define HWIO_OEM_CONFIG1_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG1_IN          \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, HWIO_OEM_CONFIG1_RMSK)
#define HWIO_OEM_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, m)
#define HWIO_OEM_CONFIG1_APPS0_SPNIDEN_DISABLE_BMSK                                           0x80000000
#define HWIO_OEM_CONFIG1_APPS0_SPNIDEN_DISABLE_SHFT                                                 0x1f
#define HWIO_OEM_CONFIG1_MSS_NIDEN_DISABLE_BMSK                                               0x40000000
#define HWIO_OEM_CONFIG1_MSS_NIDEN_DISABLE_SHFT                                                     0x1e
#define HWIO_OEM_CONFIG1_GSS_A5_NIDEN_DISABLE_BMSK                                            0x20000000
#define HWIO_OEM_CONFIG1_GSS_A5_NIDEN_DISABLE_SHFT                                                  0x1d
#define HWIO_OEM_CONFIG1_SSC_NIDEN_DISABLE_BMSK                                               0x10000000
#define HWIO_OEM_CONFIG1_SSC_NIDEN_DISABLE_SHFT                                                     0x1c
#define HWIO_OEM_CONFIG1_PIMEM_NIDEN_DISABLE_BMSK                                              0x8000000
#define HWIO_OEM_CONFIG1_PIMEM_NIDEN_DISABLE_SHFT                                                   0x1b
#define HWIO_OEM_CONFIG1_RPM_NIDEN_DISABLE_BMSK                                                0x4000000
#define HWIO_OEM_CONFIG1_RPM_NIDEN_DISABLE_SHFT                                                     0x1a
#define HWIO_OEM_CONFIG1_WCSS_NIDEN_DISABLE_BMSK                                               0x2000000
#define HWIO_OEM_CONFIG1_WCSS_NIDEN_DISABLE_SHFT                                                    0x19
#define HWIO_OEM_CONFIG1_LPASS_NIDEN_DISABLE_BMSK                                              0x1000000
#define HWIO_OEM_CONFIG1_LPASS_NIDEN_DISABLE_SHFT                                                   0x18
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_BMSK                                                 0x800000
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_SHFT                                                     0x17
#define HWIO_OEM_CONFIG1_APPS1_NIDEN_DISABLE_BMSK                                               0x400000
#define HWIO_OEM_CONFIG1_APPS1_NIDEN_DISABLE_SHFT                                                   0x16
#define HWIO_OEM_CONFIG1_APPS0_NIDEN_DISABLE_BMSK                                               0x200000
#define HWIO_OEM_CONFIG1_APPS0_NIDEN_DISABLE_SHFT                                                   0x15
#define HWIO_OEM_CONFIG1_MSS_DBGEN_DISABLE_BMSK                                                 0x100000
#define HWIO_OEM_CONFIG1_MSS_DBGEN_DISABLE_SHFT                                                     0x14
#define HWIO_OEM_CONFIG1_GSS_A5_DBGEN_DISABLE_BMSK                                               0x80000
#define HWIO_OEM_CONFIG1_GSS_A5_DBGEN_DISABLE_SHFT                                                  0x13
#define HWIO_OEM_CONFIG1_A5X_ISDB_DBGEN_DISABLE_BMSK                                             0x40000
#define HWIO_OEM_CONFIG1_A5X_ISDB_DBGEN_DISABLE_SHFT                                                0x12
#define HWIO_OEM_CONFIG1_VENUS_0_DBGEN_DISABLE_BMSK                                              0x20000
#define HWIO_OEM_CONFIG1_VENUS_0_DBGEN_DISABLE_SHFT                                                 0x11
#define HWIO_OEM_CONFIG1_SSC_Q6_DBGEN_DISABLE_BMSK                                               0x10000
#define HWIO_OEM_CONFIG1_SSC_Q6_DBGEN_DISABLE_SHFT                                                  0x10
#define HWIO_OEM_CONFIG1_SSC_DBGEN_DISABLE_BMSK                                                   0x8000
#define HWIO_OEM_CONFIG1_SSC_DBGEN_DISABLE_SHFT                                                      0xf
#define HWIO_OEM_CONFIG1_PIMEM_DBGEN_DISABLE_BMSK                                                 0x4000
#define HWIO_OEM_CONFIG1_PIMEM_DBGEN_DISABLE_SHFT                                                    0xe
#define HWIO_OEM_CONFIG1_RPM_DBGEN_DISABLE_BMSK                                                   0x2000
#define HWIO_OEM_CONFIG1_RPM_DBGEN_DISABLE_SHFT                                                      0xd
#define HWIO_OEM_CONFIG1_WCSS_DBGEN_DISABLE_BMSK                                                  0x1000
#define HWIO_OEM_CONFIG1_WCSS_DBGEN_DISABLE_SHFT                                                     0xc
#define HWIO_OEM_CONFIG1_LPASS_DBGEN_DISABLE_BMSK                                                  0x800
#define HWIO_OEM_CONFIG1_LPASS_DBGEN_DISABLE_SHFT                                                    0xb
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_BMSK                                                    0x400
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_SHFT                                                      0xa
#define HWIO_OEM_CONFIG1_APPS1_DBGEN_DISABLE_BMSK                                                  0x200
#define HWIO_OEM_CONFIG1_APPS1_DBGEN_DISABLE_SHFT                                                    0x9
#define HWIO_OEM_CONFIG1_APPS0_DBGEN_DISABLE_BMSK                                                  0x100
#define HWIO_OEM_CONFIG1_APPS0_DBGEN_DISABLE_SHFT                                                    0x8
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_BMSK                                                  0x80
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_SHFT                                                   0x7
#define HWIO_OEM_CONFIG1_RPM_DAPEN_DISABLE_BMSK                                                     0x40
#define HWIO_OEM_CONFIG1_RPM_DAPEN_DISABLE_SHFT                                                      0x6
#define HWIO_OEM_CONFIG1_SSC_Q6_ETM_DISABLE_BMSK                                                    0x20
#define HWIO_OEM_CONFIG1_SSC_Q6_ETM_DISABLE_SHFT                                                     0x5
#define HWIO_OEM_CONFIG1_SPARE6_DISABLE_BMSK                                                        0x10
#define HWIO_OEM_CONFIG1_SPARE6_DISABLE_SHFT                                                         0x4
#define HWIO_OEM_CONFIG1_SPARE5_DISABLE_BMSK                                                         0x8
#define HWIO_OEM_CONFIG1_SPARE5_DISABLE_SHFT                                                         0x3
#define HWIO_OEM_CONFIG1_SPARE4_DISABLE_BMSK                                                         0x4
#define HWIO_OEM_CONFIG1_SPARE4_DISABLE_SHFT                                                         0x2
#define HWIO_OEM_CONFIG1_SPARE3_DISABLE_BMSK                                                         0x2
#define HWIO_OEM_CONFIG1_SPARE3_DISABLE_SHFT                                                         0x1
#define HWIO_OEM_CONFIG1_SPARE2_DISABLE_BMSK                                                         0x1
#define HWIO_OEM_CONFIG1_SPARE2_DISABLE_SHFT                                                         0x0

#define HWIO_OEM_CONFIG2_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006034)
#define HWIO_OEM_CONFIG2_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG2_IN          \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, HWIO_OEM_CONFIG2_RMSK)
#define HWIO_OEM_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, m)
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG46_SECURE_BMSK                                          0x80000000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG46_SECURE_SHFT                                                0x1f
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG45_SECURE_BMSK                                          0x40000000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG45_SECURE_SHFT                                                0x1e
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG44_SECURE_BMSK                                          0x20000000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG44_SECURE_SHFT                                                0x1d
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG43_SECURE_BMSK                                          0x10000000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG43_SECURE_SHFT                                                0x1c
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG42_SECURE_BMSK                                           0x8000000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG42_SECURE_SHFT                                                0x1b
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG41_SECURE_BMSK                                           0x4000000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG41_SECURE_SHFT                                                0x1a
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG40_SECURE_BMSK                                           0x2000000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG40_SECURE_SHFT                                                0x19
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG39_SECURE_BMSK                                           0x1000000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG39_SECURE_SHFT                                                0x18
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG38_SECURE_BMSK                                            0x800000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG38_SECURE_SHFT                                                0x17
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG37_SECURE_BMSK                                            0x400000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG37_SECURE_SHFT                                                0x16
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG36_SECURE_BMSK                                            0x200000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG36_SECURE_SHFT                                                0x15
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG35_SECURE_BMSK                                            0x100000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG35_SECURE_SHFT                                                0x14
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG34_SECURE_BMSK                                             0x80000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG34_SECURE_SHFT                                                0x13
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG33_SECURE_BMSK                                             0x40000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG33_SECURE_SHFT                                                0x12
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG32_SECURE_BMSK                                             0x20000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG32_SECURE_SHFT                                                0x11
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG31_SECURE_BMSK                                             0x10000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG31_SECURE_SHFT                                                0x10
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG30_SECURE_BMSK                                              0x8000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG30_SECURE_SHFT                                                 0xf
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG29_SECURE_BMSK                                              0x4000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG29_SECURE_SHFT                                                 0xe
#define HWIO_OEM_CONFIG2_RSVD0_BMSK                                                               0x3800
#define HWIO_OEM_CONFIG2_RSVD0_SHFT                                                                  0xb
#define HWIO_OEM_CONFIG2_GSS_A5_SPIDEN_DISABLE_BMSK                                                0x400
#define HWIO_OEM_CONFIG2_GSS_A5_SPIDEN_DISABLE_SHFT                                                  0xa
#define HWIO_OEM_CONFIG2_SSC_SPIDEN_DISABLE_BMSK                                                   0x200
#define HWIO_OEM_CONFIG2_SSC_SPIDEN_DISABLE_SHFT                                                     0x9
#define HWIO_OEM_CONFIG2_PIMEM_SPIDEN_DISABLE_BMSK                                                 0x100
#define HWIO_OEM_CONFIG2_PIMEM_SPIDEN_DISABLE_SHFT                                                   0x8
#define HWIO_OEM_CONFIG2_DAP_SPIDEN_DISABLE_BMSK                                                    0x80
#define HWIO_OEM_CONFIG2_DAP_SPIDEN_DISABLE_SHFT                                                     0x7
#define HWIO_OEM_CONFIG2_APPS1_SPIDEN_DISABLE_BMSK                                                  0x40
#define HWIO_OEM_CONFIG2_APPS1_SPIDEN_DISABLE_SHFT                                                   0x6
#define HWIO_OEM_CONFIG2_APPS0_SPIDEN_DISABLE_BMSK                                                  0x20
#define HWIO_OEM_CONFIG2_APPS0_SPIDEN_DISABLE_SHFT                                                   0x5
#define HWIO_OEM_CONFIG2_GSS_A5_SPNIDEN_DISABLE_BMSK                                                0x10
#define HWIO_OEM_CONFIG2_GSS_A5_SPNIDEN_DISABLE_SHFT                                                 0x4
#define HWIO_OEM_CONFIG2_SSC_SPNIDEN_DISABLE_BMSK                                                    0x8
#define HWIO_OEM_CONFIG2_SSC_SPNIDEN_DISABLE_SHFT                                                    0x3
#define HWIO_OEM_CONFIG2_PIMEM_SPNIDEN_DISABLE_BMSK                                                  0x4
#define HWIO_OEM_CONFIG2_PIMEM_SPNIDEN_DISABLE_SHFT                                                  0x2
#define HWIO_OEM_CONFIG2_DAP_SPNIDEN_DISABLE_BMSK                                                    0x2
#define HWIO_OEM_CONFIG2_DAP_SPNIDEN_DISABLE_SHFT                                                    0x1
#define HWIO_OEM_CONFIG2_APPS1_SPNIDEN_DISABLE_BMSK                                                  0x1
#define HWIO_OEM_CONFIG2_APPS1_SPNIDEN_DISABLE_SHFT                                                  0x0

#define HWIO_OEM_CONFIG3_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006038)
#define HWIO_OEM_CONFIG3_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG3_IN          \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, HWIO_OEM_CONFIG3_RMSK)
#define HWIO_OEM_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, m)
#define HWIO_OEM_CONFIG3_OEM_PRODUCT_ID_BMSK                                                  0xffff0000
#define HWIO_OEM_CONFIG3_OEM_PRODUCT_ID_SHFT                                                        0x10
#define HWIO_OEM_CONFIG3_OEM_HW_ID_BMSK                                                           0xffff
#define HWIO_OEM_CONFIG3_OEM_HW_ID_SHFT                                                              0x0

#define HWIO_OEM_CONFIG4_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000603c)
#define HWIO_OEM_CONFIG4_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG4_IN          \
        in_dword_masked(HWIO_OEM_CONFIG4_ADDR, HWIO_OEM_CONFIG4_RMSK)
#define HWIO_OEM_CONFIG4_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG4_ADDR, m)
#define HWIO_OEM_CONFIG4_PERIPH_VID_BMSK                                                      0xffff0000
#define HWIO_OEM_CONFIG4_PERIPH_VID_SHFT                                                            0x10
#define HWIO_OEM_CONFIG4_PERIPH_PID_BMSK                                                          0xffff
#define HWIO_OEM_CONFIG4_PERIPH_PID_SHFT                                                             0x0

#define HWIO_OEM_CONFIG5_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006040)
#define HWIO_OEM_CONFIG5_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG5_IN          \
        in_dword_masked(HWIO_OEM_CONFIG5_ADDR, HWIO_OEM_CONFIG5_RMSK)
#define HWIO_OEM_CONFIG5_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG5_ADDR, m)
#define HWIO_OEM_CONFIG5_RSVD0_BMSK                                                           0xffffff00
#define HWIO_OEM_CONFIG5_RSVD0_SHFT                                                                  0x8
#define HWIO_OEM_CONFIG5_ANTI_ROLLBACK_FEATURE_EN_BMSK                                              0xff
#define HWIO_OEM_CONFIG5_ANTI_ROLLBACK_FEATURE_EN_SHFT                                               0x0

#define HWIO_BOOT_CONFIG_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006044)
#define HWIO_BOOT_CONFIG_RMSK                                                                      0x7ff
#define HWIO_BOOT_CONFIG_IN          \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, HWIO_BOOT_CONFIG_RMSK)
#define HWIO_BOOT_CONFIG_INM(m)      \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, m)
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_BMSK                                                    0x400
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_SHFT                                                      0xa
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_BMSK                                                  0x300
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_SHFT                                                    0x8
#define HWIO_BOOT_CONFIG_RPM_BOOT_FROM_ROM_BMSK                                                     0x80
#define HWIO_BOOT_CONFIG_RPM_BOOT_FROM_ROM_SHFT                                                      0x7
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_BMSK                                                    0x40
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_SHFT                                                     0x6
#define HWIO_BOOT_CONFIG_FAST_BOOT_BMSK                                                             0x3e
#define HWIO_BOOT_CONFIG_FAST_BOOT_SHFT                                                              0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_BMSK                                                                0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_SHFT                                                                0x0

#define HWIO_SECURE_BOOTn_ADDR(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006048 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_RMSK                                                                     0x1ff
#define HWIO_SECURE_BOOTn_MAXn                                                                        14
#define HWIO_SECURE_BOOTn_INI(n)        \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), HWIO_SECURE_BOOTn_RMSK)
#define HWIO_SECURE_BOOTn_INMI(n,mask)    \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), mask)
#define HWIO_SECURE_BOOTn_FUSE_SRC_BMSK                                                            0x100
#define HWIO_SECURE_BOOTn_FUSE_SRC_SHFT                                                              0x8
#define HWIO_SECURE_BOOTn_RSVD_7_BMSK                                                               0x80
#define HWIO_SECURE_BOOTn_RSVD_7_SHFT                                                                0x7
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_BMSK                                                       0x40
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_SHFT                                                        0x6
#define HWIO_SECURE_BOOTn_AUTH_EN_BMSK                                                              0x20
#define HWIO_SECURE_BOOTn_AUTH_EN_SHFT                                                               0x5
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_BMSK                                                      0x10
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHFT                                                       0x4
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_BMSK                                                     0xf
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_SHFT                                                     0x0

#define HWIO_OVERRIDE_0_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c0)
#define HWIO_OVERRIDE_0_RMSK                                                                  0xffffffff
#define HWIO_OVERRIDE_0_IN          \
        in_dword_masked(HWIO_OVERRIDE_0_ADDR, HWIO_OVERRIDE_0_RMSK)
#define HWIO_OVERRIDE_0_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_0_ADDR, m)
#define HWIO_OVERRIDE_0_OUT(v)      \
        out_dword(HWIO_OVERRIDE_0_ADDR,v)
#define HWIO_OVERRIDE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_0_ADDR,m,v,HWIO_OVERRIDE_0_IN)
#define HWIO_OVERRIDE_0_RSVD_31_3_BMSK                                                        0xfffffff8
#define HWIO_OVERRIDE_0_RSVD_31_3_SHFT                                                               0x3
#define HWIO_OVERRIDE_0_TX_DISABLE_BMSK                                                              0x4
#define HWIO_OVERRIDE_0_TX_DISABLE_SHFT                                                              0x2
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_BMSK                                                     0x2
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_SHFT                                                     0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_BMSK                                               0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_SHFT                                               0x0

#define HWIO_OVERRIDE_1_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c4)
#define HWIO_OVERRIDE_1_RMSK                                                                  0xffffffff
#define HWIO_OVERRIDE_1_IN          \
        in_dword_masked(HWIO_OVERRIDE_1_ADDR, HWIO_OVERRIDE_1_RMSK)
#define HWIO_OVERRIDE_1_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_1_ADDR, m)
#define HWIO_OVERRIDE_1_OUT(v)      \
        out_dword(HWIO_OVERRIDE_1_ADDR,v)
#define HWIO_OVERRIDE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_1_ADDR,m,v,HWIO_OVERRIDE_1_IN)
#define HWIO_OVERRIDE_1_RSVD_31_10_BMSK                                                       0xfffffc00
#define HWIO_OVERRIDE_1_RSVD_31_10_SHFT                                                              0xa
#define HWIO_OVERRIDE_1_OVRID_DAP_DEVICEEN_DISABLE_BMSK                                            0x200
#define HWIO_OVERRIDE_1_OVRID_DAP_DEVICEEN_DISABLE_SHFT                                              0x9
#define HWIO_OVERRIDE_1_OVRID_RPM_DAPEN_DISABLE_BMSK                                               0x100
#define HWIO_OVERRIDE_1_OVRID_RPM_DAPEN_DISABLE_SHFT                                                 0x8
#define HWIO_OVERRIDE_1_OVRID_SSC_Q6_ETM_DISABLE_BMSK                                               0x80
#define HWIO_OVERRIDE_1_OVRID_SSC_Q6_ETM_DISABLE_SHFT                                                0x7
#define HWIO_OVERRIDE_1_OVRID_SPARE6_DISABLE_BMSK                                                   0x40
#define HWIO_OVERRIDE_1_OVRID_SPARE6_DISABLE_SHFT                                                    0x6
#define HWIO_OVERRIDE_1_OVRID_SPARE5_DISABLE_BMSK                                                   0x20
#define HWIO_OVERRIDE_1_OVRID_SPARE5_DISABLE_SHFT                                                    0x5
#define HWIO_OVERRIDE_1_OVRID_SPARE4_DISABLE_BMSK                                                   0x10
#define HWIO_OVERRIDE_1_OVRID_SPARE4_DISABLE_SHFT                                                    0x4
#define HWIO_OVERRIDE_1_OVRID_SPARE3_DISABLE_BMSK                                                    0x8
#define HWIO_OVERRIDE_1_OVRID_SPARE3_DISABLE_SHFT                                                    0x3
#define HWIO_OVERRIDE_1_OVRID_SPARE2_DISABLE_BMSK                                                    0x4
#define HWIO_OVERRIDE_1_OVRID_SPARE2_DISABLE_SHFT                                                    0x2
#define HWIO_OVERRIDE_1_OVRID_SPARE1_DISABLE_BMSK                                                    0x2
#define HWIO_OVERRIDE_1_OVRID_SPARE1_DISABLE_SHFT                                                    0x1
#define HWIO_OVERRIDE_1_OVRID_SPARE0_DISABLE_BMSK                                                    0x1
#define HWIO_OVERRIDE_1_OVRID_SPARE0_DISABLE_SHFT                                                    0x0

#define HWIO_OVERRIDE_2_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c8)
#define HWIO_OVERRIDE_2_RMSK                                                                  0xffffffff
#define HWIO_OVERRIDE_2_IN          \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, HWIO_OVERRIDE_2_RMSK)
#define HWIO_OVERRIDE_2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, m)
#define HWIO_OVERRIDE_2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_2_ADDR,v)
#define HWIO_OVERRIDE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_2_ADDR,m,v,HWIO_OVERRIDE_2_IN)
#define HWIO_OVERRIDE_2_RSVD_31_19_BMSK                                                       0xfff80000
#define HWIO_OVERRIDE_2_RSVD_31_19_SHFT                                                             0x13
#define HWIO_OVERRIDE_2_OVRID_SSC_NIDEN_DISABLE_BMSK                                             0x40000
#define HWIO_OVERRIDE_2_OVRID_SSC_NIDEN_DISABLE_SHFT                                                0x12
#define HWIO_OVERRIDE_2_OVRID_PIMEM_NIDEN_DISABLE_BMSK                                           0x20000
#define HWIO_OVERRIDE_2_OVRID_PIMEM_NIDEN_DISABLE_SHFT                                              0x11
#define HWIO_OVERRIDE_2_OVRID_RPM_NIDEN_DISABLE_BMSK                                             0x10000
#define HWIO_OVERRIDE_2_OVRID_RPM_NIDEN_DISABLE_SHFT                                                0x10
#define HWIO_OVERRIDE_2_OVRID_WCSS_NIDEN_DISABLE_BMSK                                             0x8000
#define HWIO_OVERRIDE_2_OVRID_WCSS_NIDEN_DISABLE_SHFT                                                0xf
#define HWIO_OVERRIDE_2_OVRID_LPASS_NIDEN_DISABLE_BMSK                                            0x4000
#define HWIO_OVERRIDE_2_OVRID_LPASS_NIDEN_DISABLE_SHFT                                               0xe
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_BMSK                                              0x2000
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_SHFT                                                 0xd
#define HWIO_OVERRIDE_2_OVRID_APPS1_NIDEN_DISABLE_BMSK                                            0x1000
#define HWIO_OVERRIDE_2_OVRID_APPS1_NIDEN_DISABLE_SHFT                                               0xc
#define HWIO_OVERRIDE_2_OVRID_APPS0_NIDEN_DISABLE_BMSK                                             0x800
#define HWIO_OVERRIDE_2_OVRID_APPS0_NIDEN_DISABLE_SHFT                                               0xb
#define HWIO_OVERRIDE_2_OVRID_A5X_ISDB_DBGEN_DISABLE_BMSK                                          0x400
#define HWIO_OVERRIDE_2_OVRID_A5X_ISDB_DBGEN_DISABLE_SHFT                                            0xa
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_BMSK                                           0x200
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_SHFT                                             0x9
#define HWIO_OVERRIDE_2_OVRID_SSC_Q6_DBGEN_DISABLE_BMSK                                            0x100
#define HWIO_OVERRIDE_2_OVRID_SSC_Q6_DBGEN_DISABLE_SHFT                                              0x8
#define HWIO_OVERRIDE_2_OVRID_SSC_DBGEN_DISABLE_BMSK                                                0x80
#define HWIO_OVERRIDE_2_OVRID_SSC_DBGEN_DISABLE_SHFT                                                 0x7
#define HWIO_OVERRIDE_2_OVRID_PIMEM_DBGEN_DISABLE_BMSK                                              0x40
#define HWIO_OVERRIDE_2_OVRID_PIMEM_DBGEN_DISABLE_SHFT                                               0x6
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_BMSK                                                0x20
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_SHFT                                                 0x5
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_BMSK                                               0x10
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_SHFT                                                0x4
#define HWIO_OVERRIDE_2_OVRID_LPASS_DBGEN_DISABLE_BMSK                                               0x8
#define HWIO_OVERRIDE_2_OVRID_LPASS_DBGEN_DISABLE_SHFT                                               0x3
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_BMSK                                                 0x4
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_SHFT                                                 0x2
#define HWIO_OVERRIDE_2_OVRID_APPS1_DBGEN_DISABLE_BMSK                                               0x2
#define HWIO_OVERRIDE_2_OVRID_APPS1_DBGEN_DISABLE_SHFT                                               0x1
#define HWIO_OVERRIDE_2_OVRID_APPS0_DBGEN_DISABLE_BMSK                                               0x1
#define HWIO_OVERRIDE_2_OVRID_APPS0_DBGEN_DISABLE_SHFT                                               0x0

#define HWIO_OVERRIDE_3_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060cc)
#define HWIO_OVERRIDE_3_RMSK                                                                  0xffffffff
#define HWIO_OVERRIDE_3_IN          \
        in_dword_masked(HWIO_OVERRIDE_3_ADDR, HWIO_OVERRIDE_3_RMSK)
#define HWIO_OVERRIDE_3_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_3_ADDR, m)
#define HWIO_OVERRIDE_3_OUT(v)      \
        out_dword(HWIO_OVERRIDE_3_ADDR,v)
#define HWIO_OVERRIDE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_3_ADDR,m,v,HWIO_OVERRIDE_3_IN)
#define HWIO_OVERRIDE_3_RSVD_31_11_BMSK                                                       0xfffff800
#define HWIO_OVERRIDE_3_RSVD_31_11_SHFT                                                              0xb
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_BMSK                                                0x400
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_SHFT                                                  0xa
#define HWIO_OVERRIDE_3_OVRID_SSC_SPIDEN_DISABLE_BMSK                                              0x200
#define HWIO_OVERRIDE_3_OVRID_SSC_SPIDEN_DISABLE_SHFT                                                0x9
#define HWIO_OVERRIDE_3_OVRID_PIMEM_SPIDEN_DISABLE_BMSK                                            0x100
#define HWIO_OVERRIDE_3_OVRID_PIMEM_SPIDEN_DISABLE_SHFT                                              0x8
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_BMSK                                               0x80
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_SHFT                                                0x7
#define HWIO_OVERRIDE_3_OVRID_APPS1_SPIDEN_DISABLE_BMSK                                             0x40
#define HWIO_OVERRIDE_3_OVRID_APPS1_SPIDEN_DISABLE_SHFT                                              0x6
#define HWIO_OVERRIDE_3_OVRID_APPS0_SPIDEN_DISABLE_BMSK                                             0x20
#define HWIO_OVERRIDE_3_OVRID_APPS0_SPIDEN_DISABLE_SHFT                                              0x5
#define HWIO_OVERRIDE_3_OVRID_SSC_SPNIDEN_DISABLE_BMSK                                              0x10
#define HWIO_OVERRIDE_3_OVRID_SSC_SPNIDEN_DISABLE_SHFT                                               0x4
#define HWIO_OVERRIDE_3_OVRID_PIMEM_SPNIDEN_DISABLE_BMSK                                             0x8
#define HWIO_OVERRIDE_3_OVRID_PIMEM_SPNIDEN_DISABLE_SHFT                                             0x3
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_BMSK                                               0x4
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_SHFT                                               0x2
#define HWIO_OVERRIDE_3_OVRID_APPS1_SPNIDEN_DISABLE_BMSK                                             0x2
#define HWIO_OVERRIDE_3_OVRID_APPS1_SPNIDEN_DISABLE_SHFT                                             0x1
#define HWIO_OVERRIDE_3_OVRID_APPS0_SPNIDEN_DISABLE_BMSK                                             0x1
#define HWIO_OVERRIDE_3_OVRID_APPS0_SPNIDEN_DISABLE_SHFT                                             0x0

#define HWIO_OVERRIDE_4_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d0)
#define HWIO_OVERRIDE_4_RMSK                                                                  0xffffffff
#define HWIO_OVERRIDE_4_IN          \
        in_dword_masked(HWIO_OVERRIDE_4_ADDR, HWIO_OVERRIDE_4_RMSK)
#define HWIO_OVERRIDE_4_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_4_ADDR, m)
#define HWIO_OVERRIDE_4_OUT(v)      \
        out_dword(HWIO_OVERRIDE_4_ADDR,v)
#define HWIO_OVERRIDE_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_4_ADDR,m,v,HWIO_OVERRIDE_4_IN)
#define HWIO_OVERRIDE_4_RSVD_31_6_BMSK                                                        0xffffffc0
#define HWIO_OVERRIDE_4_RSVD_31_6_SHFT                                                               0x6
#define HWIO_OVERRIDE_4_OVRID_GSS_A5_NIDEN_DISABLE_BMSK                                             0x20
#define HWIO_OVERRIDE_4_OVRID_GSS_A5_NIDEN_DISABLE_SHFT                                              0x5
#define HWIO_OVERRIDE_4_OVRID_GSS_A5_DBGEN_DISABLE_BMSK                                             0x10
#define HWIO_OVERRIDE_4_OVRID_GSS_A5_DBGEN_DISABLE_SHFT                                              0x4
#define HWIO_OVERRIDE_4_OVRID_GSS_A5_SPIDEN_DISABLE_BMSK                                             0x8
#define HWIO_OVERRIDE_4_OVRID_GSS_A5_SPIDEN_DISABLE_SHFT                                             0x3
#define HWIO_OVERRIDE_4_OVRID_GSS_A5_SPNIDEN_DISABLE_BMSK                                            0x4
#define HWIO_OVERRIDE_4_OVRID_GSS_A5_SPNIDEN_DISABLE_SHFT                                            0x2
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_BMSK                                                 0x2
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_SHFT                                                 0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_BMSK                                                 0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_SHFT                                                 0x0

#define HWIO_OVERRIDE_5_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d4)
#define HWIO_OVERRIDE_5_RMSK                                                                  0xffffffff
#define HWIO_OVERRIDE_5_IN          \
        in_dword_masked(HWIO_OVERRIDE_5_ADDR, HWIO_OVERRIDE_5_RMSK)
#define HWIO_OVERRIDE_5_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_5_ADDR, m)
#define HWIO_OVERRIDE_5_OUT(v)      \
        out_dword(HWIO_OVERRIDE_5_ADDR,v)
#define HWIO_OVERRIDE_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_5_ADDR,m,v,HWIO_OVERRIDE_5_IN)
#define HWIO_OVERRIDE_5_RSVD_31_0_BMSK                                                        0xffffffff
#define HWIO_OVERRIDE_5_RSVD_31_0_SHFT                                                               0x0

#define HWIO_OVERRIDE_6_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d8)
#define HWIO_OVERRIDE_6_RMSK                                                                  0xffffffff
#define HWIO_OVERRIDE_6_IN          \
        in_dword_masked(HWIO_OVERRIDE_6_ADDR, HWIO_OVERRIDE_6_RMSK)
#define HWIO_OVERRIDE_6_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_6_ADDR, m)
#define HWIO_OVERRIDE_6_OUT(v)      \
        out_dword(HWIO_OVERRIDE_6_ADDR,v)
#define HWIO_OVERRIDE_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_6_ADDR,m,v,HWIO_OVERRIDE_6_IN)
#define HWIO_OVERRIDE_6_RSVD_31_0_BMSK                                                        0xffffffff
#define HWIO_OVERRIDE_6_RSVD_31_0_SHFT                                                               0x0

#define HWIO_CAPT_SEC_GPIO_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060dc)
#define HWIO_CAPT_SEC_GPIO_RMSK                                                                  0x3ffff
#define HWIO_CAPT_SEC_GPIO_IN          \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, HWIO_CAPT_SEC_GPIO_RMSK)
#define HWIO_CAPT_SEC_GPIO_INM(m)      \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, m)
#define HWIO_CAPT_SEC_GPIO_OUT(v)      \
        out_dword(HWIO_CAPT_SEC_GPIO_ADDR,v)
#define HWIO_CAPT_SEC_GPIO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CAPT_SEC_GPIO_ADDR,m,v,HWIO_CAPT_SEC_GPIO_IN)
#define HWIO_CAPT_SEC_GPIO_SDC_EMMC_MODE1P2_EN_BMSK                                              0x20000
#define HWIO_CAPT_SEC_GPIO_SDC_EMMC_MODE1P2_EN_SHFT                                                 0x11
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_BMSK                                              0x10000
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_SHFT                                                 0x10
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_FORCE_MSA_AUTH_EN_BMSK                                0x8000
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_FORCE_MSA_AUTH_EN_SHFT                                   0xf
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_AUTH_EN_BMSK                                       0x4000
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_AUTH_EN_SHFT                                          0xe
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_PK_HASH_IN_FUSE_BMSK                               0x2000
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_PK_HASH_IN_FUSE_SHFT                                  0xd
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_MSA_AUTH_EN_BMSK                                      0x1000
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_MSA_AUTH_EN_SHFT                                         0xc
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_MSA_PK_HASH_IN_FUSE_BMSK                               0x800
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_MSA_PK_HASH_IN_FUSE_SHFT                                 0xb
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_ALL_USE_SERIAL_NUM_BMSK                                0x400
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_ALL_USE_SERIAL_NUM_SHFT                                  0xa
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_PK_HASH_INDEX_SRC_BMSK                                 0x200
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_PK_HASH_INDEX_SRC_SHFT                                   0x9
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_PBL_BOOT_SPEED_BMSK                               0x180
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_PBL_BOOT_SPEED_SHFT                                 0x7
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_RPM_BOOT_FROM_ROM_BMSK                                  0x40
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_RPM_BOOT_FROM_ROM_SHFT                                   0x6
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_BOOT_FROM_ROM_BMSK                                 0x20
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_BOOT_FROM_ROM_SHFT                                  0x5
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_FAST_BOOT_BMSK                                          0x1e
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_FAST_BOOT_SHFT                                           0x1
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_WDOG_DISABLE_BMSK                                        0x1
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_WDOG_DISABLE_SHFT                                        0x0

#define HWIO_APP_PROC_CFG_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e0)
#define HWIO_APP_PROC_CFG_RMSK                                                                    0x1fff
#define HWIO_APP_PROC_CFG_IN          \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, HWIO_APP_PROC_CFG_RMSK)
#define HWIO_APP_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, m)
#define HWIO_APP_PROC_CFG_OUT(v)      \
        out_dword(HWIO_APP_PROC_CFG_ADDR,v)
#define HWIO_APP_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APP_PROC_CFG_ADDR,m,v,HWIO_APP_PROC_CFG_IN)
#define HWIO_APP_PROC_CFG_SSC_DBG_SPNIDEN_BMSK                                                    0x1000
#define HWIO_APP_PROC_CFG_SSC_DBG_SPNIDEN_SHFT                                                       0xc
#define HWIO_APP_PROC_CFG_PIMEM_DBG_SPNIDEN_BMSK                                                   0x800
#define HWIO_APP_PROC_CFG_PIMEM_DBG_SPNIDEN_SHFT                                                     0xb
#define HWIO_APP_PROC_CFG_DAP_DBG_SPNIDEN_BMSK                                                     0x400
#define HWIO_APP_PROC_CFG_DAP_DBG_SPNIDEN_SHFT                                                       0xa
#define HWIO_APP_PROC_CFG_APPS_DBG_SPNIDEN_BMSK                                                    0x300
#define HWIO_APP_PROC_CFG_APPS_DBG_SPNIDEN_SHFT                                                      0x8
#define HWIO_APP_PROC_CFG_SSC_DBG_NIDEN_BMSK                                                        0x80
#define HWIO_APP_PROC_CFG_SSC_DBG_NIDEN_SHFT                                                         0x7
#define HWIO_APP_PROC_CFG_PIMEM_DBG_NIDEN_BMSK                                                      0x40
#define HWIO_APP_PROC_CFG_PIMEM_DBG_NIDEN_SHFT                                                       0x6
#define HWIO_APP_PROC_CFG_RPM_DBG_NIDEN_BMSK                                                        0x20
#define HWIO_APP_PROC_CFG_RPM_DBG_NIDEN_SHFT                                                         0x5
#define HWIO_APP_PROC_CFG_WCSS_DBG_NIDEN_BMSK                                                       0x10
#define HWIO_APP_PROC_CFG_WCSS_DBG_NIDEN_SHFT                                                        0x4
#define HWIO_APP_PROC_CFG_LPASS_DBG_NIDEN_BMSK                                                       0x8
#define HWIO_APP_PROC_CFG_LPASS_DBG_NIDEN_SHFT                                                       0x3
#define HWIO_APP_PROC_CFG_DAP_DBG_NIDEN_BMSK                                                         0x4
#define HWIO_APP_PROC_CFG_DAP_DBG_NIDEN_SHFT                                                         0x2
#define HWIO_APP_PROC_CFG_APPS_DBG_NIDEN_BMSK                                                        0x3
#define HWIO_APP_PROC_CFG_APPS_DBG_NIDEN_SHFT                                                        0x0

#define HWIO_MSS_PROC_CFG_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e4)
#define HWIO_MSS_PROC_CFG_RMSK                                                                       0x7
#define HWIO_MSS_PROC_CFG_IN          \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, HWIO_MSS_PROC_CFG_RMSK)
#define HWIO_MSS_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, m)
#define HWIO_MSS_PROC_CFG_OUT(v)      \
        out_dword(HWIO_MSS_PROC_CFG_ADDR,v)
#define HWIO_MSS_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_PROC_CFG_ADDR,m,v,HWIO_MSS_PROC_CFG_IN)
#define HWIO_MSS_PROC_CFG_GSS_A5_DBG_SPNIDEN_BMSK                                                    0x4
#define HWIO_MSS_PROC_CFG_GSS_A5_DBG_SPNIDEN_SHFT                                                    0x2
#define HWIO_MSS_PROC_CFG_GSS_A5_DBG_NIDEN_BMSK                                                      0x2
#define HWIO_MSS_PROC_CFG_GSS_A5_DBG_NIDEN_SHFT                                                      0x1
#define HWIO_MSS_PROC_CFG_MSS_DBG_NIDEN_BMSK                                                         0x1
#define HWIO_MSS_PROC_CFG_MSS_DBG_NIDEN_SHFT                                                         0x0

#define HWIO_QFPROM_CLK_CTL_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e8)
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

#define HWIO_HDCP_KSV_LSB_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060ec)
#define HWIO_HDCP_KSV_LSB_RMSK                                                                0xffffffff
#define HWIO_HDCP_KSV_LSB_IN          \
        in_dword_masked(HWIO_HDCP_KSV_LSB_ADDR, HWIO_HDCP_KSV_LSB_RMSK)
#define HWIO_HDCP_KSV_LSB_INM(m)      \
        in_dword_masked(HWIO_HDCP_KSV_LSB_ADDR, m)
#define HWIO_HDCP_KSV_LSB_KSV_LSB_BMSK                                                        0xffffffff
#define HWIO_HDCP_KSV_LSB_KSV_LSB_SHFT                                                               0x0

#define HWIO_HDCP_KSV_MSB_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060f0)
#define HWIO_HDCP_KSV_MSB_RMSK                                                                      0xff
#define HWIO_HDCP_KSV_MSB_IN          \
        in_dword_masked(HWIO_HDCP_KSV_MSB_ADDR, HWIO_HDCP_KSV_MSB_RMSK)
#define HWIO_HDCP_KSV_MSB_INM(m)      \
        in_dword_masked(HWIO_HDCP_KSV_MSB_ADDR, m)
#define HWIO_HDCP_KSV_MSB_KSV_MSB_BMSK                                                              0xff
#define HWIO_HDCP_KSV_MSB_KSV_MSB_SHFT                                                               0x0

#define HWIO_JTAG_ID_ADDR                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060f4)
#define HWIO_JTAG_ID_RMSK                                                                     0xffffffff
#define HWIO_JTAG_ID_IN          \
        in_dword_masked(HWIO_JTAG_ID_ADDR, HWIO_JTAG_ID_RMSK)
#define HWIO_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_JTAG_ID_ADDR, m)
#define HWIO_JTAG_ID_JTAG_ID_BMSK                                                             0xffffffff
#define HWIO_JTAG_ID_JTAG_ID_SHFT                                                                    0x0

#define HWIO_OEM_ID_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060f8)
#define HWIO_OEM_ID_RMSK                                                                      0xffffffff
#define HWIO_OEM_ID_IN          \
        in_dword_masked(HWIO_OEM_ID_ADDR, HWIO_OEM_ID_RMSK)
#define HWIO_OEM_ID_INM(m)      \
        in_dword_masked(HWIO_OEM_ID_ADDR, m)
#define HWIO_OEM_ID_OEM_ID_BMSK                                                               0xffff0000
#define HWIO_OEM_ID_OEM_ID_SHFT                                                                     0x10
#define HWIO_OEM_ID_OEM_PRODUCT_ID_BMSK                                                           0xffff
#define HWIO_OEM_ID_OEM_PRODUCT_ID_SHFT                                                              0x0

#define HWIO_TEST_BUS_SEL_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060fc)
#define HWIO_TEST_BUS_SEL_RMSK                                                                       0x7
#define HWIO_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, HWIO_TEST_BUS_SEL_RMSK)
#define HWIO_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, m)
#define HWIO_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_TEST_BUS_SEL_ADDR,v)
#define HWIO_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TEST_BUS_SEL_ADDR,m,v,HWIO_TEST_BUS_SEL_IN)
#define HWIO_TEST_BUS_SEL_TEST_EN_BMSK                                                               0x4
#define HWIO_TEST_BUS_SEL_TEST_EN_SHFT                                                               0x2
#define HWIO_TEST_BUS_SEL_TEST_SELECT_BMSK                                                           0x3
#define HWIO_TEST_BUS_SEL_TEST_SELECT_SHFT                                                           0x0

#define HWIO_SPDM_DYN_SECURE_MODE_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006100)
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

#define HWIO_QC_IMAGE_ENCR_KEYn_ADDR(n)                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006104 + 0x4 * (n))
#define HWIO_QC_IMAGE_ENCR_KEYn_RMSK                                                          0xffffffff
#define HWIO_QC_IMAGE_ENCR_KEYn_MAXn                                                                   3
#define HWIO_QC_IMAGE_ENCR_KEYn_INI(n)        \
        in_dword_masked(HWIO_QC_IMAGE_ENCR_KEYn_ADDR(n), HWIO_QC_IMAGE_ENCR_KEYn_RMSK)
#define HWIO_QC_IMAGE_ENCR_KEYn_INMI(n,mask)    \
        in_dword_masked(HWIO_QC_IMAGE_ENCR_KEYn_ADDR(n), mask)
#define HWIO_QC_IMAGE_ENCR_KEYn_KEY_DATA0_BMSK                                                0xffffffff
#define HWIO_QC_IMAGE_ENCR_KEYn_KEY_DATA0_SHFT                                                       0x0

#define HWIO_OEM_IMAGE_ENCR_KEYn_ADDR(n)                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006114 + 0x4 * (n))
#define HWIO_OEM_IMAGE_ENCR_KEYn_RMSK                                                         0xffffffff
#define HWIO_OEM_IMAGE_ENCR_KEYn_MAXn                                                                  3
#define HWIO_OEM_IMAGE_ENCR_KEYn_INI(n)        \
        in_dword_masked(HWIO_OEM_IMAGE_ENCR_KEYn_ADDR(n), HWIO_OEM_IMAGE_ENCR_KEYn_RMSK)
#define HWIO_OEM_IMAGE_ENCR_KEYn_INMI(n,mask)    \
        in_dword_masked(HWIO_OEM_IMAGE_ENCR_KEYn_ADDR(n), mask)
#define HWIO_OEM_IMAGE_ENCR_KEYn_KEY_DATA0_BMSK                                               0xffffffff
#define HWIO_OEM_IMAGE_ENCR_KEYn_KEY_DATA0_SHFT                                                      0x0

#define HWIO_IMAGE_ENCR_KEY1_0_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006124)
#define HWIO_IMAGE_ENCR_KEY1_0_RMSK                                                           0xffffffff
#define HWIO_IMAGE_ENCR_KEY1_0_IN          \
        in_dword_masked(HWIO_IMAGE_ENCR_KEY1_0_ADDR, HWIO_IMAGE_ENCR_KEY1_0_RMSK)
#define HWIO_IMAGE_ENCR_KEY1_0_INM(m)      \
        in_dword_masked(HWIO_IMAGE_ENCR_KEY1_0_ADDR, m)
#define HWIO_IMAGE_ENCR_KEY1_0_KEY_DATA0_BMSK                                                 0xffffffff
#define HWIO_IMAGE_ENCR_KEY1_0_KEY_DATA0_SHFT                                                        0x0

#define HWIO_IMAGE_ENCR_KEY1_1_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006128)
#define HWIO_IMAGE_ENCR_KEY1_1_RMSK                                                           0xffffffff
#define HWIO_IMAGE_ENCR_KEY1_1_IN          \
        in_dword_masked(HWIO_IMAGE_ENCR_KEY1_1_ADDR, HWIO_IMAGE_ENCR_KEY1_1_RMSK)
#define HWIO_IMAGE_ENCR_KEY1_1_INM(m)      \
        in_dword_masked(HWIO_IMAGE_ENCR_KEY1_1_ADDR, m)
#define HWIO_IMAGE_ENCR_KEY1_1_KEY_DATA0_BMSK                                                 0xffffffff
#define HWIO_IMAGE_ENCR_KEY1_1_KEY_DATA0_SHFT                                                        0x0

#define HWIO_IMAGE_ENCR_KEY1_2_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000612c)
#define HWIO_IMAGE_ENCR_KEY1_2_RMSK                                                           0xffffffff
#define HWIO_IMAGE_ENCR_KEY1_2_IN          \
        in_dword_masked(HWIO_IMAGE_ENCR_KEY1_2_ADDR, HWIO_IMAGE_ENCR_KEY1_2_RMSK)
#define HWIO_IMAGE_ENCR_KEY1_2_INM(m)      \
        in_dword_masked(HWIO_IMAGE_ENCR_KEY1_2_ADDR, m)
#define HWIO_IMAGE_ENCR_KEY1_2_KEY_DATA0_BMSK                                                 0xffffffff
#define HWIO_IMAGE_ENCR_KEY1_2_KEY_DATA0_SHFT                                                        0x0

#define HWIO_IMAGE_ENCR_KEY1_3_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006130)
#define HWIO_IMAGE_ENCR_KEY1_3_RMSK                                                           0xffffffff
#define HWIO_IMAGE_ENCR_KEY1_3_IN          \
        in_dword_masked(HWIO_IMAGE_ENCR_KEY1_3_ADDR, HWIO_IMAGE_ENCR_KEY1_3_RMSK)
#define HWIO_IMAGE_ENCR_KEY1_3_INM(m)      \
        in_dword_masked(HWIO_IMAGE_ENCR_KEY1_3_ADDR, m)
#define HWIO_IMAGE_ENCR_KEY1_3_KEY_DATA0_BMSK                                                 0xffffffff
#define HWIO_IMAGE_ENCR_KEY1_3_KEY_DATA0_SHFT                                                        0x0

#define HWIO_PK_HASH1_0_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006134)
#define HWIO_PK_HASH1_0_RMSK                                                                  0xffffffff
#define HWIO_PK_HASH1_0_IN          \
        in_dword_masked(HWIO_PK_HASH1_0_ADDR, HWIO_PK_HASH1_0_RMSK)
#define HWIO_PK_HASH1_0_INM(m)      \
        in_dword_masked(HWIO_PK_HASH1_0_ADDR, m)
#define HWIO_PK_HASH1_0_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_PK_HASH1_0_HASH_DATA0_SHFT                                                              0x0

#define HWIO_PK_HASH1_1_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006138)
#define HWIO_PK_HASH1_1_RMSK                                                                  0xffffffff
#define HWIO_PK_HASH1_1_IN          \
        in_dword_masked(HWIO_PK_HASH1_1_ADDR, HWIO_PK_HASH1_1_RMSK)
#define HWIO_PK_HASH1_1_INM(m)      \
        in_dword_masked(HWIO_PK_HASH1_1_ADDR, m)
#define HWIO_PK_HASH1_1_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_PK_HASH1_1_HASH_DATA0_SHFT                                                              0x0

#define HWIO_PK_HASH1_2_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000613c)
#define HWIO_PK_HASH1_2_RMSK                                                                  0xffffffff
#define HWIO_PK_HASH1_2_IN          \
        in_dword_masked(HWIO_PK_HASH1_2_ADDR, HWIO_PK_HASH1_2_RMSK)
#define HWIO_PK_HASH1_2_INM(m)      \
        in_dword_masked(HWIO_PK_HASH1_2_ADDR, m)
#define HWIO_PK_HASH1_2_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_PK_HASH1_2_HASH_DATA0_SHFT                                                              0x0

#define HWIO_PK_HASH1_3_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006140)
#define HWIO_PK_HASH1_3_RMSK                                                                  0xffffffff
#define HWIO_PK_HASH1_3_IN          \
        in_dword_masked(HWIO_PK_HASH1_3_ADDR, HWIO_PK_HASH1_3_RMSK)
#define HWIO_PK_HASH1_3_INM(m)      \
        in_dword_masked(HWIO_PK_HASH1_3_ADDR, m)
#define HWIO_PK_HASH1_3_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_PK_HASH1_3_HASH_DATA0_SHFT                                                              0x0

#define HWIO_PK_HASH1_4_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006144)
#define HWIO_PK_HASH1_4_RMSK                                                                  0xffffffff
#define HWIO_PK_HASH1_4_IN          \
        in_dword_masked(HWIO_PK_HASH1_4_ADDR, HWIO_PK_HASH1_4_RMSK)
#define HWIO_PK_HASH1_4_INM(m)      \
        in_dword_masked(HWIO_PK_HASH1_4_ADDR, m)
#define HWIO_PK_HASH1_4_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_PK_HASH1_4_HASH_DATA0_SHFT                                                              0x0

#define HWIO_PK_HASH1_5_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006148)
#define HWIO_PK_HASH1_5_RMSK                                                                  0xffffffff
#define HWIO_PK_HASH1_5_IN          \
        in_dword_masked(HWIO_PK_HASH1_5_ADDR, HWIO_PK_HASH1_5_RMSK)
#define HWIO_PK_HASH1_5_INM(m)      \
        in_dword_masked(HWIO_PK_HASH1_5_ADDR, m)
#define HWIO_PK_HASH1_5_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_PK_HASH1_5_HASH_DATA0_SHFT                                                              0x0

#define HWIO_PK_HASH1_6_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000614c)
#define HWIO_PK_HASH1_6_RMSK                                                                  0xffffffff
#define HWIO_PK_HASH1_6_IN          \
        in_dword_masked(HWIO_PK_HASH1_6_ADDR, HWIO_PK_HASH1_6_RMSK)
#define HWIO_PK_HASH1_6_INM(m)      \
        in_dword_masked(HWIO_PK_HASH1_6_ADDR, m)
#define HWIO_PK_HASH1_6_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_PK_HASH1_6_HASH_DATA0_SHFT                                                              0x0

#define HWIO_PK_HASH1_7_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006150)
#define HWIO_PK_HASH1_7_RMSK                                                                  0xffffffff
#define HWIO_PK_HASH1_7_IN          \
        in_dword_masked(HWIO_PK_HASH1_7_ADDR, HWIO_PK_HASH1_7_RMSK)
#define HWIO_PK_HASH1_7_INM(m)      \
        in_dword_masked(HWIO_PK_HASH1_7_ADDR, m)
#define HWIO_PK_HASH1_7_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_PK_HASH1_7_HASH_DATA0_SHFT                                                              0x0

#define HWIO_SW_ROT_STICKY_BIT_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006154)
#define HWIO_SW_ROT_STICKY_BIT_RMSK                                                                  0x1
#define HWIO_SW_ROT_STICKY_BIT_IN          \
        in_dword_masked(HWIO_SW_ROT_STICKY_BIT_ADDR, HWIO_SW_ROT_STICKY_BIT_RMSK)
#define HWIO_SW_ROT_STICKY_BIT_INM(m)      \
        in_dword_masked(HWIO_SW_ROT_STICKY_BIT_ADDR, m)
#define HWIO_SW_ROT_STICKY_BIT_OUT(v)      \
        out_dword(HWIO_SW_ROT_STICKY_BIT_ADDR,v)
#define HWIO_SW_ROT_STICKY_BIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SW_ROT_STICKY_BIT_ADDR,m,v,HWIO_SW_ROT_STICKY_BIT_IN)
#define HWIO_SW_ROT_STICKY_BIT_SW_ROT_STICKY_BIT_0_BMSK                                              0x1
#define HWIO_SW_ROT_STICKY_BIT_SW_ROT_STICKY_BIT_0_SHFT                                              0x0

#define HWIO_SW_ROT_CONFIG_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006158)
#define HWIO_SW_ROT_CONFIG_RMSK                                                                      0x3
#define HWIO_SW_ROT_CONFIG_IN          \
        in_dword_masked(HWIO_SW_ROT_CONFIG_ADDR, HWIO_SW_ROT_CONFIG_RMSK)
#define HWIO_SW_ROT_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SW_ROT_CONFIG_ADDR, m)
#define HWIO_SW_ROT_CONFIG_CURRENT_SW_ROT_MODEM_BMSK                                                 0x2
#define HWIO_SW_ROT_CONFIG_CURRENT_SW_ROT_MODEM_SHFT                                                 0x1
#define HWIO_SW_ROT_CONFIG_CURRENT_SW_ROT_APPS_BMSK                                                  0x1
#define HWIO_SW_ROT_CONFIG_CURRENT_SW_ROT_APPS_SHFT                                                  0x0


#endif /* __CPR_QFPROM_H__ */
