#ifndef __UIEHWIOREG_H__
#define __UIEHWIOREG_H__
/*
===========================================================================
*/
/**
  @file uiehwioreg.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8956_76 (Eldarion) [eldarion_v1.0_p3q3r41_MTO]
 
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

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/uie/chipset/msm8937/src/uiehwioreg.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

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

#define HWIO_QFPROM_RAW_JTAG_ID_ADDR                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000120)
#define HWIO_QFPROM_RAW_JTAG_ID_RMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_JTAG_ID_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_ADDR, HWIO_QFPROM_RAW_JTAG_ID_RMSK)
#define HWIO_QFPROM_RAW_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_ADDR, m)
#define HWIO_QFPROM_RAW_JTAG_ID_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_JTAG_ID_ADDR,v)
#define HWIO_QFPROM_RAW_JTAG_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_JTAG_ID_ADDR,m,v,HWIO_QFPROM_RAW_JTAG_ID_IN)
#define HWIO_QFPROM_RAW_JTAG_ID_SPARE0_BMSK                                                   0xe0000000
#define HWIO_QFPROM_RAW_JTAG_ID_SPARE0_SHFT                                                         0x1d
#define HWIO_QFPROM_RAW_JTAG_ID_MACCHIATO_EN_BMSK                                             0x10000000
#define HWIO_QFPROM_RAW_JTAG_ID_MACCHIATO_EN_SHFT                                                   0x1c
#define HWIO_QFPROM_RAW_JTAG_ID_FEATURE_ID_BMSK                                                0xff00000
#define HWIO_QFPROM_RAW_JTAG_ID_FEATURE_ID_SHFT                                                     0x14
#define HWIO_QFPROM_RAW_JTAG_ID_JTAG_ID_BMSK                                                     0xfffff
#define HWIO_QFPROM_RAW_JTAG_ID_JTAG_ID_SHFT                                                         0x0

#define HWIO_QFPROM_RAW_PTE1_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000124)
#define HWIO_QFPROM_RAW_PTE1_RMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_PTE1_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE1_ADDR, HWIO_QFPROM_RAW_PTE1_RMSK)
#define HWIO_QFPROM_RAW_PTE1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE1_ADDR, m)
#define HWIO_QFPROM_RAW_PTE1_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE1_ADDR,v)
#define HWIO_QFPROM_RAW_PTE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE1_ADDR,m,v,HWIO_QFPROM_RAW_PTE1_IN)
#define HWIO_QFPROM_RAW_PTE1_IDDQ_MSS_ACTIVE_MEAS_FUSE_BITS_BMSK                              0xfc000000
#define HWIO_QFPROM_RAW_PTE1_IDDQ_MSS_ACTIVE_MEAS_FUSE_BITS_SHFT                                    0x1a
#define HWIO_QFPROM_RAW_PTE1_PROCESS_NODE_ID_BMSK                                              0x2000000
#define HWIO_QFPROM_RAW_PTE1_PROCESS_NODE_ID_SHFT                                                   0x19
#define HWIO_QFPROM_RAW_PTE1_SPARE2_BMSK                                                       0x1800000
#define HWIO_QFPROM_RAW_PTE1_SPARE2_SHFT                                                            0x17
#define HWIO_QFPROM_RAW_PTE1_BONE_PILE_BMSK                                                     0x400000
#define HWIO_QFPROM_RAW_PTE1_BONE_PILE_SHFT                                                         0x16
#define HWIO_QFPROM_RAW_PTE1_A53_ACC_SETTINGS_ID_BMSK                                           0x300000
#define HWIO_QFPROM_RAW_PTE1_A53_ACC_SETTINGS_ID_SHFT                                               0x14
#define HWIO_QFPROM_RAW_PTE1_SPARE1_BMSK                                                         0xfe000
#define HWIO_QFPROM_RAW_PTE1_SPARE1_SHFT                                                             0xd
#define HWIO_QFPROM_RAW_PTE1_WAFER_BIN_BMSK                                                       0x1c00
#define HWIO_QFPROM_RAW_PTE1_WAFER_BIN_SHFT                                                          0xa
#define HWIO_QFPROM_RAW_PTE1_METAL_REV_BMSK                                                        0x300
#define HWIO_QFPROM_RAW_PTE1_METAL_REV_SHFT                                                          0x8
#define HWIO_QFPROM_RAW_PTE1_SPARE0_BMSK                                                            0xff
#define HWIO_QFPROM_RAW_PTE1_SPARE0_SHFT                                                             0x0

#define HWIO_QFPROM_RAW_SERIAL_NUM_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000128)
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

#define HWIO_QFPROM_RAW_PTE2_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000012c)
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

#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000130)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RSVD0_BMSK                                             0xf8000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RSVD0_SHFT                                                   0x1b
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE26_BMSK                                            0x4000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE26_SHFT                                                 0x1a
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE25_BMSK                                            0x2000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE25_SHFT                                                 0x19
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE24_BMSK                                            0x1000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE24_SHFT                                                 0x18
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE23_BMSK                                             0x800000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE23_SHFT                                                 0x17
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE22_BMSK                                             0x400000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE22_SHFT                                                 0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE21_BMSK                                             0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE21_SHFT                                                 0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE20_BMSK                                             0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE20_SHFT                                                 0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_BMSK                                              0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE18_BMSK                                              0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE18_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_BMSK                                               0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_SHFT                                                  0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                       0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                          0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_CONFIG_BMSK                                            0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_CONFIG_SHFT                                               0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_BMSK                                                 0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_SHFT                                                    0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PK_HASH_BMSK                                               0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PK_HASH_SHFT                                                  0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB12_BMSK                                               0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB12_SHFT                                                  0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                           0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                             0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                 0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                   0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                 0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                   0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CM_FEAT_CONFIG_BMSK                                         0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CM_FEAT_CONFIG_SHFT                                           0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                             0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                              0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                              0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                               0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                         0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                          0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                         0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                          0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                          0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                          0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                               0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                               0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_BMSK                                                      0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_SHFT                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CRI_CM_PRIVATE_BMSK                                           0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CRI_CM_PRIVATE_SHFT                                           0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000134)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RSVD0_BMSK                                             0xf8000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RSVD0_SHFT                                                   0x1b
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE26_BMSK                                            0x4000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE26_SHFT                                                 0x1a
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE25_BMSK                                            0x2000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE25_SHFT                                                 0x19
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE24_BMSK                                            0x1000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE24_SHFT                                                 0x18
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE23_BMSK                                             0x800000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE23_SHFT                                                 0x17
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE22_BMSK                                             0x400000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE22_SHFT                                                 0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE21_BMSK                                             0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE21_SHFT                                                 0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE20_BMSK                                             0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE20_SHFT                                                 0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_BMSK                                              0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE18_BMSK                                              0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE18_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_BMSK                                               0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_SHFT                                                  0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                       0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                          0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_CONFIG_BMSK                                            0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_CONFIG_SHFT                                               0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_BMSK                                                 0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_SHFT                                                    0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PK_HASH_BMSK                                               0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PK_HASH_SHFT                                                  0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB12_BMSK                                               0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB12_SHFT                                                  0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                           0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                             0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                 0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                   0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                 0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                   0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CM_FEAT_CONFIG_BMSK                                         0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CM_FEAT_CONFIG_SHFT                                           0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                             0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                              0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                              0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                               0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_BMSK                                         0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_SHFT                                          0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_BMSK                                         0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_SHFT                                          0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_BMSK                                          0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_SHFT                                          0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                               0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                               0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_BMSK                                                      0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_SHFT                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CRI_CM_PRIVATE_BMSK                                           0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CRI_CM_PRIVATE_SHFT                                           0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000138)
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

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000013c)
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

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000140)
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

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000144)
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

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000148)
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

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000014c)
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

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000150)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD0_BMSK                                        0xc0000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD0_SHFT                                              0x1e
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_BMSK                      0x20000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_SHFT                            0x1d
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_DISABLE_POLICY_BMSK                         0x10000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_DISABLE_POLICY_SHFT                               0x1c
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG26_SECURE_BMSK                            0x8000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG26_SECURE_SHFT                                 0x1b
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG25_SECURE_BMSK                            0x4000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG25_SECURE_SHFT                                 0x1a
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG24_SECURE_BMSK                            0x2000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG24_SECURE_SHFT                                 0x19
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG23_SECURE_BMSK                            0x1000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG23_SECURE_SHFT                                 0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG22_SECURE_BMSK                             0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG22_SECURE_SHFT                                 0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                                0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                    0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                            0x3c0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                                0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_BMSK                              0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_SHFT                                 0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_BMSK                              0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_SHFT                                 0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_BMSK                                           0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_SHFT                                              0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_BMSK                               0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_SHFT                                  0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_BMSK                               0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_SHFT                                  0xd
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

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000154)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                       0xf8000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                             0x1b
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SDCC5_SCM_FORCE_EFUSE_KEY_BMSK                     0x4000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SDCC5_SCM_FORCE_EFUSE_KEY_SHFT                          0x1a
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_A5X_ISDB_DBGEN_DISABLE_BMSK                        0x2000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_A5X_ISDB_DBGEN_DISABLE_SHFT                             0x19
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_LPASS_NIDEN_DISABLE_BMSK                           0x1000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_LPASS_NIDEN_DISABLE_SHFT                                0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_LPASS_DBGEN_DISABLE_BMSK                            0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_LPASS_DBGEN_DISABLE_SHFT                                0x17
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

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000158)
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

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000015c)
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

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000160)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RESOLUTION_LIMITER_0_BMSK                        0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RESOLUTION_LIMITER_0_SHFT                              0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                               0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                     0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RESOLUTION_LIMITER_1_BMSK                        0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RESOLUTION_LIMITER_1_SHFT                              0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MDP_EFUSE_LTC0_DISABLE_BMSK                      0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MDP_EFUSE_LTC0_DISABLE_SHFT                            0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_BMSK                                0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_SHFT                                     0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_HEVC_BMSK                           0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_HEVC_SHFT                                0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SECURE_CHANNEL_DISABLE_BMSK                       0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SECURE_CHANNEL_DISABLE_SHFT                            0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                          0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                               0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                           0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                               0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_22_BMSK                                      0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_22_SHFT                                          0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_BMSK                                   0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_SHFT                                       0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_20_BMSK                                      0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_20_SHFT                                          0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_BMSK                            0xe0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_SHFT                               0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM2_DISABLE_BMSK                             0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM2_DISABLE_SHFT                                0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM1_DISABLE_BMSK                              0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM1_DISABLE_SHFT                                 0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SMMU_TBU_BYPASS_DISABLE_BMSK                         0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SMMU_TBU_BYPASS_DISABLE_SHFT                            0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DSDA_DISABLE_BMSK                              0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DSDA_DISABLE_SHFT                                 0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_BMSK                             0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_SHFT                                0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_BMSK                              0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_SHFT                                0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_BMSK                             0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_SHFT                               0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_9_BMSK                                          0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_9_SHFT                                            0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_8_BMSK                                          0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_8_SHFT                                            0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_7_BMSK                                           0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_7_SHFT                                            0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_BMSK                      0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_SHFT                       0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_BMSK                             0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_SHFT                              0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_BMSK                                0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_SHFT                                 0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_BMSK                                  0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_SHFT                                  0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_BMSK                                0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_SHFT                                0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_BMSK                                   0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_SHFT                                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_BMSK                                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_SHFT                                   0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000164)
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
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_BMSK                      0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_SHFT                            0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_GFX_FREQ_LIMIT_FUSE_BMSK                         0x30000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_GFX_FREQ_LIMIT_FUSE_SHFT                               0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SYSBARDISABLE_BMSK                                0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SYSBARDISABLE_SHFT                                     0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE6_BMSK                                       0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE6_SHFT                                            0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_DDR_FREQ_LIMIT_EN_BMSK                            0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_DDR_FREQ_LIMIT_EN_SHFT                                 0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_BMSK                                  0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_SHFT                                       0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_BMSK                       0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_SHFT                           0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_BMSK                       0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_SHFT                           0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_BMSK                             0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_SHFT                                 0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE5_BMSK                                        0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE5_SHFT                                            0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_SCM_DISABLE_BMSK                              0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_SCM_DISABLE_SHFT                                 0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_CCI_FREQ_SCALE_BMSK                                 0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_CCI_FREQ_SCALE_SHFT                                    0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_BMSK                                  0x38000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_SHFT                                      0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_DCC_DISABLE_BMSK                                     0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_DCC_DISABLE_SHFT                                        0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_LTE_UL_CA_DISABLE_BMSK                          0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_LTE_UL_CA_DISABLE_SHFT                             0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_LTE_ABOVE_CAT4_DISABLE_BMSK                     0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_LTE_ABOVE_CAT4_DISABLE_SHFT                        0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_HSUPA_DC_DISABLE_BMSK                      0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_HSUPA_DC_DISABLE_SHFT                        0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_VP9D_BMSK                               0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_VP9D_SHFT                                 0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE2_BMSK                                           0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE2_SHFT                                             0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_265E_BMSK                               0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_265E_SHFT                                 0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_4KX2KD_BMSK                              0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_4KX2KD_SHFT                               0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_CSID_2_DISABLE_BMSK                                    0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_CSID_2_DISABLE_SHFT                                     0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BMSK                                 0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_SHFT                                  0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_BMSK                               0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_SHFT                                0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE1_BMSK                                             0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE1_SHFT                                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_L2_CACHE_UPPER_BANK_DISABLE_BMSK                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_L2_CACHE_UPPER_BANK_DISABLE_SHFT                   0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000168)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                      0xfc000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                            0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_CAMCC_VFE_1_EFUSE_LIM_RES_8MP_BMSK                0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_CAMCC_VFE_1_EFUSE_LIM_RES_8MP_SHFT                     0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_CAMCC_VFE_0_EFUSE_LIM_RES_8MP_BMSK                0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_CAMCC_VFE_0_EFUSE_LIM_RES_8MP_SHFT                     0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_A5X_ISDB_DBGEN_DISABLE_BMSK                     0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_A5X_ISDB_DBGEN_DISABLE_SHFT                         0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_LPASS_NIDEN_DISABLE_BMSK                        0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_LPASS_NIDEN_DISABLE_SHFT                            0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_BMSK                        0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_SHFT                            0x15
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

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000016c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                      0xfe000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                            0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                     0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                          0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                             0xfff800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                  0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SPARE1_BMSK                                           0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SPARE1_SHFT                                             0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_BMSK                          0x3e0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_SHFT                            0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                            0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                             0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000170)
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

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000174)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_BMSK                      0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_SHFT                            0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_BMSK                      0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_SHFT                            0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_BMSK                      0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_SHFT                            0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_4_BMSK                      0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_4_SHFT                            0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_GCC_DISABLE_BOOT_FSM_BMSK                         0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_GCC_DISABLE_BOOT_FSM_SHFT                              0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_BMSK                           0x7800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_SHFT                                0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                             0x780000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                 0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_3_BMSK                         0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_3_SHFT                            0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_2_BMSK                         0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_2_SHFT                            0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_1_BMSK                         0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_1_SHFT                            0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_CSR_BMSK                              0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_CSR_SHFT                                 0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_BMSK                             0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_SHFT                                0xe
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

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ADDR                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000178)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RSVD0_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RSVD0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_ADDR                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000017c)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD0_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_ADDR                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000180)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_RSVD0_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_RSVD0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_ADDR                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000184)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_RSVD0_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_RSVD0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_ADDR                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000188)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_RSVD0_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_RSVD0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_ADDR                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000018c)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000190 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000194 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b0 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b4 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d0)
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

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d4)
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

#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d8)
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_SPARE0_BMSK                                          0xc0000000
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_SPARE0_SHFT                                                0x1e
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_CPR6_AGING_SENSOR_0_BMSK                             0x3f000000
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_CPR6_AGING_SENSOR_0_SHFT                                   0x18
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_CPR5_AGING_SENSOR_0_BMSK                               0xfc0000
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_CPR5_AGING_SENSOR_0_SHFT                                   0x12
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_CPR2_AGING_SENSOR_0_BMSK                                0x3f000
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_CPR2_AGING_SENSOR_0_SHFT                                    0xc
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_CPR1_AGING_SENSOR_0_BMSK                                  0xfc0
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_CPR1_AGING_SENSOR_0_SHFT                                    0x6
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_CPR0_AGING_SENSOR_0_BMSK                                   0x3f
#define HWIO_QFPROM_RAW_CALIB12_ROW0_LSB_CPR0_AGING_SENSOR_0_SHFT                                    0x0

#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001dc)
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_SPARE0_BMSK                                          0x80000000
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_SPARE0_SHFT                                                0x1f
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_FEC_VALUE_BMSK                                       0x7f000000
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_FEC_VALUE_SHFT                                             0x18
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_CPR_LOCAL_RC_BMSK                                      0xe00000
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_CPR_LOCAL_RC_SHFT                                          0x15
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_CPR3_NOM_TARGET_BMSK                                   0x1f0000
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_CPR3_NOM_TARGET_SHFT                                       0x10
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_CPR3_TURBO_TARGET_BMSK                                   0xf800
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_CPR3_TURBO_TARGET_SHFT                                      0xb
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_CPR1_SVS2_TARGET_BMSK                                     0x7c0
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_CPR1_SVS2_TARGET_SHFT                                       0x6
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_CPR3_SVS_TARGET_BMSK                                       0x3e
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_CPR3_SVS_TARGET_SHFT                                        0x1
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_SPARE1_BMSK                                                 0x1
#define HWIO_QFPROM_RAW_CALIB12_ROW0_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001e0 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001e4 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000200)
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

#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000204)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_RMSK                                                 0xffffffff
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
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_UNUSED_BMSK                                            0xffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_UNUSED_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000208)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CS_SENSOR_PARAM_CORE_1_8_0_BMSK                        0xff800000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CS_SENSOR_PARAM_CORE_1_8_0_SHFT                              0x17
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CS_SENSOR_PARAM_CORE_0_BMSK                              0x7fe000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CS_SENSOR_PARAM_CORE_0_SHFT                                   0xd
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CSI_PHY_BMSK                                               0x1f00
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CSI_PHY_SHFT                                                  0x8
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSI_PHY_BMSK                                                 0xf0
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSI_PHY_SHFT                                                  0x4
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSIPHY_PLL_BMSK                                               0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSIPHY_PLL_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000020c)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_2_0_BMSK                0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_2_0_SHFT                    0x15
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_3_BMSK                              0x1ff800
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_3_SHFT                                   0xb
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_2_BMSK                                 0x7fe
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_2_SHFT                                   0x1
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_1_9_BMSK                                 0x1
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_1_9_SHFT                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000210)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_2_INTERCEPT_6_0_BMSK              0xfe000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_2_INTERCEPT_6_0_SHFT                    0x19
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_1_INTERCEPT_BMSK                   0x1ff8000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_1_INTERCEPT_SHFT                         0xf
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_9_4_BMSK                  0x7e00
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_9_4_SHFT                     0x9
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IDDQ_A53_ACTIVE_MEAS_BMSK                                   0x1fe
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IDDQ_A53_ACTIVE_MEAS_SHFT                                     0x1
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_3_BMSK                       0x1
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_3_SHFT                       0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000214)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SPARE1_BMSK                                              0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SPARE1_SHFT                                                  0x16
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_APC1_LDO_VREF_TRIM_BMSK                                  0x3e0000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_APC1_LDO_VREF_TRIM_SHFT                                      0x11
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_AMP_COMP_BMSK                                             0x1e000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_AMP_COMP_SHFT                                                 0xd
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CS_SENSOR_PARAM_CORE_3_INTERCEPT_BMSK                      0x1ff8
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CS_SENSOR_PARAM_CORE_3_INTERCEPT_SHFT                         0x3
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CS_SENSOR_PARAM_CORE_2_INTERCEPT_9_7_BMSK                     0x7
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CS_SENSOR_PARAM_CORE_2_INTERCEPT_9_7_SHFT                     0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000218)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS1_POINT2_BMSK                                     0xfc000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS1_POINT2_SHFT                                           0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS1_POINT1_BMSK                                      0x3f00000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS1_POINT1_SHFT                                           0x14
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS0_POINT2_BMSK                                        0xfc000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS0_POINT2_SHFT                                            0xe
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS0_POINT1_BMSK                                         0x3f00
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS0_POINT1_SHFT                                            0x8
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS_BASE0_BMSK                                             0xff
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS_BASE0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000021c)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS3_POINT2_BMSK                                       0xfc0000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS3_POINT2_SHFT                                           0x12
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS3_POINT1_BMSK                                        0x3f000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS3_POINT1_SHFT                                            0xc
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS2_POINT2_BMSK                                          0xfc0
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS2_POINT2_SHFT                                            0x6
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS2_POINT1_BMSK                                           0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS2_POINT1_SHFT                                            0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000220)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS5_POINT2_BMSK                                     0xfc000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS5_POINT2_SHFT                                           0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS5_POINT1_BMSK                                      0x3f00000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS5_POINT1_SHFT                                           0x14
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS4_POINT2_BMSK                                        0xfc000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS4_POINT2_SHFT                                            0xe
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS4_POINT1_BMSK                                         0x3f00
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS4_POINT1_SHFT                                            0x8
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS_BASE1_BMSK                                             0xff
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS_BASE1_SHFT                                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000224)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_TSENS7_POINT2_BMSK                                       0xfc0000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_TSENS7_POINT2_SHFT                                           0x12
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_TSENS7_POINT1_BMSK                                        0x3f000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_TSENS7_POINT1_SHFT                                            0xc
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_TSENS6_POINT2_BMSK                                          0xfc0
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_TSENS6_POINT2_SHFT                                            0x6
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_TSENS6_POINT1_BMSK                                           0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_TSENS6_POINT1_SHFT                                            0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000228)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS10_POINT1_4_0_BMSK                                0xf8000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS10_POINT1_4_0_SHFT                                      0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS9_POINT2_BMSK                                      0x7e00000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS9_POINT2_SHFT                                           0x15
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS9_POINT1_BMSK                                       0x1f8000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS9_POINT1_SHFT                                            0xf
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS8_POINT2_BMSK                                         0x7e00
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS8_POINT2_SHFT                                            0x9
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS8_POINT1_BMSK                                          0x1f8
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS8_POINT1_SHFT                                            0x3
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS_CALIB_BMSK                                              0x7
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_TSENS_CALIB_SHFT                                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000022c)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TXDAC0_SPARE_6_0_BMSK                                    0xfe0000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TXDAC0_SPARE_6_0_SHFT                                        0x11
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TXDAC0_RANGE_CORR_BMSK                                    0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TXDAC0_RANGE_CORR_SHFT                                       0x10
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TXDAC0_AVEG_CORR_BMSK                                      0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TXDAC0_AVEG_CORR_SHFT                                         0xf
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TXDAC0_RPOLY_CAL_BMSK                                      0x7f80
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TXDAC0_RPOLY_CAL_SHFT                                         0x7
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TSENS10_POINT2_BMSK                                          0x7e
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TSENS10_POINT2_SHFT                                           0x1
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TSENS10_POINT1_5_BMSK                                         0x1
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_TSENS10_POINT1_5_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000230)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B0M3_1_0_BMSK                                       0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B0M3_1_0_SHFT                                             0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B0M2_BMSK                                           0x38000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B0M2_SHFT                                                 0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B0M1_BMSK                                            0x7000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B0M1_SHFT                                                 0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B0M0_BMSK                                             0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B0M0_SHFT                                                 0x15
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_G_B0_BMSK                                                0x1c0000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_G_B0_SHFT                                                    0x12
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CLK_B_BMSK                                                0x30000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CLK_B_SHFT                                                   0x10
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CAP_B_BMSK                                                 0xc000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CAP_B_SHFT                                                    0xe
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_SAR_B_BMSK                                                 0x3000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_SAR_B_SHFT                                                    0xc
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_MODEM_TXDAC_FUSEFLAG_BMSK                                   0x800
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_MODEM_TXDAC_FUSEFLAG_SHFT                                     0xb
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_TXDAC1_RANGE_CORR_BMSK                                      0x400
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_TXDAC1_RANGE_CORR_SHFT                                        0xa
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_TXDAC1_AVEG_CORR_BMSK                                       0x200
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_TXDAC1_AVEG_CORR_SHFT                                         0x9
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_TXDAC1_RPOLY_CAL_BMSK                                       0x1fe
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_TXDAC1_RPOLY_CAL_SHFT                                         0x1
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_TXDAC0_SPARE_7_BMSK                                           0x1
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_TXDAC0_SPARE_7_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000234)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B2M0_0_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B2M0_0_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_G_B2_BMSK                                                0x700000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_G_B2_SHFT                                                    0x14
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_VREF_B1_BMSK                                              0xc0000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_VREF_B1_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B1M3_BMSK                                              0x38000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B1M3_SHFT                                                  0xf
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B1M2_BMSK                                               0x7000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B1M2_SHFT                                                  0xc
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B1M1_BMSK                                                0xe00
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B1M1_SHFT                                                  0x9
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B1M0_BMSK                                                0x1c0
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B1M0_SHFT                                                  0x6
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_G_B1_BMSK                                                    0x38
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_G_B1_SHFT                                                     0x3
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_VREF_B0_BMSK                                                  0x6
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_VREF_B0_SHFT                                                  0x1
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B0M3_2_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B0M3_2_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000238)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_GNSS_ADC_CALIB_1_0_BMSK                                0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_GNSS_ADC_CALIB_1_0_SHFT                                      0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_B3_BMSK                                           0x30000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_B3_SHFT                                                 0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B3M3_BMSK                                            0xe000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B3M3_SHFT                                                 0x19
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B3M2_BMSK                                            0x1c00000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B3M2_SHFT                                                 0x16
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B3M1_BMSK                                             0x380000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B3M1_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B3M0_BMSK                                              0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B3M0_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_G_B3_BMSK                                                  0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_G_B3_SHFT                                                     0xd
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_B2_BMSK                                               0x1800
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_B2_SHFT                                                  0xb
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B2M3_BMSK                                                0x700
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B2M3_SHFT                                                  0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B2M2_BMSK                                                 0xe0
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B2M2_SHFT                                                  0x5
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B2M1_BMSK                                                 0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B2M1_SHFT                                                  0x2
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B2M0_2_1_BMSK                                              0x3
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B2M0_2_1_SHFT                                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000023c)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_SPARE1_BMSK                                              0xf00000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_SPARE1_SHFT                                                  0x14
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_VOLTAGE_SENSOR_CALIB_BMSK                                 0xfffc0
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_VOLTAGE_SENSOR_CALIB_SHFT                                     0x6
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_GNSS_ADC_CALIB_7_2_BMSK                                      0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_GNSS_ADC_CALIB_7_2_SHFT                                       0x0

#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000240)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW7_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_SPARE0_BMSK                                            0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_SPARE0_SHFT                                                  0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR1_SVS_TARGET_BMSK                                   0x3e000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR1_SVS_TARGET_SHFT                                         0x19
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR1_NOMINAL_TARGET_BMSK                                0x1f00000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR1_NOMINAL_TARGET_SHFT                                     0x14
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR1_TURBO_TARGET_BMSK                                    0xf8000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR1_TURBO_TARGET_SHFT                                        0xf
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR0_SVS_TARGET_BMSK                                       0x7c00
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR0_SVS_TARGET_SHFT                                          0xa
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR0_NOMINAL_TARGET_BMSK                                    0x3e0
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR0_NOMINAL_TARGET_SHFT                                      0x5
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR0_TURBO_TARGET_BMSK                                       0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR0_TURBO_TARGET_SHFT                                        0x0

#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000244)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW7_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_SPARE2_BMSK                                              0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_SPARE2_SHFT                                                  0x17
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR2_SVS2_TARGET_BMSK                                    0x7c0000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR2_SVS2_TARGET_SHFT                                        0x12
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR2_SVS_TARGET_BMSK                                      0x3e000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR2_SVS_TARGET_SHFT                                          0xd
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR2_NOMINAL_TARGET_BMSK                                   0x1f00
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR2_NOMINAL_TARGET_SHFT                                      0x8
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR2_TURBO_TARGET_BMSK                                       0xf8
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR2_TURBO_TARGET_SHFT                                        0x3
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_SPARE1_BMSK                                                   0x7
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_SPARE1_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000248)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW8_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW8_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_SVS_TARGET_BMSK                                   0xfc000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_SVS_TARGET_SHFT                                         0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_NOMINAL_TARGET_BMSK                                0x3f00000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_NOMINAL_TARGET_SHFT                                     0x14
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_TURBO_TARGET_BMSK                                    0xfc000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_TURBO_TARGET_SHFT                                        0xe
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_SVS_ROSEL_BMSK                                        0x3800
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_SVS_ROSEL_SHFT                                           0xb
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_NOM_ROSEL_BMSK                                         0x700
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_NOM_ROSEL_SHFT                                           0x8
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_TUR_ROSEL_BMSK                                          0xe0
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_TUR_ROSEL_SHFT                                           0x5
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR3_SVS2_TARGET_BMSK                                        0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR3_SVS2_TARGET_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000024c)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW8_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CPR5_SVS_QUOT_VMIN_BMSK                                  0xff0000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CPR5_SVS_QUOT_VMIN_SHFT                                      0x10
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CPR5_NOMINAL_QUOT_VMIN_BMSK                                0xff00
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CPR5_NOMINAL_QUOT_VMIN_SHFT                                   0x8
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CPR5_TURBO_QUOT_VMIN_BMSK                                    0xff
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CPR5_TURBO_QUOT_VMIN_SHFT                                     0x0

#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000250)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW9_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR6_NOM_ROSEL_1_0_BMSK                                0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR6_NOM_ROSEL_1_0_SHFT                                      0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR6_TUR_ROSEL_BMSK                                    0x38000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR6_TUR_ROSEL_SHFT                                          0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR5_SVS_OFFSET_BMSK                                    0x7e00000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR5_SVS_OFFSET_SHFT                                         0x15
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR5_NOMINAL_OFFSET_BMSK                                 0x1f8000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR5_NOMINAL_OFFSET_SHFT                                      0xf
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR5_TURBO_OFFSET_BMSK                                     0x7f00
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR5_TURBO_OFFSET_SHFT                                        0x8
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR5_SVS2_QUOT_VMIN_BMSK                                     0xff
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_CPR5_SVS2_QUOT_VMIN_SHFT                                      0x0

#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000254)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW9_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_SPARE1_BMSK                                              0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_SPARE1_SHFT                                                  0x16
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CPR6_SVS_TARGET_BMSK                                     0x3f0000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CPR6_SVS_TARGET_SHFT                                         0x10
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CPR6_NOMINAL_TARGET_BMSK                                   0xfc00
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CPR6_NOMINAL_TARGET_SHFT                                      0xa
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CPR6_TURBO_TARGET_BMSK                                      0x3f0
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CPR6_TURBO_TARGET_SHFT                                        0x4
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CPR6_SVS_ROSEL_BMSK                                           0xe
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CPR6_SVS_ROSEL_SHFT                                           0x1
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CPR6_NOM_ROSEL_2_BMSK                                         0x1
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_CPR6_NOM_ROSEL_2_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000258)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW10_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CPR6_NOMINAL_OFFSET_0_BMSK                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CPR6_NOMINAL_OFFSET_0_SHFT                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CPR6_TURBO_OFFSET_BMSK                                0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CPR6_TURBO_OFFSET_SHFT                                      0x18
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CPR6_SVS_QUOT_VMIN_BMSK                                 0xff0000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CPR6_SVS_QUOT_VMIN_SHFT                                     0x10
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CPR6_NOMINAL_QUOT_VMIN_BMSK                               0xff00
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CPR6_NOMINAL_QUOT_VMIN_SHFT                                  0x8
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CPR6_TURBO_QUOT_VMIN_BMSK                                   0xff
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_CPR6_TURBO_QUOT_VMIN_SHFT                                    0x0

#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000025c)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW10_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_SPARE1_BMSK                                             0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_SPARE1_SHFT                                                 0x17
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TXDAC1_SPARE_BMSK                                       0x7f8000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_TXDAC1_SPARE_SHFT                                            0xf
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_CPR_GLOBAL_RC_BMSK                                        0x7000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_CPR_GLOBAL_RC_SHFT                                           0xc
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_SPARE2_BMSK                                                0x800
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_SPARE2_SHFT                                                  0xb
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_CPR6_SVS_OFFSET_BMSK                                       0x7e0
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_CPR6_SVS_OFFSET_SHFT                                         0x5
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_CPR6_NOMINAL_OFFSET_5_1_BMSK                                0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_CPR6_NOMINAL_OFFSET_5_1_SHFT                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000260)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_CX_ACTIVE_MEAS_FUSE_BITS_1_0_BMSK                0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_CX_ACTIVE_MEAS_FUSE_BITS_1_0_SHFT                      0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_A72_MAX_P3_ACTIVE_MEAS_FUSE_BITS_BMSK            0x3f000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_A72_MAX_P3_ACTIVE_MEAS_FUSE_BITS_SHFT                  0x18
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_A72_MAX_P2_ACTIVE_MEAS_FUSE_BITS_BMSK              0xfc0000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_A72_MAX_P2_ACTIVE_MEAS_FUSE_BITS_SHFT                  0x12
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_A72_MAX_P1_ACTIVE_MEAS_FUSE_BITS_BMSK               0x3f000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_A72_MAX_P1_ACTIVE_MEAS_FUSE_BITS_SHFT                   0xc
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_A72_MAX_ACTIVE_MEAS_FUSE_BITS_BMSK                    0xfc0
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_A72_MAX_ACTIVE_MEAS_FUSE_BITS_SHFT                      0x6
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_A72_L2_ACTIVE_MEAS_FUSE_BITS_BMSK                      0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IDDQ_A72_L2_ACTIVE_MEAS_FUSE_BITS_SHFT                       0x0

#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000264)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_SPARE1_BMSK                                             0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_SPARE1_SHFT                                                 0x15
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IDDQ_REV_CTRL_BMSK                                      0x180000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IDDQ_REV_CTRL_SHFT                                          0x13
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_A72_WORST_CORE_NUM_BMSK                                  0x60000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_A72_WORST_CORE_NUM_SHFT                                     0x11
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_MULTIPLICATION_FACTOR_BMSK                               0x18000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_MULTIPLICATION_FACTOR_SHFT                                   0xf
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IDDQ_MX_ACTIVE_MEAS_FUSE_BITS_BMSK                        0x7e00
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IDDQ_MX_ACTIVE_MEAS_FUSE_BITS_SHFT                           0x9
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IDDQ_GFX_ACTIVE_SUM_MEAS_FUSE_BITS_BMSK                    0x1f8
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IDDQ_GFX_ACTIVE_SUM_MEAS_FUSE_BITS_SHFT                      0x3
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IDDQ_CX_ACTIVE_MEAS_FUSE_BITS_4_2_BMSK                       0x7
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IDDQ_CX_ACTIVE_MEAS_FUSE_BITS_4_2_SHFT                       0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000268 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_MAXn                                                      16
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

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000026c + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_MAXn                                                      16
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

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002f0)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002f4)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_SPARE0_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_SPARE0_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_FEC_VALUE_BMSK                                   0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_FEC_VALUE_SHFT                                         0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_MEM_ACCEL_BMSK                                     0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002f8)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002fc)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_SPARE0_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_SPARE0_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_FEC_VALUE_BMSK                                   0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_FEC_VALUE_SHFT                                         0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_MEM_ACCEL_BMSK                                     0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000300 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000304 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000400)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_LSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_RSVD0_BMSK                                                 0xf8000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_RSVD0_SHFT                                                       0x1b
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

#define HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000404)
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

#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000408)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG18_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG18_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_SPARE0_BMSK                                           0xffffff00
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_SPARE0_SHFT                                                  0x8
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_SAFE_SWITCH_BMSK                                            0xff
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_SAFE_SWITCH_SHFT                                             0x0

#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000040c)
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

#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000410)
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

#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000414)
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

#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000418)
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

#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000041c)
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

#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000420)
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

#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000424)
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

#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000428)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG22_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG22_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG22_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG22_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG22_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG22_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000042c)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG22_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG22_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG22_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG22_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG22_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG22_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000430)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG23_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG23_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG23_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG23_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG23_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG23_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000434)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG23_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG23_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG23_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG23_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG23_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG23_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000438)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG24_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG24_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG24_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG24_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG24_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG24_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000043c)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG24_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG24_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG24_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG24_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG24_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG24_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000440)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG25_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG25_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG25_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG25_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG25_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG25_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000444)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG25_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG25_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG25_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG25_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG25_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG25_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000448)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG26_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG26_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG26_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG26_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG26_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG26_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000044c)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG26_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG26_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG26_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG26_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG26_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG26_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_SPARE1_SHFT                                                  0x0

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

#define HWIO_FEAT_PROV_OUTn_ADDR(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000020a0 + 0x4 * (n))
#define HWIO_FEAT_PROV_OUTn_RMSK                                                              0xffffffff
#define HWIO_FEAT_PROV_OUTn_MAXn                                                                       3
#define HWIO_FEAT_PROV_OUTn_INI(n)        \
        in_dword_masked(HWIO_FEAT_PROV_OUTn_ADDR(n), HWIO_FEAT_PROV_OUTn_RMSK)
#define HWIO_FEAT_PROV_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_FEAT_PROV_OUTn_ADDR(n), mask)
#define HWIO_FEAT_PROV_OUTn_FEAT_PROV_OUT_VALUE_BMSK                                          0xffffffff
#define HWIO_FEAT_PROV_OUTn_FEAT_PROV_OUT_VALUE_SHFT                                                 0x0

#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000020b0 + 0x4 * (n))
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

#define HWIO_QFPROM_CORR_JTAG_ID_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004120)
#define HWIO_QFPROM_CORR_JTAG_ID_RMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_JTAG_ID_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_ADDR, HWIO_QFPROM_CORR_JTAG_ID_RMSK)
#define HWIO_QFPROM_CORR_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_ADDR, m)
#define HWIO_QFPROM_CORR_JTAG_ID_SPARE0_BMSK                                                  0xe0000000
#define HWIO_QFPROM_CORR_JTAG_ID_SPARE0_SHFT                                                        0x1d
#define HWIO_QFPROM_CORR_JTAG_ID_MACCHIATO_EN_BMSK                                            0x10000000
#define HWIO_QFPROM_CORR_JTAG_ID_MACCHIATO_EN_SHFT                                                  0x1c
#define HWIO_QFPROM_CORR_JTAG_ID_FEATURE_ID_BMSK                                               0xff00000
#define HWIO_QFPROM_CORR_JTAG_ID_FEATURE_ID_SHFT                                                    0x14
#define HWIO_QFPROM_CORR_JTAG_ID_JTAG_ID_BMSK                                                    0xfffff
#define HWIO_QFPROM_CORR_JTAG_ID_JTAG_ID_SHFT                                                        0x0

#define HWIO_QFPROM_CORR_PTE1_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004124)
#define HWIO_QFPROM_CORR_PTE1_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE1_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, HWIO_QFPROM_CORR_PTE1_RMSK)
#define HWIO_QFPROM_CORR_PTE1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, m)
#define HWIO_QFPROM_CORR_PTE1_IDDQ_MSS_ACTIVE_MEAS_FUSE_BITS_BMSK                             0xfc000000
#define HWIO_QFPROM_CORR_PTE1_IDDQ_MSS_ACTIVE_MEAS_FUSE_BITS_SHFT                                   0x1a
#define HWIO_QFPROM_CORR_PTE1_PROCESS_NODE_ID_BMSK                                             0x2000000
#define HWIO_QFPROM_CORR_PTE1_PROCESS_NODE_ID_SHFT                                                  0x19
#define HWIO_QFPROM_CORR_PTE1_SPARE2_BMSK                                                      0x1800000
#define HWIO_QFPROM_CORR_PTE1_SPARE2_SHFT                                                           0x17
#define HWIO_QFPROM_CORR_PTE1_BONE_PILE_BMSK                                                    0x400000
#define HWIO_QFPROM_CORR_PTE1_BONE_PILE_SHFT                                                        0x16
#define HWIO_QFPROM_CORR_PTE1_A53_ACC_SETTINGS_ID_BMSK                                          0x300000
#define HWIO_QFPROM_CORR_PTE1_A53_ACC_SETTINGS_ID_SHFT                                              0x14
#define HWIO_QFPROM_CORR_PTE1_SPARE1_BMSK                                                        0xfe000
#define HWIO_QFPROM_CORR_PTE1_SPARE1_SHFT                                                            0xd
#define HWIO_QFPROM_CORR_PTE1_WAFER_BIN_BMSK                                                      0x1c00
#define HWIO_QFPROM_CORR_PTE1_WAFER_BIN_SHFT                                                         0xa
#define HWIO_QFPROM_CORR_PTE1_METAL_REV_BMSK                                                       0x300
#define HWIO_QFPROM_CORR_PTE1_METAL_REV_SHFT                                                         0x8
#define HWIO_QFPROM_CORR_PTE1_SPARE0_BMSK                                                           0xff
#define HWIO_QFPROM_CORR_PTE1_SPARE0_SHFT                                                            0x0

#define HWIO_QFPROM_CORR_SERIAL_NUM_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004128)
#define HWIO_QFPROM_CORR_SERIAL_NUM_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_PTE2_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000412c)
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

#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004130)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RSVD0_BMSK                                            0xf8000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RSVD0_SHFT                                                  0x1b
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE26_BMSK                                           0x4000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE26_SHFT                                                0x1a
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE25_BMSK                                           0x2000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE25_SHFT                                                0x19
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE24_BMSK                                           0x1000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE24_SHFT                                                0x18
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE23_BMSK                                            0x800000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE23_SHFT                                                0x17
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE22_BMSK                                            0x400000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE22_SHFT                                                0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE21_BMSK                                            0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE21_SHFT                                                0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE20_BMSK                                            0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE20_SHFT                                                0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_BMSK                                             0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_SHFT                                                0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_BMSK                                             0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_SHFT                                                0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_BMSK                                              0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_SHFT                                                 0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                      0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                         0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_BMSK                                           0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_SHFT                                              0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_BMSK                                                0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_SHFT                                                   0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_BMSK                                              0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_SHFT                                                 0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB12_BMSK                                              0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB12_SHFT                                                 0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                          0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                            0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                  0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                  0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CM_FEAT_CONFIG_BMSK                                        0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CM_FEAT_CONFIG_SHFT                                          0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                            0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                             0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                             0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                              0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                        0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                         0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                        0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                         0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                         0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                              0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                              0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_BMSK                                                     0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_SHFT                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CRI_CM_PRIVATE_BMSK                                          0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CRI_CM_PRIVATE_SHFT                                          0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004134)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RSVD0_BMSK                                            0xf8000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RSVD0_SHFT                                                  0x1b
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE26_BMSK                                           0x4000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE26_SHFT                                                0x1a
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE25_BMSK                                           0x2000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE25_SHFT                                                0x19
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE24_BMSK                                           0x1000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE24_SHFT                                                0x18
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE23_BMSK                                            0x800000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE23_SHFT                                                0x17
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE22_BMSK                                            0x400000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE22_SHFT                                                0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE21_BMSK                                            0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE21_SHFT                                                0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE20_BMSK                                            0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE20_SHFT                                                0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_BMSK                                             0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_SHFT                                                0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_BMSK                                             0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_SHFT                                                0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_BMSK                                              0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_SHFT                                                 0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                      0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                         0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_BMSK                                           0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_SHFT                                              0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_BMSK                                                0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_SHFT                                                   0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_BMSK                                              0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_SHFT                                                 0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB12_BMSK                                              0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB12_SHFT                                                 0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                          0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                            0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                  0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                  0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CM_FEAT_CONFIG_BMSK                                        0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CM_FEAT_CONFIG_SHFT                                          0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                            0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                             0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                             0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                              0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_BMSK                                        0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_SHFT                                         0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_BMSK                                        0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_SHFT                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_BMSK                                         0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_SHFT                                         0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                              0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                              0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_BMSK                                                     0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_SHFT                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CRI_CM_PRIVATE_BMSK                                          0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CRI_CM_PRIVATE_SHFT                                          0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004138)
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

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000413c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_APPSBL0_BMSK                                     0xfffc0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_APPSBL0_SHFT                                           0x12
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_BMSK                                 0x3ffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_SHFT                                     0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004140)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_APPSBL1_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_APPSBL1_SHFT                                            0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004144)
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

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004148)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_BMSK                                         0xffff0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_SHFT                                               0x10
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_BMSK                                             0xffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_SHFT                                                0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000414c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SPARE0_BMSK                                      0xffffff00
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SPARE0_SHFT                                             0x8
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                     0xff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                      0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004150)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD0_BMSK                                       0xc0000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD0_SHFT                                             0x1e
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_BMSK                     0x20000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_SHFT                           0x1d
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_DISABLE_POLICY_BMSK                        0x10000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_DISABLE_POLICY_SHFT                              0x1c
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG26_SECURE_BMSK                           0x8000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG26_SECURE_SHFT                                0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG25_SECURE_BMSK                           0x4000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG25_SECURE_SHFT                                0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG24_SECURE_BMSK                           0x2000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG24_SECURE_SHFT                                0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG23_SECURE_BMSK                           0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG23_SECURE_SHFT                                0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG22_SECURE_BMSK                            0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG22_SECURE_SHFT                                0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                               0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                   0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                           0x3c0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                               0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_BMSK                             0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_SHFT                                0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_BMSK                             0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_SHFT                                0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_BMSK                                          0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_SHFT                                             0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_BMSK                              0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_SHFT                                 0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_BMSK                              0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_SHFT                                 0xd
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

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004154)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                      0xf8000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                            0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SDCC5_SCM_FORCE_EFUSE_KEY_BMSK                    0x4000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SDCC5_SCM_FORCE_EFUSE_KEY_SHFT                         0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_A5X_ISDB_DBGEN_DISABLE_BMSK                       0x2000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_A5X_ISDB_DBGEN_DISABLE_SHFT                            0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_NIDEN_DISABLE_BMSK                          0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_NIDEN_DISABLE_SHFT                               0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_DBGEN_DISABLE_BMSK                           0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_LPASS_DBGEN_DISABLE_SHFT                               0x17
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

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004158)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_BMSK                              0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_SHFT                                    0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_BMSK                                       0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_SHFT                                          0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000415c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_VID_BMSK                                  0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_VID_SHFT                                        0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_PID_BMSK                                      0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_PID_SHFT                                         0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004160)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RESOLUTION_LIMITER_0_BMSK                       0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RESOLUTION_LIMITER_0_SHFT                             0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                              0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                    0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RESOLUTION_LIMITER_1_BMSK                       0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RESOLUTION_LIMITER_1_SHFT                             0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDP_EFUSE_LTC0_DISABLE_BMSK                     0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDP_EFUSE_LTC0_DISABLE_SHFT                           0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_BMSK                               0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_SHFT                                    0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_HEVC_BMSK                          0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VENUS_DISABLE_HEVC_SHFT                               0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SECURE_CHANNEL_DISABLE_BMSK                      0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SECURE_CHANNEL_DISABLE_SHFT                           0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                         0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                              0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                          0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                              0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_22_BMSK                                     0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_22_SHFT                                         0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_BMSK                                  0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_SHFT                                      0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_20_BMSK                                     0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_20_SHFT                                         0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_BMSK                           0xe0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_SHFT                              0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_UIM2_DISABLE_BMSK                            0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_UIM2_DISABLE_SHFT                               0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_UIM1_DISABLE_BMSK                             0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_UIM1_DISABLE_SHFT                                0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SMMU_TBU_BYPASS_DISABLE_BMSK                        0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SMMU_TBU_BYPASS_DISABLE_SHFT                           0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_DSDA_DISABLE_BMSK                             0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_DSDA_DISABLE_SHFT                                0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_BMSK                            0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_SHFT                               0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_BMSK                             0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_SHFT                               0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_BMSK                            0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_SHFT                              0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_9_BMSK                                         0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_9_SHFT                                           0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_8_BMSK                                         0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_8_SHFT                                           0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_7_BMSK                                          0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_7_SHFT                                           0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_BMSK                     0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_SHFT                      0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_BMSK                            0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_SHFT                             0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_BMSK                               0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_SHFT                                0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_BMSK                                 0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_SHFT                                 0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_BMSK                               0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_SHFT                               0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_BMSK                                  0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_SHFT                                  0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_BMSK                                  0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_SHFT                                  0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004164)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                      0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_BMSK                     0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_SHFT                           0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_GFX_FREQ_LIMIT_FUSE_BMSK                        0x30000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_GFX_FREQ_LIMIT_FUSE_SHFT                              0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SYSBARDISABLE_BMSK                               0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SYSBARDISABLE_SHFT                                    0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE6_BMSK                                      0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE6_SHFT                                           0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_DDR_FREQ_LIMIT_EN_BMSK                           0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_DDR_FREQ_LIMIT_EN_SHFT                                0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_BMSK                                 0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_SHFT                                      0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_BMSK                      0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_SHFT                          0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_BMSK                      0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_SHFT                          0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_BMSK                            0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_SHFT                                0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE5_BMSK                                       0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE5_SHFT                                           0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_SCM_DISABLE_BMSK                             0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_SCM_DISABLE_SHFT                                0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_CCI_FREQ_SCALE_BMSK                                0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_CCI_FREQ_SCALE_SHFT                                   0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_BMSK                                 0x38000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_SHFT                                     0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_DCC_DISABLE_BMSK                                    0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_DCC_DISABLE_SHFT                                       0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_LTE_UL_CA_DISABLE_BMSK                         0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_LTE_UL_CA_DISABLE_SHFT                            0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_LTE_ABOVE_CAT4_DISABLE_BMSK                    0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_LTE_ABOVE_CAT4_DISABLE_SHFT                       0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_HSUPA_DC_DISABLE_BMSK                     0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_HSUPA_DC_DISABLE_SHFT                       0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_VP9D_BMSK                              0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_VP9D_SHFT                                0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE2_BMSK                                          0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE2_SHFT                                            0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_265E_BMSK                              0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_265E_SHFT                                0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_4KX2KD_BMSK                             0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_4KX2KD_SHFT                              0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_CSID_2_DISABLE_BMSK                                   0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_CSID_2_DISABLE_SHFT                                    0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BMSK                                0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_SHFT                                 0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_BMSK                              0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_SHFT                               0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE1_BMSK                                            0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE1_SHFT                                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_L2_CACHE_UPPER_BANK_DISABLE_BMSK                  0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_L2_CACHE_UPPER_BANK_DISABLE_SHFT                  0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004168)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                     0xfc000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                           0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_CAMCC_VFE_1_EFUSE_LIM_RES_8MP_BMSK               0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_CAMCC_VFE_1_EFUSE_LIM_RES_8MP_SHFT                    0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_CAMCC_VFE_0_EFUSE_LIM_RES_8MP_BMSK               0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_CAMCC_VFE_0_EFUSE_LIM_RES_8MP_SHFT                    0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_A5X_ISDB_DBGEN_DISABLE_BMSK                    0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_A5X_ISDB_DBGEN_DISABLE_SHFT                        0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_LPASS_NIDEN_DISABLE_BMSK                       0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_LPASS_NIDEN_DISABLE_SHFT                           0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_BMSK                       0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_SHFT                           0x15
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

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000416c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                     0xfe000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                           0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                    0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                         0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                            0xfff800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                 0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SPARE1_BMSK                                          0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SPARE1_SHFT                                            0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_BMSK                         0x3e0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_SHFT                           0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                           0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                            0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004170)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK           0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                  0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004174)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_BMSK                     0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_SHFT                           0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_BMSK                     0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_SHFT                           0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_BMSK                     0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_SHFT                           0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_4_BMSK                     0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_4_SHFT                           0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_GCC_DISABLE_BOOT_FSM_BMSK                        0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_GCC_DISABLE_BOOT_FSM_SHFT                             0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_BMSK                          0x7800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_SHFT                               0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                            0x780000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_3_BMSK                        0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_3_SHFT                           0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_2_BMSK                        0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_2_SHFT                           0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_1_BMSK                        0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_1_SHFT                           0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_CSR_BMSK                             0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_CSR_SHFT                                0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_BMSK                            0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_SHFT                               0xe
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

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ADDR                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004178)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RSVD0_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RSVD0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ADDR                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000417c)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD0_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_ADDR                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004180)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_RSVD0_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_RSVD0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_ADDR                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004184)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_RSVD0_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_RSVD0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_ADDR                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004188)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_RSVD0_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_RSVD0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_ADDR                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000418c)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004190 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                          3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                              0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004194 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                          3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                              0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                          3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                              0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                          3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                              0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d0)
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

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d4)
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

#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d8)
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_SPARE0_BMSK                                         0xc0000000
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_SPARE0_SHFT                                               0x1e
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_CPR6_AGING_SENSOR_0_BMSK                            0x3f000000
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_CPR6_AGING_SENSOR_0_SHFT                                  0x18
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_CPR5_AGING_SENSOR_0_BMSK                              0xfc0000
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_CPR5_AGING_SENSOR_0_SHFT                                  0x12
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_CPR2_AGING_SENSOR_0_BMSK                               0x3f000
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_CPR2_AGING_SENSOR_0_SHFT                                   0xc
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_CPR1_AGING_SENSOR_0_BMSK                                 0xfc0
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_CPR1_AGING_SENSOR_0_SHFT                                   0x6
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_CPR0_AGING_SENSOR_0_BMSK                                  0x3f
#define HWIO_QFPROM_CORR_CALIB12_ROW0_LSB_CPR0_AGING_SENSOR_0_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041dc)
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_RMSK                                                  0xffffff
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_CPR_LOCAL_RC_BMSK                                     0xe00000
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_CPR_LOCAL_RC_SHFT                                         0x15
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_CPR3_NOM_TARGET_BMSK                                  0x1f0000
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_CPR3_NOM_TARGET_SHFT                                      0x10
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_CPR3_TURBO_TARGET_BMSK                                  0xf800
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_CPR3_TURBO_TARGET_SHFT                                     0xb
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_CPR1_SVS2_TARGET_BMSK                                    0x7c0
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_CPR1_SVS2_TARGET_SHFT                                      0x6
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_CPR3_SVS_TARGET_BMSK                                      0x3e
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_CPR3_SVS_TARGET_SHFT                                       0x1
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_SPARE1_BMSK                                                0x1
#define HWIO_QFPROM_CORR_CALIB12_ROW0_MSB_SPARE1_SHFT                                                0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_MAXn                                                         3
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_HASH_DATA0_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_HASH_DATA0_SHFT                                            0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_RMSK                                                  0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_MAXn                                                         3
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                       0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                            0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004200)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_HASH_DATA0_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_HASH_DATA0_SHFT                                            0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004204)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_RMSK                                                  0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_QFPROM_CORR_PK_HASH_ROW4_MSB_BMSK                     0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_QFPROM_CORR_PK_HASH_ROW4_MSB_SHFT                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004208)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CS_SENSOR_PARAM_CORE_1_8_0_BMSK                       0xff800000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CS_SENSOR_PARAM_CORE_1_8_0_SHFT                             0x17
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CS_SENSOR_PARAM_CORE_0_BMSK                             0x7fe000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CS_SENSOR_PARAM_CORE_0_SHFT                                  0xd
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CSI_PHY_BMSK                                              0x1f00
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CSI_PHY_SHFT                                                 0x8
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSI_PHY_BMSK                                                0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSI_PHY_SHFT                                                 0x4
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSIPHY_PLL_BMSK                                              0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSIPHY_PLL_SHFT                                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000420c)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_2_0_BMSK               0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_2_0_SHFT                   0x15
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_3_BMSK                             0x1ff800
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_3_SHFT                                  0xb
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_2_BMSK                                0x7fe
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_2_SHFT                                  0x1
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_1_9_BMSK                                0x1
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CS_SENSOR_PARAM_CORE_1_9_SHFT                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004210)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_2_INTERCEPT_6_0_BMSK             0xfe000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_2_INTERCEPT_6_0_SHFT                   0x19
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_1_INTERCEPT_BMSK                  0x1ff8000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_1_INTERCEPT_SHFT                        0xf
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_9_4_BMSK                 0x7e00
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_9_4_SHFT                    0x9
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IDDQ_A53_ACTIVE_MEAS_BMSK                                  0x1fe
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IDDQ_A53_ACTIVE_MEAS_SHFT                                    0x1
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_3_BMSK                      0x1
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CS_SENSOR_PARAM_CORE_0_INTERCEPT_3_SHFT                      0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004214)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE1_BMSK                                             0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE1_SHFT                                                 0x16
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_APC1_LDO_VREF_TRIM_BMSK                                 0x3e0000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_APC1_LDO_VREF_TRIM_SHFT                                     0x11
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_AMP_COMP_BMSK                                            0x1e000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_AMP_COMP_SHFT                                                0xd
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CS_SENSOR_PARAM_CORE_3_INTERCEPT_BMSK                     0x1ff8
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CS_SENSOR_PARAM_CORE_3_INTERCEPT_SHFT                        0x3
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CS_SENSOR_PARAM_CORE_2_INTERCEPT_9_7_BMSK                    0x7
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CS_SENSOR_PARAM_CORE_2_INTERCEPT_9_7_SHFT                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004218)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS1_POINT2_BMSK                                    0xfc000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS1_POINT2_SHFT                                          0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS1_POINT1_BMSK                                     0x3f00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS1_POINT1_SHFT                                          0x14
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS0_POINT2_BMSK                                       0xfc000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS0_POINT2_SHFT                                           0xe
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS0_POINT1_BMSK                                        0x3f00
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS0_POINT1_SHFT                                           0x8
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS_BASE0_BMSK                                            0xff
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS_BASE0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000421c)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS3_POINT2_BMSK                                      0xfc0000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS3_POINT2_SHFT                                          0x12
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS3_POINT1_BMSK                                       0x3f000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS3_POINT1_SHFT                                           0xc
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS2_POINT2_BMSK                                         0xfc0
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS2_POINT2_SHFT                                           0x6
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS2_POINT1_BMSK                                          0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS2_POINT1_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004220)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS5_POINT2_BMSK                                    0xfc000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS5_POINT2_SHFT                                          0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS5_POINT1_BMSK                                     0x3f00000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS5_POINT1_SHFT                                          0x14
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS4_POINT2_BMSK                                       0xfc000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS4_POINT2_SHFT                                           0xe
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS4_POINT1_BMSK                                        0x3f00
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS4_POINT1_SHFT                                           0x8
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS_BASE1_BMSK                                            0xff
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS_BASE1_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004224)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TSENS7_POINT2_BMSK                                      0xfc0000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TSENS7_POINT2_SHFT                                          0x12
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TSENS7_POINT1_BMSK                                       0x3f000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TSENS7_POINT1_SHFT                                           0xc
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TSENS6_POINT2_BMSK                                         0xfc0
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TSENS6_POINT2_SHFT                                           0x6
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TSENS6_POINT1_BMSK                                          0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TSENS6_POINT1_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004228)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS10_POINT1_4_0_BMSK                               0xf8000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS10_POINT1_4_0_SHFT                                     0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS9_POINT2_BMSK                                     0x7e00000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS9_POINT2_SHFT                                          0x15
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS9_POINT1_BMSK                                      0x1f8000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS9_POINT1_SHFT                                           0xf
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS8_POINT2_BMSK                                        0x7e00
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS8_POINT2_SHFT                                           0x9
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS8_POINT1_BMSK                                         0x1f8
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS8_POINT1_SHFT                                           0x3
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS_CALIB_BMSK                                             0x7
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_TSENS_CALIB_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000422c)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TXDAC0_SPARE_6_0_BMSK                                   0xfe0000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TXDAC0_SPARE_6_0_SHFT                                       0x11
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TXDAC0_RANGE_CORR_BMSK                                   0x10000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TXDAC0_RANGE_CORR_SHFT                                      0x10
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TXDAC0_AVEG_CORR_BMSK                                     0x8000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TXDAC0_AVEG_CORR_SHFT                                        0xf
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TXDAC0_RPOLY_CAL_BMSK                                     0x7f80
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TXDAC0_RPOLY_CAL_SHFT                                        0x7
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TSENS10_POINT2_BMSK                                         0x7e
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TSENS10_POINT2_SHFT                                          0x1
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TSENS10_POINT1_5_BMSK                                        0x1
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_TSENS10_POINT1_5_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004230)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B0M3_1_0_BMSK                                      0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B0M3_1_0_SHFT                                            0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B0M2_BMSK                                          0x38000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B0M2_SHFT                                                0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B0M1_BMSK                                           0x7000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B0M1_SHFT                                                0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B0M0_BMSK                                            0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B0M0_SHFT                                                0x15
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_G_B0_BMSK                                               0x1c0000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_G_B0_SHFT                                                   0x12
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CLK_B_BMSK                                               0x30000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CLK_B_SHFT                                                  0x10
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CAP_B_BMSK                                                0xc000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CAP_B_SHFT                                                   0xe
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_SAR_B_BMSK                                                0x3000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_SAR_B_SHFT                                                   0xc
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_MODEM_TXDAC_FUSEFLAG_BMSK                                  0x800
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_MODEM_TXDAC_FUSEFLAG_SHFT                                    0xb
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TXDAC1_RANGE_CORR_BMSK                                     0x400
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TXDAC1_RANGE_CORR_SHFT                                       0xa
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TXDAC1_AVEG_CORR_BMSK                                      0x200
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TXDAC1_AVEG_CORR_SHFT                                        0x9
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TXDAC1_RPOLY_CAL_BMSK                                      0x1fe
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TXDAC1_RPOLY_CAL_SHFT                                        0x1
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TXDAC0_SPARE_7_BMSK                                          0x1
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_TXDAC0_SPARE_7_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004234)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B2M0_0_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B2M0_0_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_G_B2_BMSK                                               0x700000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_G_B2_SHFT                                                   0x14
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_VREF_B1_BMSK                                             0xc0000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_VREF_B1_SHFT                                                0x12
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B1M3_BMSK                                             0x38000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B1M3_SHFT                                                 0xf
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B1M2_BMSK                                              0x7000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B1M2_SHFT                                                 0xc
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B1M1_BMSK                                               0xe00
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B1M1_SHFT                                                 0x9
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B1M0_BMSK                                               0x1c0
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B1M0_SHFT                                                 0x6
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_G_B1_BMSK                                                   0x38
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_G_B1_SHFT                                                    0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_VREF_B0_BMSK                                                 0x6
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_VREF_B0_SHFT                                                 0x1
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B0M3_2_BMSK                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B0M3_2_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004238)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_GNSS_ADC_CALIB_1_0_BMSK                               0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_GNSS_ADC_CALIB_1_0_SHFT                                     0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_B3_BMSK                                          0x30000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_B3_SHFT                                                0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B3M3_BMSK                                           0xe000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B3M3_SHFT                                                0x19
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B3M2_BMSK                                           0x1c00000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B3M2_SHFT                                                0x16
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B3M1_BMSK                                            0x380000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B3M1_SHFT                                                0x13
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B3M0_BMSK                                             0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B3M0_SHFT                                                0x10
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_G_B3_BMSK                                                 0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_G_B3_SHFT                                                    0xd
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_B2_BMSK                                              0x1800
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_B2_SHFT                                                 0xb
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B2M3_BMSK                                               0x700
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B2M3_SHFT                                                 0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B2M2_BMSK                                                0xe0
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B2M2_SHFT                                                 0x5
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B2M1_BMSK                                                0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B2M1_SHFT                                                 0x2
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B2M0_2_1_BMSK                                             0x3
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B2M0_2_1_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000423c)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_SPARE1_BMSK                                             0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_SPARE1_SHFT                                                 0x14
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_VOLTAGE_SENSOR_CALIB_BMSK                                0xfffc0
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_VOLTAGE_SENSOR_CALIB_SHFT                                    0x6
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_GNSS_ADC_CALIB_7_2_BMSK                                     0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_GNSS_ADC_CALIB_7_2_SHFT                                      0x0

#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004240)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_SPARE0_BMSK                                           0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_SPARE0_SHFT                                                 0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR1_SVS_TARGET_BMSK                                  0x3e000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR1_SVS_TARGET_SHFT                                        0x19
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR1_NOMINAL_TARGET_BMSK                               0x1f00000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR1_NOMINAL_TARGET_SHFT                                    0x14
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR1_TURBO_TARGET_BMSK                                   0xf8000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR1_TURBO_TARGET_SHFT                                       0xf
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR0_SVS_TARGET_BMSK                                      0x7c00
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR0_SVS_TARGET_SHFT                                         0xa
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR0_NOMINAL_TARGET_BMSK                                   0x3e0
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR0_NOMINAL_TARGET_SHFT                                     0x5
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR0_TURBO_TARGET_BMSK                                      0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR0_TURBO_TARGET_SHFT                                       0x0

#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004244)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_SPARE2_BMSK                                             0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_SPARE2_SHFT                                                 0x17
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR2_SVS2_TARGET_BMSK                                   0x7c0000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR2_SVS2_TARGET_SHFT                                       0x12
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR2_SVS_TARGET_BMSK                                     0x3e000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR2_SVS_TARGET_SHFT                                         0xd
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR2_NOMINAL_TARGET_BMSK                                  0x1f00
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR2_NOMINAL_TARGET_SHFT                                     0x8
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR2_TURBO_TARGET_BMSK                                      0xf8
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR2_TURBO_TARGET_SHFT                                       0x3
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_SPARE1_BMSK                                                  0x7
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004248)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW8_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_SVS_TARGET_BMSK                                  0xfc000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_SVS_TARGET_SHFT                                        0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_NOMINAL_TARGET_BMSK                               0x3f00000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_NOMINAL_TARGET_SHFT                                    0x14
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_TURBO_TARGET_BMSK                                   0xfc000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_TURBO_TARGET_SHFT                                       0xe
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_SVS_ROSEL_BMSK                                       0x3800
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_SVS_ROSEL_SHFT                                          0xb
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_NOM_ROSEL_BMSK                                        0x700
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_NOM_ROSEL_SHFT                                          0x8
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_TUR_ROSEL_BMSK                                         0xe0
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_TUR_ROSEL_SHFT                                          0x5
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR3_SVS2_TARGET_BMSK                                       0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR3_SVS2_TARGET_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000424c)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR5_SVS_QUOT_VMIN_BMSK                                 0xff0000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR5_SVS_QUOT_VMIN_SHFT                                     0x10
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR5_NOMINAL_QUOT_VMIN_BMSK                               0xff00
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR5_NOMINAL_QUOT_VMIN_SHFT                                  0x8
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR5_TURBO_QUOT_VMIN_BMSK                                   0xff
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR5_TURBO_QUOT_VMIN_SHFT                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004250)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW9_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR6_NOM_ROSEL_1_0_BMSK                               0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR6_NOM_ROSEL_1_0_SHFT                                     0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR6_TUR_ROSEL_BMSK                                   0x38000000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR6_TUR_ROSEL_SHFT                                         0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR5_SVS_OFFSET_BMSK                                   0x7e00000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR5_SVS_OFFSET_SHFT                                        0x15
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR5_NOMINAL_OFFSET_BMSK                                0x1f8000
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR5_NOMINAL_OFFSET_SHFT                                     0xf
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR5_TURBO_OFFSET_BMSK                                    0x7f00
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR5_TURBO_OFFSET_SHFT                                       0x8
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR5_SVS2_QUOT_VMIN_BMSK                                    0xff
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_CPR5_SVS2_QUOT_VMIN_SHFT                                     0x0

#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004254)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW9_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_SPARE1_BMSK                                             0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_SPARE1_SHFT                                                 0x16
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR6_SVS_TARGET_BMSK                                    0x3f0000
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR6_SVS_TARGET_SHFT                                        0x10
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR6_NOMINAL_TARGET_BMSK                                  0xfc00
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR6_NOMINAL_TARGET_SHFT                                     0xa
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR6_TURBO_TARGET_BMSK                                     0x3f0
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR6_TURBO_TARGET_SHFT                                       0x4
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR6_SVS_ROSEL_BMSK                                          0xe
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR6_SVS_ROSEL_SHFT                                          0x1
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR6_NOM_ROSEL_2_BMSK                                        0x1
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_CPR6_NOM_ROSEL_2_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004258)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW10_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR6_NOMINAL_OFFSET_0_BMSK                           0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR6_NOMINAL_OFFSET_0_SHFT                                 0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR6_TURBO_OFFSET_BMSK                               0x7f000000
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR6_TURBO_OFFSET_SHFT                                     0x18
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR6_SVS_QUOT_VMIN_BMSK                                0xff0000
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR6_SVS_QUOT_VMIN_SHFT                                    0x10
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR6_NOMINAL_QUOT_VMIN_BMSK                              0xff00
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR6_NOMINAL_QUOT_VMIN_SHFT                                 0x8
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR6_TURBO_QUOT_VMIN_BMSK                                  0xff
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_CPR6_TURBO_QUOT_VMIN_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000425c)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW10_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_SPARE1_BMSK                                            0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_SPARE1_SHFT                                                0x17
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TXDAC1_SPARE_BMSK                                      0x7f8000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_TXDAC1_SPARE_SHFT                                           0xf
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_GLOBAL_RC_BMSK                                       0x7000
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR_GLOBAL_RC_SHFT                                          0xc
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_SPARE2_BMSK                                               0x800
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_SPARE2_SHFT                                                 0xb
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR6_SVS_OFFSET_BMSK                                      0x7e0
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR6_SVS_OFFSET_SHFT                                        0x5
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR6_NOMINAL_OFFSET_5_1_BMSK                               0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_CPR6_NOMINAL_OFFSET_5_1_SHFT                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004260)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW11_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_CX_ACTIVE_MEAS_FUSE_BITS_1_0_BMSK               0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_CX_ACTIVE_MEAS_FUSE_BITS_1_0_SHFT                     0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_A72_MAX_P3_ACTIVE_MEAS_FUSE_BITS_BMSK           0x3f000000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_A72_MAX_P3_ACTIVE_MEAS_FUSE_BITS_SHFT                 0x18
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_A72_MAX_P2_ACTIVE_MEAS_FUSE_BITS_BMSK             0xfc0000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_A72_MAX_P2_ACTIVE_MEAS_FUSE_BITS_SHFT                 0x12
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_A72_MAX_P1_ACTIVE_MEAS_FUSE_BITS_BMSK              0x3f000
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_A72_MAX_P1_ACTIVE_MEAS_FUSE_BITS_SHFT                  0xc
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_A72_MAX_ACTIVE_MEAS_FUSE_BITS_BMSK                   0xfc0
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_A72_MAX_ACTIVE_MEAS_FUSE_BITS_SHFT                     0x6
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_A72_L2_ACTIVE_MEAS_FUSE_BITS_BMSK                     0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IDDQ_A72_L2_ACTIVE_MEAS_FUSE_BITS_SHFT                      0x0

#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004264)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW11_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_SPARE1_BMSK                                            0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_SPARE1_SHFT                                                0x15
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IDDQ_REV_CTRL_BMSK                                     0x180000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IDDQ_REV_CTRL_SHFT                                         0x13
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_A72_WORST_CORE_NUM_BMSK                                 0x60000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_A72_WORST_CORE_NUM_SHFT                                    0x11
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_MULTIPLICATION_FACTOR_BMSK                              0x18000
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_MULTIPLICATION_FACTOR_SHFT                                  0xf
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IDDQ_MX_ACTIVE_MEAS_FUSE_BITS_BMSK                       0x7e00
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IDDQ_MX_ACTIVE_MEAS_FUSE_BITS_SHFT                          0x9
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IDDQ_GFX_ACTIVE_SUM_MEAS_FUSE_BITS_BMSK                   0x1f8
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IDDQ_GFX_ACTIVE_SUM_MEAS_FUSE_BITS_SHFT                     0x3
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IDDQ_CX_ACTIVE_MEAS_FUSE_BITS_4_2_BMSK                      0x7
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IDDQ_CX_ACTIVE_MEAS_FUSE_BITS_4_2_SHFT                      0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004268 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_MAXn                                                     16
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000426c + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK                                               0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_MAXn                                                     16
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                    0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042f0)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042f4)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_RMSK                                              0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_MEM_ACCEL_BMSK                                    0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042f8)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042fc)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_RMSK                                              0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_MEM_ACCEL_BMSK                                    0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_MEM_ACCEL_SHFT                                         0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004300 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_MAXn                                                      31
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_SHFT                                          0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004304 + 0x8 * (n))
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

#define HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004400)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RSVD0_BMSK                                                0xf8000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RSVD0_SHFT                                                      0x1b
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

#define HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004404)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_FEC_EN_REDUNDANCY_BMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_FEC_EN_REDUNDANCY_SHFT                                           0x0

#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004408)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG18_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_SPARE0_BMSK                                          0xffffff00
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_SPARE0_SHFT                                                 0x8
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_SAFE_SWITCH_BMSK                                           0xff
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_SAFE_SWITCH_SHFT                                            0x0

#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000440c)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG18_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004410)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004414)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004418)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG20_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000441c)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG20_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004420)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG21_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004424)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG21_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004428)
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG22_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG22_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG22_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000442c)
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG22_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG22_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG22_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004430)
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG23_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG23_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG23_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004434)
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG23_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG23_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG23_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004438)
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG24_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG24_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG24_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000443c)
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG24_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG24_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG24_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004440)
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG25_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG25_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG25_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004444)
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG25_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG25_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG25_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004448)
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG26_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG26_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG26_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000444c)
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG26_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG26_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG26_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_SPARE1_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_SPARE1_SHFT                                                 0x0

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
#define HWIO_FEATURE_CONFIG0_RESOLUTION_LIMITER_0_BMSK                                        0x80000000
#define HWIO_FEATURE_CONFIG0_RESOLUTION_LIMITER_0_SHFT                                              0x1f
#define HWIO_FEATURE_CONFIG0_DSI_1_DISABLE_BMSK                                               0x40000000
#define HWIO_FEATURE_CONFIG0_DSI_1_DISABLE_SHFT                                                     0x1e
#define HWIO_FEATURE_CONFIG0_RESOLUTION_LIMITER_1_BMSK                                        0x20000000
#define HWIO_FEATURE_CONFIG0_RESOLUTION_LIMITER_1_SHFT                                              0x1d
#define HWIO_FEATURE_CONFIG0_MDP_EFUSE_LTC0_DISABLE_BMSK                                      0x10000000
#define HWIO_FEATURE_CONFIG0_MDP_EFUSE_LTC0_DISABLE_SHFT                                            0x1c
#define HWIO_FEATURE_CONFIG0_NIDNT_DISABLE_BMSK                                                0x8000000
#define HWIO_FEATURE_CONFIG0_NIDNT_DISABLE_SHFT                                                     0x1b
#define HWIO_FEATURE_CONFIG0_VENUS_DISABLE_HEVC_BMSK                                           0x4000000
#define HWIO_FEATURE_CONFIG0_VENUS_DISABLE_HEVC_SHFT                                                0x1a
#define HWIO_FEATURE_CONFIG0_SECURE_CHANNEL_DISABLE_BMSK                                       0x2000000
#define HWIO_FEATURE_CONFIG0_SECURE_CHANNEL_DISABLE_SHFT                                            0x19
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_BMSK                                          0x1000000
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_SHFT                                               0x18
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_BMSK                                           0x800000
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_SHFT                                               0x17
#define HWIO_FEATURE_CONFIG0_SPARE_22_BMSK                                                      0x400000
#define HWIO_FEATURE_CONFIG0_SPARE_22_SHFT                                                          0x16
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_BMSK                                                   0x200000
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_SHFT                                                       0x15
#define HWIO_FEATURE_CONFIG0_SPARE_20_BMSK                                                      0x100000
#define HWIO_FEATURE_CONFIG0_SPARE_20_SHFT                                                          0x14
#define HWIO_FEATURE_CONFIG0_MODEM_SPARE_DISABLE_BMSK                                            0xe0000
#define HWIO_FEATURE_CONFIG0_MODEM_SPARE_DISABLE_SHFT                                               0x11
#define HWIO_FEATURE_CONFIG0_MODEM_UIM2_DISABLE_BMSK                                             0x10000
#define HWIO_FEATURE_CONFIG0_MODEM_UIM2_DISABLE_SHFT                                                0x10
#define HWIO_FEATURE_CONFIG0_MODEM_UIM1_DISABLE_BMSK                                              0x8000
#define HWIO_FEATURE_CONFIG0_MODEM_UIM1_DISABLE_SHFT                                                 0xf
#define HWIO_FEATURE_CONFIG0_SMMU_TBU_BYPASS_DISABLE_BMSK                                         0x4000
#define HWIO_FEATURE_CONFIG0_SMMU_TBU_BYPASS_DISABLE_SHFT                                            0xe
#define HWIO_FEATURE_CONFIG0_MODEM_DSDA_DISABLE_BMSK                                              0x2000
#define HWIO_FEATURE_CONFIG0_MODEM_DSDA_DISABLE_SHFT                                                 0xd
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX3_DISABLE_BMSK                                             0x1000
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX3_DISABLE_SHFT                                                0xc
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_BMSK                                              0x800
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_SHFT                                                0xb
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_BMSK                                             0x400
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_SHFT                                               0xa
#define HWIO_FEATURE_CONFIG0_SPARE_9_BMSK                                                          0x200
#define HWIO_FEATURE_CONFIG0_SPARE_9_SHFT                                                            0x9
#define HWIO_FEATURE_CONFIG0_SPARE_8_BMSK                                                          0x100
#define HWIO_FEATURE_CONFIG0_SPARE_8_SHFT                                                            0x8
#define HWIO_FEATURE_CONFIG0_SPARE_7_BMSK                                                           0x80
#define HWIO_FEATURE_CONFIG0_SPARE_7_SHFT                                                            0x7
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_BMSK                                      0x40
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_SHFT                                       0x6
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_BMSK                                             0x20
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_SHFT                                              0x5
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_BMSK                                                0x10
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_SHFT                                                 0x4
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_BMSK                                                  0x8
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_SHFT                                                  0x3
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_BMSK                                                0x4
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_SHFT                                                0x2
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_BMSK                                                   0x2
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_SHFT                                                   0x1
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_BMSK                                                   0x1
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_SHFT                                                   0x0

#define HWIO_FEATURE_CONFIG1_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006008)
#define HWIO_FEATURE_CONFIG1_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG1_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, HWIO_FEATURE_CONFIG1_RMSK)
#define HWIO_FEATURE_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, m)
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                 0x80000000
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                       0x1f
#define HWIO_FEATURE_CONFIG1_CM_FEAT_CONFIG_DISABLE_BMSK                                      0x40000000
#define HWIO_FEATURE_CONFIG1_CM_FEAT_CONFIG_DISABLE_SHFT                                            0x1e
#define HWIO_FEATURE_CONFIG1_GFX_FREQ_LIMIT_FUSE_BMSK                                         0x30000000
#define HWIO_FEATURE_CONFIG1_GFX_FREQ_LIMIT_FUSE_SHFT                                               0x1c
#define HWIO_FEATURE_CONFIG1_SYSBARDISABLE_BMSK                                                0x8000000
#define HWIO_FEATURE_CONFIG1_SYSBARDISABLE_SHFT                                                     0x1b
#define HWIO_FEATURE_CONFIG1_SPARE6_BMSK                                                       0x4000000
#define HWIO_FEATURE_CONFIG1_SPARE6_SHFT                                                            0x1a
#define HWIO_FEATURE_CONFIG1_DDR_FREQ_LIMIT_EN_BMSK                                            0x2000000
#define HWIO_FEATURE_CONFIG1_DDR_FREQ_LIMIT_EN_SHFT                                                 0x19
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_BMSK                                                  0x1000000
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_SHFT                                                       0x18
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY1_BMSK                                       0x800000
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY1_SHFT                                           0x17
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY0_BMSK                                       0x400000
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY0_SHFT                                           0x16
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_DISABLE_BMSK                                             0x200000
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_DISABLE_SHFT                                                 0x15
#define HWIO_FEATURE_CONFIG1_SPARE5_BMSK                                                        0x100000
#define HWIO_FEATURE_CONFIG1_SPARE5_SHFT                                                            0x14
#define HWIO_FEATURE_CONFIG1_SDCC5_SCM_DISABLE_BMSK                                              0x80000
#define HWIO_FEATURE_CONFIG1_SDCC5_SCM_DISABLE_SHFT                                                 0x13
#define HWIO_FEATURE_CONFIG1_CCI_FREQ_SCALE_BMSK                                                 0x40000
#define HWIO_FEATURE_CONFIG1_CCI_FREQ_SCALE_SHFT                                                    0x12
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_BMSK                                                  0x38000
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_SHFT                                                      0xf
#define HWIO_FEATURE_CONFIG1_DCC_DISABLE_BMSK                                                     0x4000
#define HWIO_FEATURE_CONFIG1_DCC_DISABLE_SHFT                                                        0xe
#define HWIO_FEATURE_CONFIG1_MSMC_LTE_UL_CA_DISABLE_BMSK                                          0x2000
#define HWIO_FEATURE_CONFIG1_MSMC_LTE_UL_CA_DISABLE_SHFT                                             0xd
#define HWIO_FEATURE_CONFIG1_MSMC_LTE_ABOVE_CAT4_DISABLE_BMSK                                     0x1000
#define HWIO_FEATURE_CONFIG1_MSMC_LTE_ABOVE_CAT4_DISABLE_SHFT                                        0xc
#define HWIO_FEATURE_CONFIG1_MSMC_MODEM_HSUPA_DC_DISABLE_BMSK                                      0x800
#define HWIO_FEATURE_CONFIG1_MSMC_MODEM_HSUPA_DC_DISABLE_SHFT                                        0xb
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_VP9D_BMSK                                               0x400
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_VP9D_SHFT                                                 0xa
#define HWIO_FEATURE_CONFIG1_SPARE2_BMSK                                                           0x200
#define HWIO_FEATURE_CONFIG1_SPARE2_SHFT                                                             0x9
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_265E_BMSK                                               0x100
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_265E_SHFT                                                 0x8
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_4KX2KD_BMSK                                              0x80
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_4KX2KD_SHFT                                               0x7
#define HWIO_FEATURE_CONFIG1_CSID_2_DISABLE_BMSK                                                    0x40
#define HWIO_FEATURE_CONFIG1_CSID_2_DISABLE_SHFT                                                     0x6
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_BMSK                                                 0x20
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_SHFT                                                  0x5
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_BMSK                                               0x18
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_SHFT                                                0x3
#define HWIO_FEATURE_CONFIG1_SPARE1_BMSK                                                             0x6
#define HWIO_FEATURE_CONFIG1_SPARE1_SHFT                                                             0x1
#define HWIO_FEATURE_CONFIG1_APPS_L2_CACHE_UPPER_BANK_DISABLE_BMSK                                   0x1
#define HWIO_FEATURE_CONFIG1_APPS_L2_CACHE_UPPER_BANK_DISABLE_SHFT                                   0x0

#define HWIO_FEATURE_CONFIG2_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, HWIO_FEATURE_CONFIG2_RMSK)
#define HWIO_FEATURE_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, m)
#define HWIO_FEATURE_CONFIG2_SPARE1_BMSK                                                      0xfc000000
#define HWIO_FEATURE_CONFIG2_SPARE1_SHFT                                                            0x1a
#define HWIO_FEATURE_CONFIG2_CAMCC_VFE_1_EFUSE_LIM_RES_8MP_BMSK                                0x2000000
#define HWIO_FEATURE_CONFIG2_CAMCC_VFE_1_EFUSE_LIM_RES_8MP_SHFT                                     0x19
#define HWIO_FEATURE_CONFIG2_CAMCC_VFE_0_EFUSE_LIM_RES_8MP_BMSK                                0x1000000
#define HWIO_FEATURE_CONFIG2_CAMCC_VFE_0_EFUSE_LIM_RES_8MP_SHFT                                     0x18
#define HWIO_FEATURE_CONFIG2_QC_A5X_ISDB_DBGEN_DISABLE_BMSK                                     0x800000
#define HWIO_FEATURE_CONFIG2_QC_A5X_ISDB_DBGEN_DISABLE_SHFT                                         0x17
#define HWIO_FEATURE_CONFIG2_QC_LPASS_NIDEN_DISABLE_BMSK                                        0x400000
#define HWIO_FEATURE_CONFIG2_QC_LPASS_NIDEN_DISABLE_SHFT                                            0x16
#define HWIO_FEATURE_CONFIG2_QC_LPASS_DBGEN_DISABLE_BMSK                                        0x200000
#define HWIO_FEATURE_CONFIG2_QC_LPASS_DBGEN_DISABLE_SHFT                                            0x15
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
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_BMSK                                      0xfe000000
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_SHFT                                            0x19
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_BMSK                                     0x1000000
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_SHFT                                          0x18
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_BMSK                                             0xfff800
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_SHFT                                                  0xb
#define HWIO_FEATURE_CONFIG3_SPARE1_BMSK                                                           0x400
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
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_7_BMSK                                      0x80000000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_7_SHFT                                            0x1f
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_6_BMSK                                      0x40000000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_6_SHFT                                            0x1e
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_5_BMSK                                      0x20000000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_5_SHFT                                            0x1d
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_4_BMSK                                      0x10000000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_4_SHFT                                            0x1c
#define HWIO_FEATURE_CONFIG5_GCC_DISABLE_BOOT_FSM_BMSK                                         0x8000000
#define HWIO_FEATURE_CONFIG5_GCC_DISABLE_BOOT_FSM_SHFT                                              0x1b
#define HWIO_FEATURE_CONFIG5_MODEM_PBL_PLL_CTRL_BMSK                                           0x7800000
#define HWIO_FEATURE_CONFIG5_MODEM_PBL_PLL_CTRL_SHFT                                                0x17
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_BMSK                                             0x780000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_SHFT                                                 0x13
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_3_BMSK                                         0x40000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_3_SHFT                                            0x12
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_2_BMSK                                         0x20000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_2_SHFT                                            0x11
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_1_BMSK                                         0x10000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_1_SHFT                                            0x10
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FROM_CSR_BMSK                                              0x8000
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FROM_CSR_SHFT                                                 0xf
#define HWIO_FEATURE_CONFIG5_APPS_AARCH64_ENABLE_BMSK                                             0x4000
#define HWIO_FEATURE_CONFIG5_APPS_AARCH64_ENABLE_SHFT                                                0xe
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
#define HWIO_OEM_CONFIG0_RSVD0_BMSK                                                           0xc0000000
#define HWIO_OEM_CONFIG0_RSVD0_SHFT                                                                 0x1e
#define HWIO_OEM_CONFIG0_IMAGE_ENCRYPTION_ENABLE_BMSK                                         0x20000000
#define HWIO_OEM_CONFIG0_IMAGE_ENCRYPTION_ENABLE_SHFT                                               0x1d
#define HWIO_OEM_CONFIG0_DEBUG_DISABLE_POLICY_BMSK                                            0x10000000
#define HWIO_OEM_CONFIG0_DEBUG_DISABLE_POLICY_SHFT                                                  0x1c
#define HWIO_OEM_CONFIG0_SPARE_REG26_SECURE_BMSK                                               0x8000000
#define HWIO_OEM_CONFIG0_SPARE_REG26_SECURE_SHFT                                                    0x1b
#define HWIO_OEM_CONFIG0_SPARE_REG25_SECURE_BMSK                                               0x4000000
#define HWIO_OEM_CONFIG0_SPARE_REG25_SECURE_SHFT                                                    0x1a
#define HWIO_OEM_CONFIG0_SPARE_REG24_SECURE_BMSK                                               0x2000000
#define HWIO_OEM_CONFIG0_SPARE_REG24_SECURE_SHFT                                                    0x19
#define HWIO_OEM_CONFIG0_SPARE_REG23_SECURE_BMSK                                               0x1000000
#define HWIO_OEM_CONFIG0_SPARE_REG23_SECURE_SHFT                                                    0x18
#define HWIO_OEM_CONFIG0_SPARE_REG22_SECURE_BMSK                                                0x800000
#define HWIO_OEM_CONFIG0_SPARE_REG22_SECURE_SHFT                                                    0x17
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_BMSK                                                   0x400000
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_SHFT                                                       0x16
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_BMSK                                               0x3c0000
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_SHFT                                                   0x12
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_BMSK                                                 0x20000
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_SHFT                                                    0x11
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_BMSK                                                 0x10000
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_SHFT                                                    0x10
#define HWIO_OEM_CONFIG0_SPARE1_BMSK                                                              0x8000
#define HWIO_OEM_CONFIG0_SPARE1_SHFT                                                                 0xf
#define HWIO_OEM_CONFIG0_SPARE_REG21_SECURE_BMSK                                                  0x4000
#define HWIO_OEM_CONFIG0_SPARE_REG21_SECURE_SHFT                                                     0xe
#define HWIO_OEM_CONFIG0_SPARE_REG20_SECURE_BMSK                                                  0x2000
#define HWIO_OEM_CONFIG0_SPARE_REG20_SECURE_SHFT                                                     0xd
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
#define HWIO_OEM_CONFIG1_SPARE1_BMSK                                                          0xf8000000
#define HWIO_OEM_CONFIG1_SPARE1_SHFT                                                                0x1b
#define HWIO_OEM_CONFIG1_SDCC5_SCM_FORCE_EFUSE_KEY_BMSK                                        0x4000000
#define HWIO_OEM_CONFIG1_SDCC5_SCM_FORCE_EFUSE_KEY_SHFT                                             0x1a
#define HWIO_OEM_CONFIG1_A5X_ISDB_DBGEN_DISABLE_BMSK                                           0x2000000
#define HWIO_OEM_CONFIG1_A5X_ISDB_DBGEN_DISABLE_SHFT                                                0x19
#define HWIO_OEM_CONFIG1_LPASS_NIDEN_DISABLE_BMSK                                              0x1000000
#define HWIO_OEM_CONFIG1_LPASS_NIDEN_DISABLE_SHFT                                                   0x18
#define HWIO_OEM_CONFIG1_LPASS_DBGEN_DISABLE_BMSK                                               0x800000
#define HWIO_OEM_CONFIG1_LPASS_DBGEN_DISABLE_SHFT                                                   0x17
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
#define HWIO_BOOT_CONFIG_APPS_AARCH64_ENABLE_BMSK                                                  0x200
#define HWIO_BOOT_CONFIG_APPS_AARCH64_ENABLE_SHFT                                                    0x9
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_BMSK                                                    0x100
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_SHFT                                                      0x8
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_BMSK                                                   0xc0
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_SHFT                                                    0x6
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_BMSK                                                    0x20
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_SHFT                                                     0x5
#define HWIO_BOOT_CONFIG_RSVD_BMSK                                                                  0x10
#define HWIO_BOOT_CONFIG_RSVD_SHFT                                                                   0x4
#define HWIO_BOOT_CONFIG_FAST_BOOT_BMSK                                                              0xe
#define HWIO_BOOT_CONFIG_FAST_BOOT_SHFT                                                              0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_BMSK                                                                0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_SHFT                                                                0x0

#define HWIO_SECURE_BOOTn_ADDR(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006030 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_RMSK                                                                     0x1ff
#define HWIO_SECURE_BOOTn_MAXn                                                                         7
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

#define HWIO_OVERRIDE_0_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006058)
#define HWIO_OVERRIDE_0_RMSK                                                                  0x1fffffff
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
#define HWIO_OVERRIDE_0_RSVD_27_2_BMSK                                                         0xffffffc
#define HWIO_OVERRIDE_0_RSVD_27_2_SHFT                                                               0x2
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
#define HWIO_OVERRIDE_2_RMSK                                                                   0x1fffc00
#define HWIO_OVERRIDE_2_IN          \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, HWIO_OVERRIDE_2_RMSK)
#define HWIO_OVERRIDE_2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, m)
#define HWIO_OVERRIDE_2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_2_ADDR,v)
#define HWIO_OVERRIDE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_2_ADDR,m,v,HWIO_OVERRIDE_2_IN)
#define HWIO_OVERRIDE_2_OVRID_A5X_ISDB_DBGEN_DISABLE_BMSK                                      0x1000000
#define HWIO_OVERRIDE_2_OVRID_A5X_ISDB_DBGEN_DISABLE_SHFT                                           0x18
#define HWIO_OVERRIDE_2_OVRID_LPASS_NIDEN_DISABLE_BMSK                                          0x800000
#define HWIO_OVERRIDE_2_OVRID_LPASS_NIDEN_DISABLE_SHFT                                              0x17
#define HWIO_OVERRIDE_2_OVRID_LPASS_DBGEN_DISABLE_BMSK                                          0x400000
#define HWIO_OVERRIDE_2_OVRID_LPASS_DBGEN_DISABLE_SHFT                                              0x16
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
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_FORCE_MSA_AUTH_EN_BMSK                                0x4000
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_FORCE_MSA_AUTH_EN_SHFT                                   0xe
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_AUTH_EN_BMSK                                       0x2000
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_AUTH_EN_SHFT                                          0xd
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_PK_HASH_IN_FUSE_BMSK                               0x1000
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_PK_HASH_IN_FUSE_SHFT                                  0xc
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_MSA_AUTH_EN_BMSK                                       0x800
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_MSA_AUTH_EN_SHFT                                         0xb
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_MSA_PK_HASH_IN_FUSE_BMSK                               0x400
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_MSA_PK_HASH_IN_FUSE_SHFT                                 0xa
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_ALL_USE_SERIAL_NUM_BMSK                                0x200
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_ALL_USE_SERIAL_NUM_SHFT                                  0x9
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_PK_HASH_INDEX_SRC_BMSK                                 0x100
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_PK_HASH_INDEX_SRC_SHFT                                   0x8
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_PBL_BOOT_SPEED_BMSK                                0xc0
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_PBL_BOOT_SPEED_SHFT                                 0x6
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_BOOT_FROM_ROM_BMSK                                 0x20
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_BOOT_FROM_ROM_SHFT                                  0x5
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_AARCH64_ENABLE_BMSK                                0x10
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_AARCH64_ENABLE_SHFT                                 0x4
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_FAST_BOOT_BMSK                                           0xe
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_FAST_BOOT_SHFT                                           0x1
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_WDOG_DISABLE_BMSK                                        0x1
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_WDOG_DISABLE_SHFT                                        0x0

#define HWIO_APP_PROC_CFG_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006070)
#define HWIO_APP_PROC_CFG_RMSK                                                                 0x1ffffff
#define HWIO_APP_PROC_CFG_IN          \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, HWIO_APP_PROC_CFG_RMSK)
#define HWIO_APP_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, m)
#define HWIO_APP_PROC_CFG_OUT(v)      \
        out_dword(HWIO_APP_PROC_CFG_ADDR,v)
#define HWIO_APP_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APP_PROC_CFG_ADDR,m,v,HWIO_APP_PROC_CFG_IN)
#define HWIO_APP_PROC_CFG_LPASS_DBG_NIDEN_BMSK                                                 0x1000000
#define HWIO_APP_PROC_CFG_LPASS_DBG_NIDEN_SHFT                                                      0x18
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

#define HWIO_OVERRIDE_5_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000608c)
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

#define HWIO_OVERRIDE_6_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006090)
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

#define HWIO_PTE0_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060a0)
#define HWIO_PTE0_RMSK                                                                        0xffffffff
#define HWIO_PTE0_IN          \
        in_dword_masked(HWIO_PTE0_ADDR, HWIO_PTE0_RMSK)
#define HWIO_PTE0_INM(m)      \
        in_dword_masked(HWIO_PTE0_ADDR, m)
#define HWIO_PTE0_SPARE0_BMSK                                                                 0xe0000000
#define HWIO_PTE0_SPARE0_SHFT                                                                       0x1d
#define HWIO_PTE0_MACCHIATO_EN_BMSK                                                           0x10000000
#define HWIO_PTE0_MACCHIATO_EN_SHFT                                                                 0x1c
#define HWIO_PTE0_FEATURE_ID_BMSK                                                              0xff00000
#define HWIO_PTE0_FEATURE_ID_SHFT                                                                   0x14
#define HWIO_PTE0_JTAG_ID_BMSK                                                                   0xfffff
#define HWIO_PTE0_JTAG_ID_SHFT                                                                       0x0

#define HWIO_PTE1_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060a4)
#define HWIO_PTE1_RMSK                                                                        0xffffffff
#define HWIO_PTE1_IN          \
        in_dword_masked(HWIO_PTE1_ADDR, HWIO_PTE1_RMSK)
#define HWIO_PTE1_INM(m)      \
        in_dword_masked(HWIO_PTE1_ADDR, m)
#define HWIO_PTE1_IDDQ_MSS_ACTIVE_MEAS_FUSE_BITS_BMSK                                         0xfc000000
#define HWIO_PTE1_IDDQ_MSS_ACTIVE_MEAS_FUSE_BITS_SHFT                                               0x1a
#define HWIO_PTE1_PROCESS_NODE_ID_BMSK                                                         0x2000000
#define HWIO_PTE1_PROCESS_NODE_ID_SHFT                                                              0x19
#define HWIO_PTE1_SPARE2_BMSK                                                                  0x1800000
#define HWIO_PTE1_SPARE2_SHFT                                                                       0x17
#define HWIO_PTE1_BONE_PILE_BMSK                                                                0x400000
#define HWIO_PTE1_BONE_PILE_SHFT                                                                    0x16
#define HWIO_PTE1_A53_ACC_SETTINGS_ID_BMSK                                                      0x300000
#define HWIO_PTE1_A53_ACC_SETTINGS_ID_SHFT                                                          0x14
#define HWIO_PTE1_SPARE1_BMSK                                                                    0xfe000
#define HWIO_PTE1_SPARE1_SHFT                                                                        0xd
#define HWIO_PTE1_WAFER_BIN_BMSK                                                                  0x1c00
#define HWIO_PTE1_WAFER_BIN_SHFT                                                                     0xa
#define HWIO_PTE1_METAL_REV_BMSK                                                                   0x300
#define HWIO_PTE1_METAL_REV_SHFT                                                                     0x8
#define HWIO_PTE1_SPARE0_BMSK                                                                       0xff
#define HWIO_PTE1_SPARE0_SHFT                                                                        0x0

#define HWIO_SERIAL_NUM_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060a8)
#define HWIO_SERIAL_NUM_RMSK                                                                  0xffffffff
#define HWIO_SERIAL_NUM_IN          \
        in_dword_masked(HWIO_SERIAL_NUM_ADDR, HWIO_SERIAL_NUM_RMSK)
#define HWIO_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_SERIAL_NUM_ADDR, m)
#define HWIO_SERIAL_NUM_SERIAL_NUM_BMSK                                                       0xffffffff
#define HWIO_SERIAL_NUM_SERIAL_NUM_SHFT                                                              0x0

#define HWIO_PTE2_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060ac)
#define HWIO_PTE2_RMSK                                                                        0xffffffff
#define HWIO_PTE2_IN          \
        in_dword_masked(HWIO_PTE2_ADDR, HWIO_PTE2_RMSK)
#define HWIO_PTE2_INM(m)      \
        in_dword_masked(HWIO_PTE2_ADDR, m)
#define HWIO_PTE2_WAFER_ID_BMSK                                                               0xf8000000
#define HWIO_PTE2_WAFER_ID_SHFT                                                                     0x1b
#define HWIO_PTE2_DIE_X_BMSK                                                                   0x7f80000
#define HWIO_PTE2_DIE_X_SHFT                                                                        0x13
#define HWIO_PTE2_DIE_Y_BMSK                                                                     0x7f800
#define HWIO_PTE2_DIE_Y_SHFT                                                                         0xb
#define HWIO_PTE2_FOUNDRY_ID_BMSK                                                                  0x700
#define HWIO_PTE2_FOUNDRY_ID_SHFT                                                                    0x8
#define HWIO_PTE2_LOGIC_RETENTION_BMSK                                                              0xe0
#define HWIO_PTE2_LOGIC_RETENTION_SHFT                                                               0x5
#define HWIO_PTE2_SPEED_BIN_BMSK                                                                    0x1c
#define HWIO_PTE2_SPEED_BIN_SHFT                                                                     0x2
#define HWIO_PTE2_MX_RET_BIN_BMSK                                                                    0x3
#define HWIO_PTE2_MX_RET_BIN_SHFT                                                                    0x0

#define HWIO_ANTI_ROLLBACK_1_0_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060b0)
#define HWIO_ANTI_ROLLBACK_1_0_RMSK                                                           0xffffffff
#define HWIO_ANTI_ROLLBACK_1_0_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_0_ADDR, HWIO_ANTI_ROLLBACK_1_0_RMSK)
#define HWIO_ANTI_ROLLBACK_1_0_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_0_ADDR, m)
#define HWIO_ANTI_ROLLBACK_1_0_PIL_SUBSYSTEM0_BMSK                                            0xfc000000
#define HWIO_ANTI_ROLLBACK_1_0_PIL_SUBSYSTEM0_SHFT                                                  0x1a
#define HWIO_ANTI_ROLLBACK_1_0_TZ_BMSK                                                         0x3fff000
#define HWIO_ANTI_ROLLBACK_1_0_TZ_SHFT                                                               0xc
#define HWIO_ANTI_ROLLBACK_1_0_SBL1_BMSK                                                           0xffe
#define HWIO_ANTI_ROLLBACK_1_0_SBL1_SHFT                                                             0x1
#define HWIO_ANTI_ROLLBACK_1_0_RPMB_KEY_PROVISIONED_BMSK                                             0x1
#define HWIO_ANTI_ROLLBACK_1_0_RPMB_KEY_PROVISIONED_SHFT                                             0x0

#define HWIO_ANTI_ROLLBACK_1_1_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060b4)
#define HWIO_ANTI_ROLLBACK_1_1_RMSK                                                           0xffffffff
#define HWIO_ANTI_ROLLBACK_1_1_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_1_ADDR, HWIO_ANTI_ROLLBACK_1_1_RMSK)
#define HWIO_ANTI_ROLLBACK_1_1_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_1_ADDR, m)
#define HWIO_ANTI_ROLLBACK_1_1_APPSBL0_BMSK                                                   0xfffc0000
#define HWIO_ANTI_ROLLBACK_1_1_APPSBL0_SHFT                                                         0x12
#define HWIO_ANTI_ROLLBACK_1_1_PIL_SUBSYSTEM1_BMSK                                               0x3ffff
#define HWIO_ANTI_ROLLBACK_1_1_PIL_SUBSYSTEM1_SHFT                                                   0x0

#define HWIO_ANTI_ROLLBACK_2_0_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060b8)
#define HWIO_ANTI_ROLLBACK_2_0_RMSK                                                           0xffffffff
#define HWIO_ANTI_ROLLBACK_2_0_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_0_ADDR, HWIO_ANTI_ROLLBACK_2_0_RMSK)
#define HWIO_ANTI_ROLLBACK_2_0_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_0_ADDR, m)
#define HWIO_ANTI_ROLLBACK_2_0_APPSBL1_BMSK                                                   0xffffffff
#define HWIO_ANTI_ROLLBACK_2_0_APPSBL1_SHFT                                                          0x0

#define HWIO_ANTI_ROLLBACK_2_1_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060bc)
#define HWIO_ANTI_ROLLBACK_2_1_RMSK                                                           0xffffffff
#define HWIO_ANTI_ROLLBACK_2_1_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_1_ADDR, HWIO_ANTI_ROLLBACK_2_1_RMSK)
#define HWIO_ANTI_ROLLBACK_2_1_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_1_ADDR, m)
#define HWIO_ANTI_ROLLBACK_2_1_ROOT_CERT_PK_HASH_INDEX_BMSK                                   0xff000000
#define HWIO_ANTI_ROLLBACK_2_1_ROOT_CERT_PK_HASH_INDEX_SHFT                                         0x18
#define HWIO_ANTI_ROLLBACK_2_1_HYPERVISOR_BMSK                                                  0xfff000
#define HWIO_ANTI_ROLLBACK_2_1_HYPERVISOR_SHFT                                                       0xc
#define HWIO_ANTI_ROLLBACK_2_1_RPM_BMSK                                                            0xff0
#define HWIO_ANTI_ROLLBACK_2_1_RPM_SHFT                                                              0x4
#define HWIO_ANTI_ROLLBACK_2_1_APPSBL2_BMSK                                                          0xf
#define HWIO_ANTI_ROLLBACK_2_1_APPSBL2_SHFT                                                          0x0

#define HWIO_ANTI_ROLLBACK_3_0_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c0)
#define HWIO_ANTI_ROLLBACK_3_0_RMSK                                                           0xffffffff
#define HWIO_ANTI_ROLLBACK_3_0_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_0_ADDR, HWIO_ANTI_ROLLBACK_3_0_RMSK)
#define HWIO_ANTI_ROLLBACK_3_0_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_0_ADDR, m)
#define HWIO_ANTI_ROLLBACK_3_0_MSS_BMSK                                                       0xffff0000
#define HWIO_ANTI_ROLLBACK_3_0_MSS_SHFT                                                             0x10
#define HWIO_ANTI_ROLLBACK_3_0_MBA_BMSK                                                           0xffff
#define HWIO_ANTI_ROLLBACK_3_0_MBA_SHFT                                                              0x0

#define HWIO_ANTI_ROLLBACK_3_1_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c4)
#define HWIO_ANTI_ROLLBACK_3_1_RMSK                                                           0xffffffff
#define HWIO_ANTI_ROLLBACK_3_1_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_1_ADDR, HWIO_ANTI_ROLLBACK_3_1_RMSK)
#define HWIO_ANTI_ROLLBACK_3_1_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_1_ADDR, m)
#define HWIO_ANTI_ROLLBACK_3_1_SPARE0_BMSK                                                    0xffffff00
#define HWIO_ANTI_ROLLBACK_3_1_SPARE0_SHFT                                                           0x8
#define HWIO_ANTI_ROLLBACK_3_1_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                                   0xff
#define HWIO_ANTI_ROLLBACK_3_1_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                                    0x0

#define HWIO_PK_HASH_0_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c8)
#define HWIO_PK_HASH_0_RMSK                                                                   0xffffffff
#define HWIO_PK_HASH_0_IN          \
        in_dword_masked(HWIO_PK_HASH_0_ADDR, HWIO_PK_HASH_0_RMSK)
#define HWIO_PK_HASH_0_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_0_ADDR, m)
#define HWIO_PK_HASH_0_HASH_DATA0_BMSK                                                        0xffffffff
#define HWIO_PK_HASH_0_HASH_DATA0_SHFT                                                               0x0

#define HWIO_PK_HASH_1_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060cc)
#define HWIO_PK_HASH_1_RMSK                                                                   0xffffffff
#define HWIO_PK_HASH_1_IN          \
        in_dword_masked(HWIO_PK_HASH_1_ADDR, HWIO_PK_HASH_1_RMSK)
#define HWIO_PK_HASH_1_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_1_ADDR, m)
#define HWIO_PK_HASH_1_HASH_DATA0_BMSK                                                        0xffffffff
#define HWIO_PK_HASH_1_HASH_DATA0_SHFT                                                               0x0

#define HWIO_PK_HASH_2_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d0)
#define HWIO_PK_HASH_2_RMSK                                                                   0xffffffff
#define HWIO_PK_HASH_2_IN          \
        in_dword_masked(HWIO_PK_HASH_2_ADDR, HWIO_PK_HASH_2_RMSK)
#define HWIO_PK_HASH_2_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_2_ADDR, m)
#define HWIO_PK_HASH_2_HASH_DATA0_BMSK                                                        0xffffffff
#define HWIO_PK_HASH_2_HASH_DATA0_SHFT                                                               0x0

#define HWIO_PK_HASH_3_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d4)
#define HWIO_PK_HASH_3_RMSK                                                                   0xffffffff
#define HWIO_PK_HASH_3_IN          \
        in_dword_masked(HWIO_PK_HASH_3_ADDR, HWIO_PK_HASH_3_RMSK)
#define HWIO_PK_HASH_3_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_3_ADDR, m)
#define HWIO_PK_HASH_3_HASH_DATA0_BMSK                                                        0xffffffff
#define HWIO_PK_HASH_3_HASH_DATA0_SHFT                                                               0x0

#define HWIO_PK_HASH_4_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d8)
#define HWIO_PK_HASH_4_RMSK                                                                   0xffffffff
#define HWIO_PK_HASH_4_IN          \
        in_dword_masked(HWIO_PK_HASH_4_ADDR, HWIO_PK_HASH_4_RMSK)
#define HWIO_PK_HASH_4_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_4_ADDR, m)
#define HWIO_PK_HASH_4_HASH_DATA0_BMSK                                                        0xffffffff
#define HWIO_PK_HASH_4_HASH_DATA0_SHFT                                                               0x0

#define HWIO_PK_HASH_5_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060dc)
#define HWIO_PK_HASH_5_RMSK                                                                   0xffffffff
#define HWIO_PK_HASH_5_IN          \
        in_dword_masked(HWIO_PK_HASH_5_ADDR, HWIO_PK_HASH_5_RMSK)
#define HWIO_PK_HASH_5_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_5_ADDR, m)
#define HWIO_PK_HASH_5_HASH_DATA0_BMSK                                                        0xffffffff
#define HWIO_PK_HASH_5_HASH_DATA0_SHFT                                                               0x0

#define HWIO_PK_HASH_6_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e0)
#define HWIO_PK_HASH_6_RMSK                                                                   0xffffffff
#define HWIO_PK_HASH_6_IN          \
        in_dword_masked(HWIO_PK_HASH_6_ADDR, HWIO_PK_HASH_6_RMSK)
#define HWIO_PK_HASH_6_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_6_ADDR, m)
#define HWIO_PK_HASH_6_HASH_DATA0_BMSK                                                        0xffffffff
#define HWIO_PK_HASH_6_HASH_DATA0_SHFT                                                               0x0

#define HWIO_PK_HASH_7_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e4)
#define HWIO_PK_HASH_7_RMSK                                                                   0xffffffff
#define HWIO_PK_HASH_7_IN          \
        in_dword_masked(HWIO_PK_HASH_7_ADDR, HWIO_PK_HASH_7_RMSK)
#define HWIO_PK_HASH_7_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_7_ADDR, m)
#define HWIO_PK_HASH_7_HASH_DATA0_BMSK                                                        0xffffffff
#define HWIO_PK_HASH_7_HASH_DATA0_SHFT                                                               0x0

#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006100)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_RMSK                                            0xffffffff
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_IN          \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_ADDR, HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_RMSK)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_INM(m)      \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_ADDR, m)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_OUT(v)      \
        out_dword(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_ADDR,v)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_ADDR,m,v,HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_IN)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION31_STICKY_BIT_BMSK                        0x80000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION31_STICKY_BIT_SHFT                              0x1f
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION30_STICKY_BIT_BMSK                        0x40000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION30_STICKY_BIT_SHFT                              0x1e
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION29_STICKY_BIT_BMSK                        0x20000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION29_STICKY_BIT_SHFT                              0x1d
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION28_STICKY_BIT_BMSK                        0x10000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION28_STICKY_BIT_SHFT                              0x1c
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION27_STICKY_BIT_BMSK                         0x8000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION27_STICKY_BIT_SHFT                              0x1b
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION26_STICKY_BIT_BMSK                         0x4000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION26_STICKY_BIT_SHFT                              0x1a
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION25_STICKY_BIT_BMSK                         0x2000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION25_STICKY_BIT_SHFT                              0x19
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION24_STICKY_BIT_BMSK                         0x1000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION24_STICKY_BIT_SHFT                              0x18
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION23_STICKY_BIT_BMSK                          0x800000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION23_STICKY_BIT_SHFT                              0x17
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION22_STICKY_BIT_BMSK                          0x400000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION22_STICKY_BIT_SHFT                              0x16
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION21_STICKY_BIT_BMSK                          0x200000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION21_STICKY_BIT_SHFT                              0x15
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION20_STICKY_BIT_BMSK                          0x100000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION20_STICKY_BIT_SHFT                              0x14
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION19_STICKY_BIT_BMSK                           0x80000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION19_STICKY_BIT_SHFT                              0x13
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION18_STICKY_BIT_BMSK                           0x40000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION18_STICKY_BIT_SHFT                              0x12
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION17_STICKY_BIT_BMSK                           0x20000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION17_STICKY_BIT_SHFT                              0x11
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION16_STICKY_BIT_BMSK                           0x10000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION16_STICKY_BIT_SHFT                              0x10
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION15_STICKY_BIT_BMSK                            0x8000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION15_STICKY_BIT_SHFT                               0xf
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION14_STICKY_BIT_BMSK                            0x4000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION14_STICKY_BIT_SHFT                               0xe
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION13_STICKY_BIT_BMSK                            0x2000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION13_STICKY_BIT_SHFT                               0xd
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION12_STICKY_BIT_BMSK                            0x1000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION12_STICKY_BIT_SHFT                               0xc
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION11_STICKY_BIT_BMSK                             0x800
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION11_STICKY_BIT_SHFT                               0xb
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION10_STICKY_BIT_BMSK                             0x400
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION10_STICKY_BIT_SHFT                               0xa
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION9_STICKY_BIT_BMSK                              0x200
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION9_STICKY_BIT_SHFT                                0x9
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION8_STICKY_BIT_BMSK                              0x100
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION8_STICKY_BIT_SHFT                                0x8
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION7_STICKY_BIT_BMSK                               0x80
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION7_STICKY_BIT_SHFT                                0x7
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION6_STICKY_BIT_BMSK                               0x40
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION6_STICKY_BIT_SHFT                                0x6
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION5_STICKY_BIT_BMSK                               0x20
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION5_STICKY_BIT_SHFT                                0x5
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION4_STICKY_BIT_BMSK                               0x10
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION4_STICKY_BIT_SHFT                                0x4
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION3_STICKY_BIT_BMSK                                0x8
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION3_STICKY_BIT_SHFT                                0x3
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION2_STICKY_BIT_BMSK                                0x4
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION2_STICKY_BIT_SHFT                                0x2
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION1_STICKY_BIT_BMSK                                0x2
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION1_STICKY_BIT_SHFT                                0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION0_STICKY_BIT_BMSK                                0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION0_STICKY_BIT_SHFT                                0x0

#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006104)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_RMSK                                            0xffffffff
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_IN          \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_ADDR, HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_RMSK)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_ADDR, m)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_OUT(v)      \
        out_dword(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_ADDR,v)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_ADDR,m,v,HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_IN)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_RSVD0_BMSK                                      0xffffe000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_RSVD0_SHFT                                             0xd
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION44_STICKY_BIT_BMSK                            0x1000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION44_STICKY_BIT_SHFT                               0xc
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION43_STICKY_BIT_BMSK                             0x800
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION43_STICKY_BIT_SHFT                               0xb
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION42_STICKY_BIT_BMSK                             0x400
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION42_STICKY_BIT_SHFT                               0xa
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION41_STICKY_BIT_BMSK                             0x200
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION41_STICKY_BIT_SHFT                               0x9
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION40_STICKY_BIT_BMSK                             0x100
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION40_STICKY_BIT_SHFT                               0x8
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION39_STICKY_BIT_BMSK                              0x80
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION39_STICKY_BIT_SHFT                               0x7
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION38_STICKY_BIT_BMSK                              0x40
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION38_STICKY_BIT_SHFT                               0x6
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION37_STICKY_BIT_BMSK                              0x20
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION37_STICKY_BIT_SHFT                               0x5
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION36_STICKY_BIT_BMSK                              0x10
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION36_STICKY_BIT_SHFT                               0x4
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION35_STICKY_BIT_BMSK                               0x8
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION35_STICKY_BIT_SHFT                               0x3
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION34_STICKY_BIT_BMSK                               0x4
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION34_STICKY_BIT_SHFT                               0x2
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION33_STICKY_BIT_BMSK                               0x2
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION33_STICKY_BIT_SHFT                               0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION32_STICKY_BIT_BMSK                               0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_REGION32_STICKY_BIT_SHFT                               0x0


#endif /* __UIEHWIOREG_H__ */
