#ifndef __SIMLOCK_HWIO_MACROS_8909_H__
#define __SIMLOCK_HWIO_MACROS_8909_H__
/*
===========================================================================
*/
/**
  @file simlock_hwio_macros_8909.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8909/08 (Cheel) [cheel_v1.0_p3q3r17.3_MTO]
 
  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE
    TCSR_TCSR_MUTEX

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/hw/simlock_hwio_macros_8909.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                        (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_PHYS                                                   (SECURITY_CONTROL_BASE_PHYS + 0x00000000)

#define HWIO_QFPROM_RAW_JTAG_ID_ADDR                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000000)
#define HWIO_QFPROM_RAW_JTAG_ID_PHYS                                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000000)
#define HWIO_QFPROM_RAW_JTAG_ID_RMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_JTAG_ID_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_ADDR, HWIO_QFPROM_RAW_JTAG_ID_RMSK)
#define HWIO_QFPROM_RAW_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_ADDR, m)
#define HWIO_QFPROM_RAW_JTAG_ID_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_JTAG_ID_ADDR,v)
#define HWIO_QFPROM_RAW_JTAG_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_JTAG_ID_ADDR,m,v,HWIO_QFPROM_RAW_JTAG_ID_IN)
#define HWIO_QFPROM_RAW_JTAG_ID_PTE_DATA0_BMSK                                                0xe0000000
#define HWIO_QFPROM_RAW_JTAG_ID_PTE_DATA0_SHFT                                                      0x1d
#define HWIO_QFPROM_RAW_JTAG_ID_MACCHIATO_EN_BMSK                                             0x10000000
#define HWIO_QFPROM_RAW_JTAG_ID_MACCHIATO_EN_SHFT                                                   0x1c
#define HWIO_QFPROM_RAW_JTAG_ID_FEATURE_ID_BMSK                                                0xff00000
#define HWIO_QFPROM_RAW_JTAG_ID_FEATURE_ID_SHFT                                                     0x14
#define HWIO_QFPROM_RAW_JTAG_ID_JTAG_ID_BMSK                                                     0xfffff
#define HWIO_QFPROM_RAW_JTAG_ID_JTAG_ID_SHFT                                                         0x0

#define HWIO_QFPROM_RAW_PTE1_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000004)
#define HWIO_QFPROM_RAW_PTE1_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000004)
#define HWIO_QFPROM_RAW_PTE1_RMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_PTE1_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE1_ADDR, HWIO_QFPROM_RAW_PTE1_RMSK)
#define HWIO_QFPROM_RAW_PTE1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE1_ADDR, m)
#define HWIO_QFPROM_RAW_PTE1_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE1_ADDR,v)
#define HWIO_QFPROM_RAW_PTE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE1_ADDR,m,v,HWIO_QFPROM_RAW_PTE1_IN)
#define HWIO_QFPROM_RAW_PTE1_SPARE1_BMSK                                                      0xfc000000
#define HWIO_QFPROM_RAW_PTE1_SPARE1_SHFT                                                            0x1a
#define HWIO_QFPROM_RAW_PTE1_PROCESS_NODE_ID_BMSK                                              0x2000000
#define HWIO_QFPROM_RAW_PTE1_PROCESS_NODE_ID_SHFT                                                   0x19
#define HWIO_QFPROM_RAW_PTE1_TIER_BMSK                                                         0x1c00000
#define HWIO_QFPROM_RAW_PTE1_TIER_SHFT                                                              0x16
#define HWIO_QFPROM_RAW_PTE1_BONE_PILE_BMSK                                                     0x200000
#define HWIO_QFPROM_RAW_PTE1_BONE_PILE_SHFT                                                         0x15
#define HWIO_QFPROM_RAW_PTE1_ACC_SETTINGS_ID_BMSK                                               0x100000
#define HWIO_QFPROM_RAW_PTE1_ACC_SETTINGS_ID_SHFT                                                   0x14
#define HWIO_QFPROM_RAW_PTE1_WAFER_MR_BMSK                                                       0x80000
#define HWIO_QFPROM_RAW_PTE1_WAFER_MR_SHFT                                                          0x13
#define HWIO_QFPROM_RAW_PTE1_Q6_MAX_FREQ_BMSK                                                    0x70000
#define HWIO_QFPROM_RAW_PTE1_Q6_MAX_FREQ_SHFT                                                       0x10
#define HWIO_QFPROM_RAW_PTE1_PTE_DATA1_BMSK                                                       0xffff
#define HWIO_QFPROM_RAW_PTE1_PTE_DATA1_SHFT                                                          0x0

#define HWIO_QFPROM_RAW_SERIAL_NUM_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000008)
#define HWIO_QFPROM_RAW_SERIAL_NUM_PHYS                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000008)
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

#define HWIO_QFPROM_RAW_PTE2_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000000c)
#define HWIO_QFPROM_RAW_PTE2_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000000c)
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

#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000010)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PHYS                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000010)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE_BMSK                                             0xfff00000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE_SHFT                                                   0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_BMSK                                              0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE18_BMSK                                              0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE18_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE17_BMSK                                              0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE17_SHFT                                                 0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE16_BMSK                                              0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE16_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE15_BMSK                                               0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE15_SHFT                                                  0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                        0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                           0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_CONFIG_BMSK                                            0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_CONFIG_SHFT                                               0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_BMSK                                                 0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_SHFT                                                    0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PK_HASH_BMSK                                                0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PK_HASH_SHFT                                                  0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB2_BMSK                                                 0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB2_SHFT                                                   0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                           0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                             0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                 0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                   0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                  0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                   0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                             0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                              0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                              0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                               0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                         0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                          0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                          0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                          0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                          0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                          0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                               0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                               0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_BMSK                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_SHFT                                                      0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000014)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PHYS                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000014)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE_BMSK                                             0xfff00000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE_SHFT                                                   0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_BMSK                                              0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE18_BMSK                                              0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE18_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE17_BMSK                                              0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE17_SHFT                                                 0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE16_BMSK                                              0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE16_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE15_BMSK                                               0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE15_SHFT                                                  0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                        0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                           0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_CONFIG_BMSK                                            0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_CONFIG_SHFT                                               0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_BMSK                                                 0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_SHFT                                                    0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PK_HASH_BMSK                                                0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PK_HASH_SHFT                                                  0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB2_BMSK                                                 0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB2_SHFT                                                   0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                           0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                             0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                 0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                   0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                  0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                   0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                             0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                              0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                              0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                               0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_BMSK                                         0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_SHFT                                          0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_BMSK                                          0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_SHFT                                          0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_BMSK                                          0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_SHFT                                          0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                               0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                               0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_BMSK                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_SHFT                                                      0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000018)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000018)
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

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000001c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000001c)
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

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000020)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000020)
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

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000024)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000024)
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

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000028)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000028)
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

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000002c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000002c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_SPARE0_BMSK                                       0x7fffff00
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_SPARE0_SHFT                                              0x8
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_SIMLOCK_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_SIMLOCK_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                      0xff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                       0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000030)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000030)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_BMSK                                       0xff800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_SHFT                                             0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                                0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                    0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                            0x3c0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                                0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_BMSK                              0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_SHFT                                 0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_BMSK                              0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_SHFT                                 0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_BMSK                               0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_SHFT                                  0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_BMSK                               0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_SHFT                                  0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG15_SECURE_BMSK                               0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG15_SECURE_SHFT                                  0xd
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

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000034)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000034)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                       0xff800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                             0x17
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

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000038)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000038)
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

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000003c)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000003c)
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

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000040)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000040)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CAMERA_RES_LIMIT_5MP_BMSK                        0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CAMERA_RES_LIMIT_5MP_SHFT                              0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_30_BMSK                                    0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_30_SHFT                                          0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MDP_PANEL_RES_LIMIT_BMSK                         0x30000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MDP_PANEL_RES_LIMIT_SHFT                               0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_BMSK                                0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_SHFT                                     0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_26_BMSK                                     0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_26_SHFT                                          0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ACC_DISABLE_BMSK                                  0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ACC_DISABLE_SHFT                                       0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                          0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                               0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                           0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                               0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_BMSK                              0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_SHFT                                  0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_BMSK                                   0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_SHFT                                       0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_BMSK                                   0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_SHFT                                       0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_BMSK                            0xe0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_SHFT                               0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM2_DISABLE_BMSK                             0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM2_DISABLE_SHFT                                0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM1_DISABLE_BMSK                              0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM1_DISABLE_SHFT                                 0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SMMU_DISABLE_BMSK                              0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SMMU_DISABLE_SHFT                                 0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TX_DSDA_DISABLE_BMSK                           0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TX_DSDA_DISABLE_SHFT                              0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_BMSK                             0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_SHFT                                0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_BMSK                              0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_SHFT                                0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_BMSK                             0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_SHFT                               0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_BMSK                            0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_SHFT                              0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_BMSK                          0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_SHFT                            0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_BMSK                      0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_SHFT                       0x7
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

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000044)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000044)
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
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_CAMERA_BMSK                        0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_CAMERA_SHFT                              0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_MDP_BMSK                           0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_MDP_SHFT                                 0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_VENUS_BMSK                         0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_VENUS_SHFT                               0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_OXILI_BMSK                          0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_OXILI_SHFT                               0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_PRONTO_BMSK                         0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_PRONTO_SHFT                              0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_HEVC_DECODE_BMSK                    0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_HEVC_DECODE_SHFT                         0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_720P_ENCODE_BMSK                    0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_720P_ENCODE_SHFT                         0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_720P_DECODE_BMSK                     0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_720P_DECODE_SHFT                         0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_1080P_ENCODE_BMSK                    0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_1080P_ENCODE_SHFT                        0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_1080P_DECODE_BMSK                    0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_1080P_DECODE_SHFT                        0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE0_BMSK                                        0x1ff800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE0_SHFT                                             0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_CONFIGURATION1_BMSK                              0x600
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_CONFIGURATION1_SHFT                                0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_CONFIGURATION0_BMSK                              0x180
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_CONFIGURATION0_SHFT                                0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE_BMSK                                             0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE_SHFT                                              0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BMSK                                 0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_SHFT                                  0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_BMSK                               0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_SHFT                                0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_BMSK                                      0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_SHFT                                      0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_L2_CACHE_UPPER_BANK_DISABLE_BMSK                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_L2_CACHE_UPPER_BANK_DISABLE_SHFT                   0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000048)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000048)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                      0xffe00000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                            0x15
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

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000004c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000004c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                      0xf8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                            0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                     0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                          0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                            0x3ffc000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                  0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SPARE1_BMSK                                          0x3c00
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SPARE1_SHFT                                             0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_BMSK                          0x3e0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_SHFT                            0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                            0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                             0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000050)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000050)
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

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000054)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000054)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE4_BMSK                                      0xc0000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE4_SHFT                                            0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_BMSK                   0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_SHFT                         0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE3_BMSK                                      0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE3_SHFT                                            0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_BMSK                           0xf000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_SHFT                                0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                             0xf80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                 0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_BMSK                           0x78000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_SHFT                               0xf
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
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_WIFI_DUAL_BAND_DISABLE_BMSK                           0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_WIFI_DUAL_BAND_DISABLE_SHFT                             0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_DDR_LIMIT_MAX_FREQ_BMSK                                0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_DDR_LIMIT_MAX_FREQ_SHFT                                 0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_BMSK                               0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_SHFT                                0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_BMSK                            0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_SHFT                             0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                 0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                  0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000058 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_PHYS(n)                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000058 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000005c + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_PHYS(n)                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000005c + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000078 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_PHYS(n)                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000078 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000007c + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_PHYS(n)                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000007c + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000098)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_PHYS                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000098)
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

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000009c)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_PHYS                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000009c)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_OEM_SEC_BOOT_ROW0_FEC_EN_BMSK                   0x80000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_OEM_SEC_BOOT_ROW0_FEC_EN_SHFT                         0x1f
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_FEC_VALUE_BMSK                                  0x7f000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_FEC_VALUE_SHFT                                        0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT7_BMSK                                    0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT7_SHFT                                        0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT6_BMSK                                      0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT6_SHFT                                         0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT5_BMSK                                        0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT5_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a0)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000a0)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS2_POINT2_BIT_1_0_BMSK                            0xc0000000
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS2_POINT2_BIT_1_0_SHFT                                  0x1e
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS2_POINT1_BMSK                                    0x3f000000
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS2_POINT1_SHFT                                          0x18
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS1_POINT2_BMSK                                      0xfc0000
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS1_POINT2_SHFT                                          0x12
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS1_POINT1_BMSK                                       0x3f000
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS1_POINT1_SHFT                                           0xc
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS0_POINT2_BMSK                                         0xfc0
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS0_POINT2_SHFT                                           0x6
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS0_POINT1_BMSK                                          0x3f
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_TSENS0_POINT1_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a4)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000a4)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_SPARE3_BMSK                                           0xf0000000
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_SPARE3_SHFT                                                 0x1c
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_TSENS4_POINT2_BMSK                                     0xfc00000
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_TSENS4_POINT2_SHFT                                          0x16
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_TSENS4_POINT1_BMSK                                      0x3f0000
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_TSENS4_POINT1_SHFT                                          0x10
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_TSENS3_POINT2_BMSK                                        0xfc00
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_TSENS3_POINT2_SHFT                                           0xa
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_TSENS3_POINT1_BMSK                                         0x3f0
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_TSENS3_POINT1_SHFT                                           0x4
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_TSENS2_POINT2_BIT_5_2_BMSK                                   0xf
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_TSENS2_POINT2_BIT_5_2_SHFT                                   0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_PHYS(n)                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000a8 + 0x8 * (n))
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

#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000ac + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_PHYS(n)                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000ac + 0x8 * (n))
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
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_PK_HASH_ROW_FEC_EN_BMSK                              0x80000000
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_PK_HASH_ROW_FEC_EN_SHFT                                    0x1f
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_FEC_VALUE_BMSK                                       0x7f000000
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_FEC_VALUE_SHFT                                             0x18
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                        0xffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                             0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000c8)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000c8)
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

#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000cc)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000cc)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_RMSK                                                 0xff000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_PK_HASH_ROW_FEC_EN_BMSK                              0x80000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_PK_HASH_ROW_FEC_EN_SHFT                                    0x1f
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_FEC_VALUE_BMSK                                       0x7f000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_FEC_VALUE_SHFT                                             0x18

#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d0)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_PHYS                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000d0)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CPR1_NOMINAL_QUOT_VMIN_BIT_7_0_BMSK                    0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CPR1_NOMINAL_QUOT_VMIN_BIT_7_0_SHFT                          0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CPR1_NOMINAL_TARGET_BMSK                                 0xfc0000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CPR1_NOMINAL_TARGET_SHFT                                     0x12
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CPR1_TURBO_QUOT_VMIN_BMSK                                 0x3ffc0
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CPR1_TURBO_QUOT_VMIN_SHFT                                     0x6
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CPR1_TURBO_TARGET_BMSK                                       0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CPR1_TURBO_TARGET_SHFT                                        0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d4)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PHYS                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000d4)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_NOM_ROSEL_BMSK                                    0xe0000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_NOM_ROSEL_SHFT                                          0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR_REV_BMSK                                           0x18000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR_REV_SHFT                                                 0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_DIS_CPR_BMSK                                       0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_DIS_CPR_SHFT                                            0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_SPARE3_BMSK                                             0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_SPARE3_SHFT                                                  0x19
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_TUR_ROSEL_BMSK                                     0x1c00000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_TUR_ROSEL_SHFT                                          0x16
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_SVS_QUOT_VMIN_BMSK                                  0x3ffc00
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_SVS_QUOT_VMIN_SHFT                                       0xa
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_SVS_TARGET_BMSK                                        0x3f0
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_SVS_TARGET_SHFT                                          0x4
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_NOMINAL_QUOT_VMIN_BIT_11_8_BMSK                          0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_CPR1_NOMINAL_QUOT_VMIN_BIT_11_8_SHFT                          0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d8)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_PHYS                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000d8)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_SVS_TARGET_BMSK                                   0xf8000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_SVS_TARGET_SHFT                                         0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_NOMINAL_TARGET_BMSK                                0x7c00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_NOMINAL_TARGET_SHFT                                     0x16
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_TURBO_TARGET_BMSK                                   0x3e0000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_TURBO_TARGET_SHFT                                       0x11
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_DIS_CPR_BMSK                                         0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_DIS_CPR_SHFT                                            0x10
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_SVS_TARGET_BMSK                                       0xf800
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_SVS_TARGET_SHFT                                          0xb
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_NOMINAL_TARGET_BMSK                                    0x7c0
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_NOMINAL_TARGET_SHFT                                      0x6
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TURBO_TARGET_BMSK                                       0x3e
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TURBO_TARGET_SHFT                                        0x1
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_DIS_CPR_BMSK                                             0x1
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_DIS_CPR_SHFT                                             0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000dc)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_PHYS                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000dc)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IDDQ_CX_ACTIVE_BIT_4_0_BMSK                            0xf8000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IDDQ_CX_ACTIVE_BIT_4_0_SHFT                                  0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IDDQ_APC_ACTIVE_BMSK                                    0x7f80000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IDDQ_APC_ACTIVE_SHFT                                         0x13
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS_CALIB_BMSK                                          0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS_CALIB_SHFT                                             0x10
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS_BASE1_BMSK                                           0xff00
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS_BASE1_SHFT                                              0x8
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS_BASE0_BMSK                                             0xff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS_BASE0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e0)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_PHYS                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000e0)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_PH_B0M0_0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_PH_B0M0_0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_G_B0_BMSK                                              0x70000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_G_B0_SHFT                                                    0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CLK_B_BMSK                                              0xc000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CLK_B_SHFT                                                   0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CAP_B_BMSK                                              0x3000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CAP_B_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_SAR_B_BMSK                                               0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_SAR_B_SHFT                                                   0x16
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CSI_PHY_BMSK                                             0x3e0000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CSI_PHY_SHFT                                                 0x11
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_DSI_PHY_BMSK                                              0x1e000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_DSI_PHY_SHFT                                                  0xd
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_DSIPHY_PLL_BMSK                                            0x1e00
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_DSIPHY_PLL_SHFT                                               0x9
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_SPARE3_BMSK                                                 0x100
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_SPARE3_SHFT                                                   0x8
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IDDQ_MX_ACTIVE_BMSK                                          0xfc
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IDDQ_MX_ACTIVE_SHFT                                           0x2
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IDDQ_CX_ACTIVE_BIT_6_5_BMSK                                   0x3
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IDDQ_CX_ACTIVE_BIT_6_5_SHFT                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e4)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PHYS                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000e4)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_G_B2_BIT_1_0_BMSK                                      0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_G_B2_BIT_1_0_SHFT                                            0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_VREF_B1_BMSK                                           0x30000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_VREF_B1_SHFT                                                 0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B1M3_BMSK                                            0xe000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B1M3_SHFT                                                 0x19
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B1M2_BMSK                                            0x1c00000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B1M2_SHFT                                                 0x16
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B1M1_BMSK                                             0x380000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B1M1_SHFT                                                 0x13
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B1M0_BMSK                                              0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B1M0_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_G_B1_BMSK                                                  0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_G_B1_SHFT                                                     0xd
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_VREF_B0_BMSK                                               0x1800
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_VREF_B0_SHFT                                                  0xb
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B0M3_BMSK                                                0x700
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B0M3_SHFT                                                  0x8
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B0M2_BMSK                                                 0xe0
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B0M2_SHFT                                                  0x5
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B0M1_BMSK                                                 0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B0M1_SHFT                                                  0x2
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B0M0_BIT_2_1_BMSK                                          0x3
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_PH_B0M0_BIT_2_1_SHFT                                          0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e8)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PHYS                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000e8)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_VREF_B3_BMSK                                           0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_VREF_B3_SHFT                                                 0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B3M3_BMSK                                           0x38000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B3M3_SHFT                                                 0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B3M2_BMSK                                            0x7000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B3M2_SHFT                                                 0x18
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B3M1_BMSK                                             0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B3M1_SHFT                                                 0x15
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B3M0_BMSK                                             0x1c0000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B3M0_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_G_B3_BMSK                                                 0x38000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_G_B3_SHFT                                                     0xf
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_VREF_B2_BMSK                                               0x6000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_VREF_B2_SHFT                                                  0xd
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B2M3_BMSK                                               0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B2M3_SHFT                                                  0xa
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B2M2_BMSK                                                0x380
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B2M2_SHFT                                                  0x7
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B2M1_BMSK                                                 0x70
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B2M1_SHFT                                                  0x4
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B2M0_BMSK                                                  0xe
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_PH_B2M0_SHFT                                                  0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_G_B2_BIT_2_BMSK                                               0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_G_B2_BIT_2_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000ec)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_PHYS                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000ec)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_SPARE4_BMSK                                            0xff800000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_SPARE4_SHFT                                                  0x17
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_GNSS_ADC_CALIB_BMSK                                      0x700000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_GNSS_ADC_CALIB_SHFT                                          0x14
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_MODEM_TXDAC_FUSEFLAG_BMSK                                 0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_MODEM_TXDAC_FUSEFLAG_SHFT                                    0x13
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_COMBOTXDAC_SPARE_BMSK                                     0x7f800
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_COMBOTXDAC_SPARE_SHFT                                         0xb
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_COMBOTXDAC_RANGE_CORR_BMSK                                  0x400
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_COMBOTXDAC_RANGE_CORR_SHFT                                    0xa
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_COMBOTXDAC_AVEG_CORR_BMSK                                   0x200
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_COMBOTXDAC_AVEG_CORR_SHFT                                     0x9
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_COMBOTXDAC_RPOLY_CAL_BMSK                                   0x1fe
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_COMBOTXDAC_RPOLY_CAL_SHFT                                     0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_SPARE3_BMSK                                                   0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_SPARE3_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_PHYS(n)                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000f0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_MAXn                                                       3
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

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_PHYS(n)                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000f4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_MAXn                                                       3
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

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000110)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000110)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_REDUN_DATA_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_LSB_REDUN_DATA_SHFT                                          0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000114)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_PHYS                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000114)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_MEM_ACCEL_BMSK                                    0xf0000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_MEM_ACCEL_SHFT                                          0x1c
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_REDUN_DATA_BMSK                                    0xfffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW4_MSB_REDUN_DATA_SHFT                                          0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000118 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_PHYS(n)                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000118 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_MAXn                                                   6
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_PATCH_ADDR1_BMSK                              0xffff0000
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_PATCH_ADDR1_SHFT                                    0x10
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_PATCH_ADDR0_BMSK                                  0xffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_LSB_PATCH_ADDR0_SHFT                                     0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000011c + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_PHYS(n)                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000011c + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_MAXn                                                   6
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_PATCH_ADDR3_BMSK                              0xffff0000
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_PATCH_ADDR3_SHFT                                    0x10
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_PATCH_ADDR2_BMSK                                  0xffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROWn_MSB_PATCH_ADDR2_SHFT                                     0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_ADDR                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000150)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_PHYS                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000150)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_ADDR, HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_IN)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_PATCH_ADDR1_BMSK                              0xffff0000
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_PATCH_ADDR1_SHFT                                    0x10
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_PATCH_ADDR0_BMSK                                  0xffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_LSB_PATCH_ADDR0_SHFT                                     0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_ADDR                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000154)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_PHYS                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000154)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_ADDR, HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_IN)
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_UNUSED_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ADDR_ROW7_MSB_UNUSED_SHFT                                          0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000158 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_PHYS(n)                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000158 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_MAXn                                                  14
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_PATCH_DATA0_BMSK                              0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_LSB_PATCH_DATA0_SHFT                                     0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_ADDR(n)                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000015c + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_PHYS(n)                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000015c + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_MAXn                                                  14
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_PATCH_DATA1_BMSK                              0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_DATA_ROWn_MSB_PATCH_DATA1_SHFT                                     0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d0)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001d0)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_ADDR, HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_IN)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_QC_SPARE_1_0_BMSK                                    0xc0000000
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_QC_SPARE_1_0_SHFT                                          0x1e
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_PATCH_EN_BMSK                                        0x3fffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_LSB_PATCH_EN_SHFT                                               0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d4)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001d4)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_ADDR, HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_IN)
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_QC_SPARE_33_2_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_EN_MSB_QC_SPARE_33_2_SHFT                                          0x0

#define HWIO_QFPROM_RAW_SPARE_REG15_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d8)
#define HWIO_QFPROM_RAW_SPARE_REG15_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001d8)
#define HWIO_QFPROM_RAW_SPARE_REG15_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG15_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG15_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG15_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG15_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG15_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG15_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG15_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG15_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG15_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG15_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG15_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG15_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG15_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001dc)
#define HWIO_QFPROM_RAW_SPARE_REG15_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001dc)
#define HWIO_QFPROM_RAW_SPARE_REG15_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG15_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG15_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG15_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG15_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG15_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG15_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG15_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG15_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG15_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG15_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG15_MSB_SPARE1_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG15_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001e0)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001e0)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG16_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG16_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001e4)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001e4)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG16_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG16_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_SPARE1_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001e8)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001e8)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG17_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG17_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001ec)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001ec)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG17_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG17_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_SPARE1_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f0)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001f0)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG18_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG18_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f4)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001f4)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG18_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG18_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_SPARE1_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f8)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001f8)
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

#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001fc)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001fc)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE1_BMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n)                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000200 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_PHYS(n)                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000200 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_MAXn                                                             39
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n), HWIO_QFPROM_RAW_ACC_PRIVATEn_RMSK)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ACC_PRIVATE_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ACC_PRIVATE_SHFT                                                0x0

#define HWIO_ACC_IR_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002000)
#define HWIO_ACC_IR_PHYS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002000)
#define HWIO_ACC_IR_RMSK                                                                            0x1f
#define HWIO_ACC_IR_OUT(v)      \
        out_dword(HWIO_ACC_IR_ADDR,v)
#define HWIO_ACC_IR_INSTRUCTION_BMSK                                                                0x1f
#define HWIO_ACC_IR_INSTRUCTION_SHFT                                                                 0x0

#define HWIO_ACC_DR_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002004)
#define HWIO_ACC_DR_PHYS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002004)
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
#define HWIO_ACC_VERID_PHYS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002008)
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
#define HWIO_ACC_FEATSETn_PHYS(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_RMSK                                                                0xffffffff
#define HWIO_ACC_FEATSETn_MAXn                                                                         7
#define HWIO_ACC_FEATSETn_INI(n)        \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(n), HWIO_ACC_FEATSETn_RMSK)
#define HWIO_ACC_FEATSETn_INMI(n,mask)    \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(n), mask)
#define HWIO_ACC_FEATSETn_FEAT_BMSK                                                           0xffffffff
#define HWIO_ACC_FEATSETn_FEAT_SHFT                                                                  0x0

#define HWIO_ACC_STATE_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002038)
#define HWIO_ACC_STATE_PHYS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002038)
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
#define HWIO_QFPROM_BLOW_TIMER_PHYS                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000203c)
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
#define HWIO_QFPROM_TEST_CTRL_PHYS                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002040)
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
#define HWIO_QFPROM_ACCEL_PHYS                                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002044)
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
#define HWIO_QFPROM_BLOW_STATUS_PHYS                                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002048)
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
#define HWIO_QFPROM_ROM_ERROR_PHYS                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000204c)
#define HWIO_QFPROM_ROM_ERROR_RMSK                                                                   0x1
#define HWIO_QFPROM_ROM_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, HWIO_QFPROM_ROM_ERROR_RMSK)
#define HWIO_QFPROM_ROM_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, m)
#define HWIO_QFPROM_ROM_ERROR_ERROR_BMSK                                                             0x1
#define HWIO_QFPROM_ROM_ERROR_ERROR_SHFT                                                             0x0

#define HWIO_QFPROM_BIST_CTRL_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_PHYS                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002050)
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
#define HWIO_QFPROM_BIST_ERROR_PHYS                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_RMSK                                                           0xffffffff
#define HWIO_QFPROM_BIST_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, HWIO_QFPROM_BIST_ERROR_RMSK)
#define HWIO_QFPROM_BIST_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR_ERROR_BMSK                                                     0xffffffff
#define HWIO_QFPROM_BIST_ERROR_ERROR_SHFT                                                            0x0

#define HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_PHYS(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_RMSK                                                      0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_MAXn                                                               7
#define HWIO_QFPROM_HASH_SIGNATUREn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), HWIO_QFPROM_HASH_SIGNATUREn_RMSK)
#define HWIO_QFPROM_HASH_SIGNATUREn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), mask)
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_BMSK                                           0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_SHFT                                                  0x0

#define HWIO_HW_KEY_STATUS_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002080)
#define HWIO_HW_KEY_STATUS_PHYS                                                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002080)
#define HWIO_HW_KEY_STATUS_RMSK                                                                     0x1f
#define HWIO_HW_KEY_STATUS_IN          \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, HWIO_HW_KEY_STATUS_RMSK)
#define HWIO_HW_KEY_STATUS_INM(m)      \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, m)
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
#define HWIO_RESET_JDR_STATUS_PHYS                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002084)
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
#define HWIO_FEC_ESR_PHYS                                                                     (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002090)
#define HWIO_FEC_ESR_RMSK                                                                          0xfff
#define HWIO_FEC_ESR_IN          \
        in_dword_masked(HWIO_FEC_ESR_ADDR, HWIO_FEC_ESR_RMSK)
#define HWIO_FEC_ESR_INM(m)      \
        in_dword_masked(HWIO_FEC_ESR_ADDR, m)
#define HWIO_FEC_ESR_OUT(v)      \
        out_dword(HWIO_FEC_ESR_ADDR,v)
#define HWIO_FEC_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FEC_ESR_ADDR,m,v,HWIO_FEC_ESR_IN)
#define HWIO_FEC_ESR_CORR_SW_ACC_BMSK                                                              0x800
#define HWIO_FEC_ESR_CORR_SW_ACC_SHFT                                                                0xb
#define HWIO_FEC_ESR_CORR_BOOT_ROM_BMSK                                                            0x200
#define HWIO_FEC_ESR_CORR_BOOT_ROM_SHFT                                                              0x9
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_BMSK                                                          0x100
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_SHFT                                                            0x8
#define HWIO_FEC_ESR_CORR_MULT_BMSK                                                                 0x80
#define HWIO_FEC_ESR_CORR_MULT_SHFT                                                                  0x7
#define HWIO_FEC_ESR_CORR_SEEN_BMSK                                                                 0x40
#define HWIO_FEC_ESR_CORR_SEEN_SHFT                                                                  0x6
#define HWIO_FEC_ESR_ERR_SW_ACC_BMSK                                                                0x20
#define HWIO_FEC_ESR_ERR_SW_ACC_SHFT                                                                 0x5
#define HWIO_FEC_ESR_ERR_BOOT_ROM_BMSK                                                               0x8
#define HWIO_FEC_ESR_ERR_BOOT_ROM_SHFT                                                               0x3
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_BMSK                                                             0x4
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_SHFT                                                             0x2
#define HWIO_FEC_ESR_ERR_MULT_BMSK                                                                   0x2
#define HWIO_FEC_ESR_ERR_MULT_SHFT                                                                   0x1
#define HWIO_FEC_ESR_ERR_SEEN_BMSK                                                                   0x1
#define HWIO_FEC_ESR_ERR_SEEN_SHFT                                                                   0x0

#define HWIO_FEC_EAR_ADDR                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002094)
#define HWIO_FEC_EAR_PHYS                                                                     (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002094)
#define HWIO_FEC_EAR_RMSK                                                                     0xffffffff
#define HWIO_FEC_EAR_IN          \
        in_dword_masked(HWIO_FEC_EAR_ADDR, HWIO_FEC_EAR_RMSK)
#define HWIO_FEC_EAR_INM(m)      \
        in_dword_masked(HWIO_FEC_EAR_ADDR, m)
#define HWIO_FEC_EAR_CORR_ADDR_BMSK                                                           0xffff0000
#define HWIO_FEC_EAR_CORR_ADDR_SHFT                                                                 0x10
#define HWIO_FEC_EAR_ERR_ADDR_BMSK                                                                0xffff
#define HWIO_FEC_EAR_ERR_ADDR_SHFT                                                                   0x0

#define HWIO_QFPROM_CORR_JTAG_ID_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004000)
#define HWIO_QFPROM_CORR_JTAG_ID_PHYS                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004000)
#define HWIO_QFPROM_CORR_JTAG_ID_RMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_JTAG_ID_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_ADDR, HWIO_QFPROM_CORR_JTAG_ID_RMSK)
#define HWIO_QFPROM_CORR_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_ADDR, m)
#define HWIO_QFPROM_CORR_JTAG_ID_PTE_DATA0_BMSK                                               0xe0000000
#define HWIO_QFPROM_CORR_JTAG_ID_PTE_DATA0_SHFT                                                     0x1d
#define HWIO_QFPROM_CORR_JTAG_ID_MACCHIATO_EN_BMSK                                            0x10000000
#define HWIO_QFPROM_CORR_JTAG_ID_MACCHIATO_EN_SHFT                                                  0x1c
#define HWIO_QFPROM_CORR_JTAG_ID_FEATURE_ID_BMSK                                               0xff00000
#define HWIO_QFPROM_CORR_JTAG_ID_FEATURE_ID_SHFT                                                    0x14
#define HWIO_QFPROM_CORR_JTAG_ID_JTAG_ID_BMSK                                                    0xfffff
#define HWIO_QFPROM_CORR_JTAG_ID_JTAG_ID_SHFT                                                        0x0

#define HWIO_QFPROM_CORR_PTE1_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004004)
#define HWIO_QFPROM_CORR_PTE1_PHYS                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004004)
#define HWIO_QFPROM_CORR_PTE1_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE1_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, HWIO_QFPROM_CORR_PTE1_RMSK)
#define HWIO_QFPROM_CORR_PTE1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, m)
#define HWIO_QFPROM_CORR_PTE1_SPARE1_BMSK                                                     0xfc000000
#define HWIO_QFPROM_CORR_PTE1_SPARE1_SHFT                                                           0x1a
#define HWIO_QFPROM_CORR_PTE1_PROCESS_NODE_ID_BMSK                                             0x2000000
#define HWIO_QFPROM_CORR_PTE1_PROCESS_NODE_ID_SHFT                                                  0x19
#define HWIO_QFPROM_CORR_PTE1_TIER_BMSK                                                        0x1c00000
#define HWIO_QFPROM_CORR_PTE1_TIER_SHFT                                                             0x16
#define HWIO_QFPROM_CORR_PTE1_BONE_PILE_BMSK                                                    0x200000
#define HWIO_QFPROM_CORR_PTE1_BONE_PILE_SHFT                                                        0x15
#define HWIO_QFPROM_CORR_PTE1_ACC_SETTINGS_ID_BMSK                                              0x100000
#define HWIO_QFPROM_CORR_PTE1_ACC_SETTINGS_ID_SHFT                                                  0x14
#define HWIO_QFPROM_CORR_PTE1_WAFER_MR_BMSK                                                      0x80000
#define HWIO_QFPROM_CORR_PTE1_WAFER_MR_SHFT                                                         0x13
#define HWIO_QFPROM_CORR_PTE1_Q6_MAX_FREQ_BMSK                                                   0x70000
#define HWIO_QFPROM_CORR_PTE1_Q6_MAX_FREQ_SHFT                                                      0x10
#define HWIO_QFPROM_CORR_PTE1_PTE_DATA1_BMSK                                                      0xffff
#define HWIO_QFPROM_CORR_PTE1_PTE_DATA1_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_SERIAL_NUM_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004008)
#define HWIO_QFPROM_CORR_SERIAL_NUM_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004008)
#define HWIO_QFPROM_CORR_SERIAL_NUM_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_PTE2_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000400c)
#define HWIO_QFPROM_CORR_PTE2_PHYS                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000400c)
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

#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004010)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004010)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE_BMSK                                            0xfff00000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE_SHFT                                                  0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_BMSK                                             0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_SHFT                                                0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_BMSK                                             0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_SHFT                                                0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE17_BMSK                                             0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE17_SHFT                                                0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE16_BMSK                                             0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE16_SHFT                                                0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE15_BMSK                                              0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE15_SHFT                                                 0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                       0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                          0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_BMSK                                           0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_SHFT                                              0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_BMSK                                                0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_SHFT                                                   0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_BMSK                                               0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_SHFT                                                 0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB2_BMSK                                                0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB2_SHFT                                                  0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                          0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                            0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                  0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                 0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                  0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                            0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                             0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                             0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                              0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                        0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                         0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                         0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                         0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                              0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                              0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_BMSK                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004014)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004014)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE_BMSK                                            0xfff00000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE_SHFT                                                  0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_BMSK                                             0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_SHFT                                                0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_BMSK                                             0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_SHFT                                                0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE17_BMSK                                             0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE17_SHFT                                                0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE16_BMSK                                             0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE16_SHFT                                                0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE15_BMSK                                              0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE15_SHFT                                                 0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                       0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                          0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_BMSK                                           0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_SHFT                                              0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_BMSK                                                0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_SHFT                                                   0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_BMSK                                               0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_SHFT                                                 0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB2_BMSK                                                0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB2_SHFT                                                  0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                          0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                            0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                  0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                 0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                  0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                            0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                             0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                             0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                              0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_BMSK                                        0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_SHFT                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_BMSK                                         0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_SHFT                                         0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_BMSK                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_SHFT                                         0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                              0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                              0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_BMSK                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004018)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004018)
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

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000401c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000401c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_APPSBL0_BMSK                                     0xfffc0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_APPSBL0_SHFT                                           0x12
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_BMSK                                 0x3ffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_SHFT                                     0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004020)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004020)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_APPSBL1_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_APPSBL1_SHFT                                            0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004024)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004024)
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

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004028)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004028)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_BMSK                                         0xffff0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_SHFT                                               0x10
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_BMSK                                             0xffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_SHFT                                                0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000402c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000402c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SPARE0_BMSK                                      0x7fffff00
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SPARE0_SHFT                                             0x8
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SIMLOCK_BMSK                                     0x80000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SIMLOCK_SHFT                                           0x1f
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                     0xff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                      0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004030)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004030)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_BMSK                                      0xff800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_SHFT                                            0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                               0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                   0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                           0x3c0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                               0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_BMSK                             0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_SHFT                                0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_BMSK                             0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_SHFT                                0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_BMSK                              0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_SHFT                                 0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_BMSK                              0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_SHFT                                 0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG15_SECURE_BMSK                              0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG15_SECURE_SHFT                                 0xd
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

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004034)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004034)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                      0xff800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                            0x17
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

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004038)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004038)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_BMSK                              0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_SHFT                                    0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_BMSK                                       0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_SHFT                                          0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000403c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000403c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_VID_BMSK                                  0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_VID_SHFT                                        0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_PID_BMSK                                      0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_PID_SHFT                                         0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004040)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_PHYS                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004040)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CAMERA_RES_LIMIT_5MP_BMSK                       0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CAMERA_RES_LIMIT_5MP_SHFT                             0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_30_BMSK                                   0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_30_SHFT                                         0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDP_PANEL_RES_LIMIT_BMSK                        0x30000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MDP_PANEL_RES_LIMIT_SHFT                              0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_BMSK                               0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_SHFT                                    0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_26_BMSK                                    0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPARE_26_SHFT                                         0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ACC_DISABLE_BMSK                                 0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ACC_DISABLE_SHFT                                      0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                         0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                              0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                          0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                              0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_BMSK                             0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_SHFT                                 0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_BMSK                                  0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_SHFT                                      0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_BMSK                                  0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_SHFT                                      0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_BMSK                           0xe0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_SHFT                              0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_UIM2_DISABLE_BMSK                            0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_UIM2_DISABLE_SHFT                               0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_UIM1_DISABLE_BMSK                             0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_UIM1_DISABLE_SHFT                                0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_SMMU_DISABLE_BMSK                             0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_SMMU_DISABLE_SHFT                                0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_TX_DSDA_DISABLE_BMSK                          0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_TX_DSDA_DISABLE_SHFT                             0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_BMSK                            0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_SHFT                               0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_BMSK                             0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_SHFT                               0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_BMSK                            0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_SHFT                              0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_BMSK                           0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_SHFT                             0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_BMSK                         0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_SHFT                           0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_BMSK                     0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_SHFT                      0x7
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

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004044)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PHYS                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004044)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                      0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_CAMERA_BMSK                       0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_CAMERA_SHFT                             0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_MDP_BMSK                          0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_MDP_SHFT                                0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_VENUS_BMSK                        0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_VENUS_SHFT                              0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_OXILI_BMSK                         0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_OXILI_SHFT                              0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_PRONTO_BMSK                        0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_EFUSE_DISABLE_PRONTO_SHFT                             0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_HEVC_DECODE_BMSK                   0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_HEVC_DECODE_SHFT                        0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_720P_ENCODE_BMSK                   0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_720P_ENCODE_SHFT                        0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_720P_DECODE_BMSK                    0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_720P_DECODE_SHFT                        0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_1080P_ENCODE_BMSK                   0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_1080P_ENCODE_SHFT                       0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_1080P_DECODE_BMSK                   0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENUS_DISABLE_1080P_DECODE_SHFT                       0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE0_BMSK                                       0x1ff800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE0_SHFT                                            0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_CONFIGURATION1_BMSK                             0x600
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_CONFIGURATION1_SHFT                               0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_CONFIGURATION0_BMSK                             0x180
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_CONFIGURATION0_SHFT                               0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE_BMSK                                            0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE_SHFT                                             0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BMSK                                0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_SHFT                                 0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_BMSK                              0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_SHFT                               0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_BMSK                                     0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_SHFT                                     0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_L2_CACHE_UPPER_BANK_DISABLE_BMSK                  0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_L2_CACHE_UPPER_BANK_DISABLE_SHFT                  0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004048)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PHYS                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004048)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                     0xffe00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                           0x15
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

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000404c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_PHYS                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000404c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                     0xf8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                           0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                    0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                         0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                           0x3ffc000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                 0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SPARE1_BMSK                                         0x3c00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SPARE1_SHFT                                            0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_BMSK                         0x3e0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_SHFT                           0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                           0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                            0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004050)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_PHYS                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004050)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK           0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                  0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004054)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_PHYS                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004054)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE4_BMSK                                     0xc0000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE4_SHFT                                           0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_BMSK                  0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_SHFT                        0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE3_BMSK                                     0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE3_SHFT                                           0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_BMSK                          0xf000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_SHFT                               0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                            0xf80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_BMSK                          0x78000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_SHFT                              0xf
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
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_WIFI_DUAL_BAND_DISABLE_BMSK                          0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_WIFI_DUAL_BAND_DISABLE_SHFT                            0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_DDR_LIMIT_MAX_FREQ_BMSK                               0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_DDR_LIMIT_MAX_FREQ_SHFT                                0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_BMSK                              0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_SHFT                               0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_BMSK                           0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_SHFT                            0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                 0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004058 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_PHYS(n)                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004058 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                          3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                              0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000405c + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_PHYS(n)                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000405c + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                          3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                              0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004078 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_PHYS(n)                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004078 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                          3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                              0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000407c + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_PHYS(n)                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000407c + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                          3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                              0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004098)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_PHYS                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004098)
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

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_ADDR                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000409c)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_PHYS                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000409c)
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

#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a0)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040a0)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS2_POINT2_BIT_1_0_BMSK                           0xc0000000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS2_POINT2_BIT_1_0_SHFT                                 0x1e
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS2_POINT1_BMSK                                   0x3f000000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS2_POINT1_SHFT                                         0x18
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS1_POINT2_BMSK                                     0xfc0000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS1_POINT2_SHFT                                         0x12
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS1_POINT1_BMSK                                      0x3f000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS1_POINT1_SHFT                                          0xc
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS0_POINT2_BMSK                                        0xfc0
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS0_POINT2_SHFT                                          0x6
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS0_POINT1_BMSK                                         0x3f
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_TSENS0_POINT1_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a4)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040a4)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_SPARE3_BMSK                                          0xf0000000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_SPARE3_SHFT                                                0x1c
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS4_POINT2_BMSK                                    0xfc00000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS4_POINT2_SHFT                                         0x16
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS4_POINT1_BMSK                                     0x3f0000
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS4_POINT1_SHFT                                         0x10
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS3_POINT2_BMSK                                       0xfc00
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS3_POINT2_SHFT                                          0xa
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS3_POINT1_BMSK                                        0x3f0
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS3_POINT1_SHFT                                          0x4
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS2_POINT2_BIT_5_2_BMSK                                  0xf
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_TSENS2_POINT2_BIT_5_2_SHFT                                  0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_PHYS(n)                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040a8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_MAXn                                                         3
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_HASH_DATA0_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_HASH_DATA0_SHFT                                            0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040ac + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_PHYS(n)                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040ac + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_RMSK                                                  0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_MAXn                                                         3
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                       0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                            0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040c8)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_PHYS                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040c8)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_HASH_DATA0_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_HASH_DATA0_SHFT                                            0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040cc)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_PHYS                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040cc)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_RMSK                                                  0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_SPARE_BMSK                                            0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_SPARE_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040d0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CPR1_NOMINAL_QUOT_VMIN_BIT_7_0_BMSK                   0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CPR1_NOMINAL_QUOT_VMIN_BIT_7_0_SHFT                         0x18
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CPR1_NOMINAL_TARGET_BMSK                                0xfc0000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CPR1_NOMINAL_TARGET_SHFT                                    0x12
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CPR1_TURBO_QUOT_VMIN_BMSK                                0x3ffc0
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CPR1_TURBO_QUOT_VMIN_SHFT                                    0x6
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CPR1_TURBO_TARGET_BMSK                                      0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CPR1_TURBO_TARGET_SHFT                                       0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040d4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_NOM_ROSEL_BMSK                                   0xe0000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_NOM_ROSEL_SHFT                                         0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR_REV_BMSK                                          0x18000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR_REV_SHFT                                                0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_DIS_CPR_BMSK                                      0x4000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_DIS_CPR_SHFT                                           0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_SPARE3_BMSK                                            0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_SPARE3_SHFT                                                 0x19
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_TUR_ROSEL_BMSK                                    0x1c00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_TUR_ROSEL_SHFT                                         0x16
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_SVS_QUOT_VMIN_BMSK                                 0x3ffc00
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_SVS_QUOT_VMIN_SHFT                                      0xa
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_SVS_TARGET_BMSK                                       0x3f0
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_SVS_TARGET_SHFT                                         0x4
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_NOMINAL_QUOT_VMIN_BIT_11_8_BMSK                         0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_CPR1_NOMINAL_QUOT_VMIN_BIT_11_8_SHFT                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d8)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040d8)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_SVS_TARGET_BMSK                                  0xf8000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_SVS_TARGET_SHFT                                        0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_NOMINAL_TARGET_BMSK                               0x7c00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_NOMINAL_TARGET_SHFT                                    0x16
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_TURBO_TARGET_BMSK                                  0x3e0000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_TURBO_TARGET_SHFT                                      0x11
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_DIS_CPR_BMSK                                        0x10000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_DIS_CPR_SHFT                                           0x10
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_SVS_TARGET_BMSK                                      0xf800
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_SVS_TARGET_SHFT                                         0xb
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_NOMINAL_TARGET_BMSK                                   0x7c0
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_NOMINAL_TARGET_SHFT                                     0x6
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TURBO_TARGET_BMSK                                      0x3e
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TURBO_TARGET_SHFT                                       0x1
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_DIS_CPR_BMSK                                            0x1
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_DIS_CPR_SHFT                                            0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040dc)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040dc)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IDDQ_CX_ACTIVE_BIT_4_0_BMSK                           0xf8000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IDDQ_CX_ACTIVE_BIT_4_0_SHFT                                 0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IDDQ_APC_ACTIVE_BMSK                                   0x7f80000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IDDQ_APC_ACTIVE_SHFT                                        0x13
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS_CALIB_BMSK                                         0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS_CALIB_SHFT                                            0x10
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS_BASE1_BMSK                                          0xff00
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS_BASE1_SHFT                                             0x8
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS_BASE0_BMSK                                            0xff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS_BASE0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e0)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040e0)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_PH_B0M0_0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_PH_B0M0_0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_G_B0_BMSK                                             0x70000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_G_B0_SHFT                                                   0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CLK_B_BMSK                                             0xc000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CLK_B_SHFT                                                  0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CAP_B_BMSK                                             0x3000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CAP_B_SHFT                                                  0x18
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_SAR_B_BMSK                                              0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_SAR_B_SHFT                                                  0x16
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CSI_PHY_BMSK                                            0x3e0000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CSI_PHY_SHFT                                                0x11
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_DSI_PHY_BMSK                                             0x1e000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_DSI_PHY_SHFT                                                 0xd
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_DSIPHY_PLL_BMSK                                           0x1e00
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_DSIPHY_PLL_SHFT                                              0x9
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_SPARE3_BMSK                                                0x100
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_SPARE3_SHFT                                                  0x8
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IDDQ_MX_ACTIVE_BMSK                                         0xfc
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IDDQ_MX_ACTIVE_SHFT                                          0x2
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IDDQ_CX_ACTIVE_BIT_6_5_BMSK                                  0x3
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IDDQ_CX_ACTIVE_BIT_6_5_SHFT                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e4)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040e4)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_G_B2_BIT_1_0_BMSK                                     0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_G_B2_BIT_1_0_SHFT                                           0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_VREF_B1_BMSK                                          0x30000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_VREF_B1_SHFT                                                0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B1M3_BMSK                                           0xe000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B1M3_SHFT                                                0x19
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B1M2_BMSK                                           0x1c00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B1M2_SHFT                                                0x16
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B1M1_BMSK                                            0x380000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B1M1_SHFT                                                0x13
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B1M0_BMSK                                             0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B1M0_SHFT                                                0x10
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_G_B1_BMSK                                                 0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_G_B1_SHFT                                                    0xd
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_VREF_B0_BMSK                                              0x1800
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_VREF_B0_SHFT                                                 0xb
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B0M3_BMSK                                               0x700
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B0M3_SHFT                                                 0x8
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B0M2_BMSK                                                0xe0
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B0M2_SHFT                                                 0x5
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B0M1_BMSK                                                0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B0M1_SHFT                                                 0x2
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B0M0_BIT_2_1_BMSK                                         0x3
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_PH_B0M0_BIT_2_1_SHFT                                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e8)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040e8)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_VREF_B3_BMSK                                          0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_VREF_B3_SHFT                                                0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B3M3_BMSK                                          0x38000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B3M3_SHFT                                                0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B3M2_BMSK                                           0x7000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B3M2_SHFT                                                0x18
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B3M1_BMSK                                            0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B3M1_SHFT                                                0x15
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B3M0_BMSK                                            0x1c0000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B3M0_SHFT                                                0x12
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_G_B3_BMSK                                                0x38000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_G_B3_SHFT                                                    0xf
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_VREF_B2_BMSK                                              0x6000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_VREF_B2_SHFT                                                 0xd
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B2M3_BMSK                                              0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B2M3_SHFT                                                 0xa
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B2M2_BMSK                                               0x380
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B2M2_SHFT                                                 0x7
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B2M1_BMSK                                                0x70
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B2M1_SHFT                                                 0x4
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B2M0_BMSK                                                 0xe
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_PH_B2M0_SHFT                                                 0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_G_B2_BIT_2_BMSK                                              0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_G_B2_BIT_2_SHFT                                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040ec)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_PHYS                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040ec)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_SPARE4_BMSK                                           0xff800000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_SPARE4_SHFT                                                 0x17
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_GNSS_ADC_CALIB_BMSK                                     0x700000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_GNSS_ADC_CALIB_SHFT                                         0x14
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_MODEM_TXDAC_FUSEFLAG_BMSK                                0x80000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_MODEM_TXDAC_FUSEFLAG_SHFT                                   0x13
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_COMBOTXDAC_SPARE_BMSK                                    0x7f800
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_COMBOTXDAC_SPARE_SHFT                                        0xb
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_COMBOTXDAC_RANGE_CORR_BMSK                                 0x400
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_COMBOTXDAC_RANGE_CORR_SHFT                                   0xa
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_COMBOTXDAC_AVEG_CORR_BMSK                                  0x200
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_COMBOTXDAC_AVEG_CORR_SHFT                                    0x9
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_COMBOTXDAC_RPOLY_CAL_BMSK                                  0x1fe
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_COMBOTXDAC_RPOLY_CAL_SHFT                                    0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_SPARE3_BMSK                                                  0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_SPARE3_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_PHYS(n)                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040f0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_MAXn                                                      3
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_PHYS(n)                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040f4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_MAXn                                                      3
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004110)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_LSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004110)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_LSB_REDUN_DATA_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_LSB_REDUN_DATA_SHFT                                         0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004114)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004114)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_MEM_ACCEL_BMSK                                   0xf0000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_MEM_ACCEL_SHFT                                         0x1c
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_REDUN_DATA_BMSK                                   0xfffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW4_MSB_REDUN_DATA_SHFT                                         0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_ADDR(n)                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004118 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_PHYS(n)                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004118 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_MAXn                                                  6
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_PATCH_ADDR1_BMSK                             0xffff0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_PATCH_ADDR1_SHFT                                   0x10
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_PATCH_ADDR0_BMSK                                 0xffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_LSB_PATCH_ADDR0_SHFT                                    0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_ADDR(n)                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000411c + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_PHYS(n)                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000411c + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_MAXn                                                  6
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_PATCH_ADDR3_BMSK                             0xffff0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_PATCH_ADDR3_SHFT                                   0x10
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_PATCH_ADDR2_BMSK                                 0xffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROWn_MSB_PATCH_ADDR2_SHFT                                    0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_ADDR                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004150)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_PHYS                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004150)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_ADDR, HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_PATCH_ADDR1_BMSK                             0xffff0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_PATCH_ADDR1_SHFT                                   0x10
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_PATCH_ADDR0_BMSK                                 0xffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_LSB_PATCH_ADDR0_SHFT                                    0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_MSB_ADDR                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004154)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_MSB_PHYS                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004154)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_MSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_MSB_ADDR, HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_MSB_UNUSED_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ADDR_ROW7_MSB_UNUSED_SHFT                                         0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_ADDR(n)                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004158 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_PHYS(n)                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004158 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_MAXn                                                 14
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_PATCH_DATA0_BMSK                             0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_LSB_PATCH_DATA0_SHFT                                    0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_ADDR(n)                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000415c + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_PHYS(n)                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000415c + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_MAXn                                                 14
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_PATCH_DATA1_BMSK                             0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_DATA_ROWn_MSB_PATCH_DATA1_SHFT                                    0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d0)
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_PHYS                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041d0)
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_ADDR, HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_QC_SPARE_1_0_BMSK                                   0xc0000000
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_QC_SPARE_1_0_SHFT                                         0x1e
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_PATCH_EN_BMSK                                       0x3fffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_LSB_PATCH_EN_SHFT                                              0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_EN_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d4)
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_MSB_PHYS                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041d4)
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_EN_MSB_ADDR, HWIO_QFPROM_CORR_ROM_PATCH_EN_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_EN_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_MSB_QC_SPARE_33_2_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_EN_MSB_QC_SPARE_33_2_SHFT                                         0x0

#define HWIO_QFPROM_CORR_SPARE_REG15_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d8)
#define HWIO_QFPROM_CORR_SPARE_REG15_LSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041d8)
#define HWIO_QFPROM_CORR_SPARE_REG15_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG15_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG15_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG15_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG15_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG15_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG15_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG15_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG15_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041dc)
#define HWIO_QFPROM_CORR_SPARE_REG15_MSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041dc)
#define HWIO_QFPROM_CORR_SPARE_REG15_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG15_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG15_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG15_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG15_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG15_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG15_MSB_SPARE1_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG15_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e0)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041e0)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG16_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e4)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041e4)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG16_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_SPARE1_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e8)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041e8)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG17_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ec)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041ec)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG17_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_SPARE1_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f0)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041f0)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG18_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f4)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041f4)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG18_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_SPARE1_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f8)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041f8)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041fc)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_PHYS                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041fc)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE1_BMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE1_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n)                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004200 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_PHYS(n)                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004200 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_MAXn                                                            39
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n), HWIO_QFPROM_CORR_ACC_PRIVATEn_RMSK)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ACC_PRIVATE_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ACC_PRIVATE_SHFT                                               0x0

#define HWIO_SEC_CTRL_HW_VERSION_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_PHYS                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006000)
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
#define HWIO_FEATURE_CONFIG0_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006004)
#define HWIO_FEATURE_CONFIG0_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG0_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, HWIO_FEATURE_CONFIG0_RMSK)
#define HWIO_FEATURE_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, m)
#define HWIO_FEATURE_CONFIG0_CAMERA_RES_LIMIT_5MP_BMSK                                        0x80000000
#define HWIO_FEATURE_CONFIG0_CAMERA_RES_LIMIT_5MP_SHFT                                              0x1f
#define HWIO_FEATURE_CONFIG0_SPARE_30_BMSK                                                    0x40000000
#define HWIO_FEATURE_CONFIG0_SPARE_30_SHFT                                                          0x1e
#define HWIO_FEATURE_CONFIG0_MDP_PANEL_RES_LIMIT_BMSK                                         0x30000000
#define HWIO_FEATURE_CONFIG0_MDP_PANEL_RES_LIMIT_SHFT                                               0x1c
#define HWIO_FEATURE_CONFIG0_NIDNT_DISABLE_BMSK                                                0x8000000
#define HWIO_FEATURE_CONFIG0_NIDNT_DISABLE_SHFT                                                     0x1b
#define HWIO_FEATURE_CONFIG0_SPARE_26_BMSK                                                     0x4000000
#define HWIO_FEATURE_CONFIG0_SPARE_26_SHFT                                                          0x1a
#define HWIO_FEATURE_CONFIG0_ACC_DISABLE_BMSK                                                  0x2000000
#define HWIO_FEATURE_CONFIG0_ACC_DISABLE_SHFT                                                       0x19
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_BMSK                                          0x1000000
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_SHFT                                               0x18
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_BMSK                                           0x800000
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_SHFT                                               0x17
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_BMSK                                              0x400000
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_SHFT                                                  0x16
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_BMSK                                                   0x200000
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_SHFT                                                       0x15
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_BMSK                                                   0x100000
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_SHFT                                                       0x14
#define HWIO_FEATURE_CONFIG0_MODEM_SPARE_DISABLE_BMSK                                            0xe0000
#define HWIO_FEATURE_CONFIG0_MODEM_SPARE_DISABLE_SHFT                                               0x11
#define HWIO_FEATURE_CONFIG0_MODEM_UIM2_DISABLE_BMSK                                             0x10000
#define HWIO_FEATURE_CONFIG0_MODEM_UIM2_DISABLE_SHFT                                                0x10
#define HWIO_FEATURE_CONFIG0_MODEM_UIM1_DISABLE_BMSK                                              0x8000
#define HWIO_FEATURE_CONFIG0_MODEM_UIM1_DISABLE_SHFT                                                 0xf
#define HWIO_FEATURE_CONFIG0_MODEM_SMMU_DISABLE_BMSK                                              0x4000
#define HWIO_FEATURE_CONFIG0_MODEM_SMMU_DISABLE_SHFT                                                 0xe
#define HWIO_FEATURE_CONFIG0_MODEM_TX_DSDA_DISABLE_BMSK                                           0x2000
#define HWIO_FEATURE_CONFIG0_MODEM_TX_DSDA_DISABLE_SHFT                                              0xd
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX3_DISABLE_BMSK                                             0x1000
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX3_DISABLE_SHFT                                                0xc
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_BMSK                                              0x800
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_SHFT                                                0xb
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_BMSK                                             0x400
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_SHFT                                               0xa
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_BMSK                                            0x200
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_SHFT                                              0x9
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_BMSK                                          0x100
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_SHFT                                            0x8
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_BMSK                                      0x80
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_SHFT                                       0x7
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
#define HWIO_FEATURE_CONFIG1_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006008)
#define HWIO_FEATURE_CONFIG1_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG1_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, HWIO_FEATURE_CONFIG1_RMSK)
#define HWIO_FEATURE_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, m)
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                 0x80000000
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                       0x1f
#define HWIO_FEATURE_CONFIG1_EFUSE_DISABLE_CAMERA_BMSK                                        0x40000000
#define HWIO_FEATURE_CONFIG1_EFUSE_DISABLE_CAMERA_SHFT                                              0x1e
#define HWIO_FEATURE_CONFIG1_EFUSE_DISABLE_MDP_BMSK                                           0x20000000
#define HWIO_FEATURE_CONFIG1_EFUSE_DISABLE_MDP_SHFT                                                 0x1d
#define HWIO_FEATURE_CONFIG1_EFUSE_DISABLE_VENUS_BMSK                                         0x10000000
#define HWIO_FEATURE_CONFIG1_EFUSE_DISABLE_VENUS_SHFT                                               0x1c
#define HWIO_FEATURE_CONFIG1_EFUSE_DISABLE_OXILI_BMSK                                          0x8000000
#define HWIO_FEATURE_CONFIG1_EFUSE_DISABLE_OXILI_SHFT                                               0x1b
#define HWIO_FEATURE_CONFIG1_EFUSE_DISABLE_PRONTO_BMSK                                         0x4000000
#define HWIO_FEATURE_CONFIG1_EFUSE_DISABLE_PRONTO_SHFT                                              0x1a
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_HEVC_DECODE_BMSK                                    0x2000000
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_HEVC_DECODE_SHFT                                         0x19
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_720P_ENCODE_BMSK                                    0x1000000
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_720P_ENCODE_SHFT                                         0x18
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_720P_DECODE_BMSK                                     0x800000
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_720P_DECODE_SHFT                                         0x17
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_1080P_ENCODE_BMSK                                    0x400000
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_1080P_ENCODE_SHFT                                        0x16
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_1080P_DECODE_BMSK                                    0x200000
#define HWIO_FEATURE_CONFIG1_VENUS_DISABLE_1080P_DECODE_SHFT                                        0x15
#define HWIO_FEATURE_CONFIG1_SPARE0_BMSK                                                        0x1ff800
#define HWIO_FEATURE_CONFIG1_SPARE0_SHFT                                                             0xb
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION1_BMSK                                              0x600
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION1_SHFT                                                0x9
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION0_BMSK                                              0x180
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION0_SHFT                                                0x7
#define HWIO_FEATURE_CONFIG1_SPARE_BMSK                                                             0x40
#define HWIO_FEATURE_CONFIG1_SPARE_SHFT                                                              0x6
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_BMSK                                                 0x20
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_SHFT                                                  0x5
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_BMSK                                               0x18
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_SHFT                                                0x3
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_BMSK                                                      0x6
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_SHFT                                                      0x1
#define HWIO_FEATURE_CONFIG1_APPS_L2_CACHE_UPPER_BANK_DISABLE_BMSK                                   0x1
#define HWIO_FEATURE_CONFIG1_APPS_L2_CACHE_UPPER_BANK_DISABLE_SHFT                                   0x0

#define HWIO_FEATURE_CONFIG2_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, HWIO_FEATURE_CONFIG2_RMSK)
#define HWIO_FEATURE_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, m)
#define HWIO_FEATURE_CONFIG2_SPARE1_BMSK                                                      0xffe00000
#define HWIO_FEATURE_CONFIG2_SPARE1_SHFT                                                            0x15
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
#define HWIO_FEATURE_CONFIG3_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006010)
#define HWIO_FEATURE_CONFIG3_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG3_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, HWIO_FEATURE_CONFIG3_RMSK)
#define HWIO_FEATURE_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, m)
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_BMSK                                      0xf8000000
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_SHFT                                            0x1b
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_BMSK                                     0x4000000
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_SHFT                                          0x1a
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_BMSK                                            0x3ffc000
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_SHFT                                                  0xe
#define HWIO_FEATURE_CONFIG3_SPARE1_BMSK                                                          0x3c00
#define HWIO_FEATURE_CONFIG3_SPARE1_SHFT                                                             0xa
#define HWIO_FEATURE_CONFIG3_MODEM_PBL_PATCH_VERSION_BMSK                                          0x3e0
#define HWIO_FEATURE_CONFIG3_MODEM_PBL_PATCH_VERSION_SHFT                                            0x5
#define HWIO_FEATURE_CONFIG3_APPS_PBL_PATCH_VERSION_BMSK                                            0x1f
#define HWIO_FEATURE_CONFIG3_APPS_PBL_PATCH_VERSION_SHFT                                             0x0

#define HWIO_FEATURE_CONFIG4_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006014)
#define HWIO_FEATURE_CONFIG4_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006014)
#define HWIO_FEATURE_CONFIG4_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG4_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, HWIO_FEATURE_CONFIG4_RMSK)
#define HWIO_FEATURE_CONFIG4_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, m)
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK                            0xffffffff
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                                   0x0

#define HWIO_FEATURE_CONFIG5_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006018)
#define HWIO_FEATURE_CONFIG5_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006018)
#define HWIO_FEATURE_CONFIG5_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG5_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, HWIO_FEATURE_CONFIG5_RMSK)
#define HWIO_FEATURE_CONFIG5_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, m)
#define HWIO_FEATURE_CONFIG5_SPARE4_BMSK                                                      0xc0000000
#define HWIO_FEATURE_CONFIG5_SPARE4_SHFT                                                            0x1e
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_TRIGGER_DISABLE_BMSK                                   0x20000000
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_TRIGGER_DISABLE_SHFT                                         0x1d
#define HWIO_FEATURE_CONFIG5_SPARE3_BMSK                                                      0x10000000
#define HWIO_FEATURE_CONFIG5_SPARE3_SHFT                                                            0x1c
#define HWIO_FEATURE_CONFIG5_MODEM_PBL_PLL_CTRL_BMSK                                           0xf000000
#define HWIO_FEATURE_CONFIG5_MODEM_PBL_PLL_CTRL_SHFT                                                0x18
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_BMSK                                             0xf80000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_SHFT                                                 0x13
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_BMSK                                           0x78000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_SHFT                                               0xf
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
#define HWIO_FEATURE_CONFIG5_WIFI_DUAL_BAND_DISABLE_BMSK                                           0x100
#define HWIO_FEATURE_CONFIG5_WIFI_DUAL_BAND_DISABLE_SHFT                                             0x8
#define HWIO_FEATURE_CONFIG5_DDR_LIMIT_MAX_FREQ_BMSK                                                0x80
#define HWIO_FEATURE_CONFIG5_DDR_LIMIT_MAX_FREQ_SHFT                                                 0x7
#define HWIO_FEATURE_CONFIG5_MODEM_BOOT_FROM_ROM_BMSK                                               0x40
#define HWIO_FEATURE_CONFIG5_MODEM_BOOT_FROM_ROM_SHFT                                                0x6
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_BMSK                                            0x20
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_SHFT                                             0x5
#define HWIO_FEATURE_CONFIG5_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                                 0x1f
#define HWIO_FEATURE_CONFIG5_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                                  0x0

#define HWIO_OEM_CONFIG0_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000601c)
#define HWIO_OEM_CONFIG0_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000601c)
#define HWIO_OEM_CONFIG0_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG0_IN          \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, HWIO_OEM_CONFIG0_RMSK)
#define HWIO_OEM_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, m)
#define HWIO_OEM_CONFIG0_SPARE0_BMSK                                                          0xff800000
#define HWIO_OEM_CONFIG0_SPARE0_SHFT                                                                0x17
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_BMSK                                                   0x400000
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_SHFT                                                       0x16
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_BMSK                                               0x3c0000
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_SHFT                                                   0x12
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_BMSK                                                 0x20000
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_SHFT                                                    0x11
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_BMSK                                                 0x10000
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_SHFT                                                    0x10
#define HWIO_OEM_CONFIG0_SPARE_REG17_SECURE_BMSK                                                  0x8000
#define HWIO_OEM_CONFIG0_SPARE_REG17_SECURE_SHFT                                                     0xf
#define HWIO_OEM_CONFIG0_SPARE_REG16_SECURE_BMSK                                                  0x4000
#define HWIO_OEM_CONFIG0_SPARE_REG16_SECURE_SHFT                                                     0xe
#define HWIO_OEM_CONFIG0_SPARE_REG15_SECURE_BMSK                                                  0x2000
#define HWIO_OEM_CONFIG0_SPARE_REG15_SECURE_SHFT                                                     0xd
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
#define HWIO_OEM_CONFIG1_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006020)
#define HWIO_OEM_CONFIG1_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG1_IN          \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, HWIO_OEM_CONFIG1_RMSK)
#define HWIO_OEM_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, m)
#define HWIO_OEM_CONFIG1_SPARE1_BMSK                                                          0xff800000
#define HWIO_OEM_CONFIG1_SPARE1_SHFT                                                                0x17
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
#define HWIO_OEM_CONFIG2_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006024)
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
#define HWIO_OEM_CONFIG3_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006028)
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
#define HWIO_BOOT_CONFIG_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000602c)
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
#define HWIO_BOOT_CONFIG_FAST_BOOT_BMSK                                                             0x1e
#define HWIO_BOOT_CONFIG_FAST_BOOT_SHFT                                                              0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_BMSK                                                                0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_SHFT                                                                0x0

#define HWIO_SECURE_BOOTn_ADDR(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006030 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_PHYS(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006030 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_RMSK                                                                     0x17f
#define HWIO_SECURE_BOOTn_MAXn                                                                         7
#define HWIO_SECURE_BOOTn_INI(n)        \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), HWIO_SECURE_BOOTn_RMSK)
#define HWIO_SECURE_BOOTn_INMI(n,mask)    \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), mask)
#define HWIO_SECURE_BOOTn_FUSE_SRC_BMSK                                                            0x100
#define HWIO_SECURE_BOOTn_FUSE_SRC_SHFT                                                              0x8
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_BMSK                                                       0x40
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_SHFT                                                        0x6
#define HWIO_SECURE_BOOTn_AUTH_EN_BMSK                                                              0x20
#define HWIO_SECURE_BOOTn_AUTH_EN_SHFT                                                               0x5
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_BMSK                                                      0x10
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHFT                                                       0x4
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_BMSK                                                     0xf
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_SHFT                                                     0x0

#define HWIO_OVERRIDE_0_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006058)
#define HWIO_OVERRIDE_0_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006058)
#define HWIO_OVERRIDE_0_RMSK                                                                  0x10000003
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
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_BMSK                                                     0x2
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_SHFT                                                     0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_BMSK                                               0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_SHFT                                               0x0

#define HWIO_OVERRIDE_1_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000605c)
#define HWIO_OVERRIDE_1_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000605c)
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
#define HWIO_OVERRIDE_2_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006060)
#define HWIO_OVERRIDE_2_RMSK                                                                    0x3ffc00
#define HWIO_OVERRIDE_2_IN          \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, HWIO_OVERRIDE_2_RMSK)
#define HWIO_OVERRIDE_2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, m)
#define HWIO_OVERRIDE_2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_2_ADDR,v)
#define HWIO_OVERRIDE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_2_ADDR,m,v,HWIO_OVERRIDE_2_IN)
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
#define HWIO_OVERRIDE_3_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006064)
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
#define HWIO_OVERRIDE_4_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006068)
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
#define HWIO_CAPT_SEC_GPIO_PHYS                                                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000606c)
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
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_BMSK                                                  0x7fff
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_SHFT                                                     0x0

#define HWIO_APP_PROC_CFG_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006070)
#define HWIO_APP_PROC_CFG_PHYS                                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006070)
#define HWIO_APP_PROC_CFG_RMSK                                                                  0xffffff
#define HWIO_APP_PROC_CFG_IN          \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, HWIO_APP_PROC_CFG_RMSK)
#define HWIO_APP_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, m)
#define HWIO_APP_PROC_CFG_OUT(v)      \
        out_dword(HWIO_APP_PROC_CFG_ADDR,v)
#define HWIO_APP_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APP_PROC_CFG_ADDR,m,v,HWIO_APP_PROC_CFG_IN)
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
#define HWIO_MSS_PROC_CFG_PHYS                                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006074)
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
#define HWIO_QFPROM_CLK_CTL_PHYS                                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006078)
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
#define HWIO_JTAG_ID_PHYS                                                                     (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000607c)
#define HWIO_JTAG_ID_RMSK                                                                     0xffffffff
#define HWIO_JTAG_ID_IN          \
        in_dword_masked(HWIO_JTAG_ID_ADDR, HWIO_JTAG_ID_RMSK)
#define HWIO_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_JTAG_ID_ADDR, m)
#define HWIO_JTAG_ID_JTAG_ID_BMSK                                                             0xffffffff
#define HWIO_JTAG_ID_JTAG_ID_SHFT                                                                    0x0

#define HWIO_OEM_ID_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006080)
#define HWIO_OEM_ID_PHYS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006080)
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
#define HWIO_TEST_BUS_SEL_PHYS                                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006084)
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

#define HWIO_SPDM_DYN_SECURE_MODE_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006088)
#define HWIO_SPDM_DYN_SECURE_MODE_PHYS                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006088)
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

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_MUTEX
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_MUTEX_REG_BASE                  (CORE_TOP_CSR_BASE      + 0x00005000)
#define TCSR_TCSR_MUTEX_REG_BASE_PHYS             (CORE_TOP_CSR_BASE_PHYS + 0x00005000)

#define HWIO_TCSR_MUTEX_REG_n_ADDR(n)             (TCSR_TCSR_MUTEX_REG_BASE      + 0x00000000 + 0x1000 * (n))
#define HWIO_TCSR_MUTEX_REG_n_PHYS(n)             (TCSR_TCSR_MUTEX_REG_BASE_PHYS + 0x00000000 + 0x1000 * (n))
#define HWIO_TCSR_MUTEX_REG_n_RMSK                      0xff
#define HWIO_TCSR_MUTEX_REG_n_MAXn                        31
#define HWIO_TCSR_MUTEX_REG_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), HWIO_TCSR_MUTEX_REG_n_RMSK)
#define HWIO_TCSR_MUTEX_REG_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), mask)
#define HWIO_TCSR_MUTEX_REG_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_MUTEX_REG_n_ADDR(n),val)
#define HWIO_TCSR_MUTEX_REG_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_MUTEX_REG_n_ADDR(n),mask,val,HWIO_TCSR_MUTEX_REG_n_INI(n))
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_BMSK                0xff
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_SHFT                 0x0


#endif /* __SIMLOCK_HWIO_MACROS_8909_H__ */
