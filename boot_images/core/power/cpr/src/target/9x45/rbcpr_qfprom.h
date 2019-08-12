#ifndef __RBCPR_QFPROM_H__
#define __RBCPR_QFPROM_H__
/*
===========================================================================
*/
/**
  @file rbcpr_qfprom.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE
    TLMM_CSR

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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/power/cpr/src/target/9x45/rbcpr_qfprom.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"



/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                         (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                    0x00000000

#define HWIO_QFPROM_RAW_JTAG_ID_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000000)
#define HWIO_QFPROM_RAW_JTAG_ID_OFFS                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000000)
#define HWIO_QFPROM_RAW_JTAG_ID_RMSK                                                           0xffffffff
#define HWIO_QFPROM_RAW_JTAG_ID_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_ADDR, HWIO_QFPROM_RAW_JTAG_ID_RMSK)
#define HWIO_QFPROM_RAW_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_ADDR, m)
#define HWIO_QFPROM_RAW_JTAG_ID_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_JTAG_ID_ADDR,v)
#define HWIO_QFPROM_RAW_JTAG_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_JTAG_ID_ADDR,m,v,HWIO_QFPROM_RAW_JTAG_ID_IN)
#define HWIO_QFPROM_RAW_JTAG_ID_IDDQ_CX_ON_BMSK                                                0xc0000000
#define HWIO_QFPROM_RAW_JTAG_ID_IDDQ_CX_ON_SHFT                                                      0x1e
#define HWIO_QFPROM_RAW_JTAG_ID_IDDQ_RC_BMSK                                                   0x30000000
#define HWIO_QFPROM_RAW_JTAG_ID_IDDQ_RC_SHFT                                                         0x1c
#define HWIO_QFPROM_RAW_JTAG_ID_FEATURE_ID_BMSK                                                 0xff00000
#define HWIO_QFPROM_RAW_JTAG_ID_FEATURE_ID_SHFT                                                      0x14
#define HWIO_QFPROM_RAW_JTAG_ID_JTAG_ID_BMSK                                                      0xfffff
#define HWIO_QFPROM_RAW_JTAG_ID_JTAG_ID_SHFT                                                          0x0

#define HWIO_QFPROM_RAW_PTE1_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000004)
#define HWIO_QFPROM_RAW_PTE1_OFFS                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000004)
#define HWIO_QFPROM_RAW_PTE1_RMSK                                                              0xffffffff
#define HWIO_QFPROM_RAW_PTE1_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE1_ADDR, HWIO_QFPROM_RAW_PTE1_RMSK)
#define HWIO_QFPROM_RAW_PTE1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE1_ADDR, m)
#define HWIO_QFPROM_RAW_PTE1_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE1_ADDR,v)
#define HWIO_QFPROM_RAW_PTE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE1_ADDR,m,v,HWIO_QFPROM_RAW_PTE1_IN)
#define HWIO_QFPROM_RAW_PTE1_IDDQ_BIN_BMSK                                                     0xe0000000
#define HWIO_QFPROM_RAW_PTE1_IDDQ_BIN_SHFT                                                           0x1d
#define HWIO_QFPROM_RAW_PTE1_IDDQ_MULTIPLIER_BMSK                                              0x1c000000
#define HWIO_QFPROM_RAW_PTE1_IDDQ_MULTIPLIER_SHFT                                                    0x1a
#define HWIO_QFPROM_RAW_PTE1_PROCESS_NODE_ID_BMSK                                               0x2000000
#define HWIO_QFPROM_RAW_PTE1_PROCESS_NODE_ID_SHFT                                                    0x19
#define HWIO_QFPROM_RAW_PTE1_IDDQ_MX_OFF_BMSK                                                   0x1f80000
#define HWIO_QFPROM_RAW_PTE1_IDDQ_MX_OFF_SHFT                                                        0x13
#define HWIO_QFPROM_RAW_PTE1_IDDQ_CX_OFF_BMSK                                                     0x7f000
#define HWIO_QFPROM_RAW_PTE1_IDDQ_CX_OFF_SHFT                                                         0xc
#define HWIO_QFPROM_RAW_PTE1_IDDQ_MX_ON_BMSK                                                        0xfe0
#define HWIO_QFPROM_RAW_PTE1_IDDQ_MX_ON_SHFT                                                          0x5
#define HWIO_QFPROM_RAW_PTE1_IDDQ_CX_ON_BMSK                                                         0x1f
#define HWIO_QFPROM_RAW_PTE1_IDDQ_CX_ON_SHFT                                                          0x0

#define HWIO_QFPROM_RAW_SERIAL_NUM_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000008)
#define HWIO_QFPROM_RAW_SERIAL_NUM_OFFS                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000008)
#define HWIO_QFPROM_RAW_SERIAL_NUM_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_ADDR, HWIO_QFPROM_RAW_SERIAL_NUM_RMSK)
#define HWIO_QFPROM_RAW_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_ADDR, m)
#define HWIO_QFPROM_RAW_SERIAL_NUM_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SERIAL_NUM_ADDR,v)
#define HWIO_QFPROM_RAW_SERIAL_NUM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SERIAL_NUM_ADDR,m,v,HWIO_QFPROM_RAW_SERIAL_NUM_IN)
#define HWIO_QFPROM_RAW_SERIAL_NUM_SERIAL_NUM_BMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_SERIAL_NUM_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PTE2_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000000c)
#define HWIO_QFPROM_RAW_PTE2_OFFS                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000000c)
#define HWIO_QFPROM_RAW_PTE2_RMSK                                                              0xffffffff
#define HWIO_QFPROM_RAW_PTE2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE2_ADDR, HWIO_QFPROM_RAW_PTE2_RMSK)
#define HWIO_QFPROM_RAW_PTE2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE2_ADDR, m)
#define HWIO_QFPROM_RAW_PTE2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE2_ADDR,v)
#define HWIO_QFPROM_RAW_PTE2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE2_ADDR,m,v,HWIO_QFPROM_RAW_PTE2_IN)
#define HWIO_QFPROM_RAW_PTE2_PTE_IDDQ_SPARE_BMSK                                               0xfc000000
#define HWIO_QFPROM_RAW_PTE2_PTE_IDDQ_SPARE_SHFT                                                     0x1a
#define HWIO_QFPROM_RAW_PTE2_IDDQ_MSS_OFF_BMSK                                                  0x3f80000
#define HWIO_QFPROM_RAW_PTE2_IDDQ_MSS_OFF_SHFT                                                       0x13
#define HWIO_QFPROM_RAW_PTE2_IDDQ_MSS_ON_BMSK                                                     0x7f800
#define HWIO_QFPROM_RAW_PTE2_IDDQ_MSS_ON_SHFT                                                         0xb
#define HWIO_QFPROM_RAW_PTE2_FOUNDRY_ID_BMSK                                                        0x700
#define HWIO_QFPROM_RAW_PTE2_FOUNDRY_ID_SHFT                                                          0x8
#define HWIO_QFPROM_RAW_PTE2_LOGIC_RETENTION_BMSK                                                    0xe0
#define HWIO_QFPROM_RAW_PTE2_LOGIC_RETENTION_SHFT                                                     0x5
#define HWIO_QFPROM_RAW_PTE2_SPEED_BIN_BMSK                                                          0x1c
#define HWIO_QFPROM_RAW_PTE2_SPEED_BIN_SHFT                                                           0x2
#define HWIO_QFPROM_RAW_PTE2_MX_RET_BIN_BMSK                                                          0x3
#define HWIO_QFPROM_RAW_PTE2_MX_RET_BIN_SHFT                                                          0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000010)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000010)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE_BMSK                                              0xffc00000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE_SHFT                                                    0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE21_BMSK                                              0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE21_SHFT                                                  0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE20_BMSK                                              0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE20_SHFT                                                  0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_BMSK                                               0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_SHFT                                                  0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE18_BMSK                                               0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE18_SHFT                                                  0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE17_BMSK                                               0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE17_SHFT                                                  0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE16_BMSK                                               0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE16_SHFT                                                  0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_BMSK                                                 0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_SHFT                                                    0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                         0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                            0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_CONFIG_BMSK                                             0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_CONFIG_SHFT                                                0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_BMSK                                                  0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_SHFT                                                     0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PK_HASH_BMSK                                                 0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PK_HASH_SHFT                                                   0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_QC_SEC_BOOT_BMSK                                             0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_QC_SEC_BOOT_SHFT                                               0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                            0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                              0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                  0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                    0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                   0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                    0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                              0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                               0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                               0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                                0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                          0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                           0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                           0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                           0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                           0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                           0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                                0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                                0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_BMSK                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000014)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000014)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE_BMSK                                              0xffc00000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE_SHFT                                                    0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE21_BMSK                                              0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE21_SHFT                                                  0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE20_BMSK                                              0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE20_SHFT                                                  0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_BMSK                                               0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_SHFT                                                  0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE18_BMSK                                               0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE18_SHFT                                                  0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE17_BMSK                                               0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE17_SHFT                                                  0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE16_BMSK                                               0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE16_SHFT                                                  0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_BMSK                                                 0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_SHFT                                                    0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                         0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                            0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_CONFIG_BMSK                                             0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_CONFIG_SHFT                                                0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_BMSK                                                  0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_SHFT                                                     0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PK_HASH_BMSK                                                 0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PK_HASH_SHFT                                                   0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_QC_SEC_BOOT_BMSK                                             0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_QC_SEC_BOOT_SHFT                                               0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                            0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                              0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                  0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                    0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                   0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                    0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                              0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                               0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                               0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                                0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_BMSK                                          0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_SHFT                                           0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_BMSK                                           0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_SHFT                                           0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_BMSK                                           0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_SHFT                                           0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                                0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                                0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_BMSK                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000018)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_OFFS                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000018)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_PIL_SUBSYSTEM0_BMSK                                0xfc000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_PIL_SUBSYSTEM0_SHFT                                      0x1a
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_TZ_BMSK                                             0x3fff000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_TZ_SHFT                                                   0xc
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_SBL1_BMSK                                               0xffe
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_SBL1_SHFT                                                 0x1
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_BMSK                                 0x1
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_SHFT                                 0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000001c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_OFFS                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000001c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_APPSBL0_BMSK                                       0xfffc0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_APPSBL0_SHFT                                             0x12
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_BMSK                                   0x3ffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_SHFT                                       0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000020)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_OFFS                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000020)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_APPSBL1_BMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_APPSBL1_SHFT                                              0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000024)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_OFFS                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000024)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_BMSK                       0xff000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_SHFT                             0x18
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_HYPERVISOR_BMSK                                      0xfff000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_HYPERVISOR_SHFT                                           0xc
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_BMSK                                                0xff0
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_SHFT                                                  0x4
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_APPSBL2_BMSK                                              0xf
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_APPSBL2_SHFT                                              0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000028)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_OFFS                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000028)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MSS_BMSK                                           0xffff0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MSS_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MBA_BMSK                                               0xffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MBA_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000002c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OFFS                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000002c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_SPARE0_BMSK                                        0xffffff00
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_SPARE0_SHFT                                               0x8
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                       0xff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                        0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000030)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OFFS                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000030)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_BMSK                                        0xff000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_SHFT                                              0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_BMSK                              0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_SHFT                                  0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                                 0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                     0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                             0x3c0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                                 0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_BMSK                               0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_SHFT                                  0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_BMSK                               0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_SHFT                                  0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_BMSK                                0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_SHFT                                   0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_BMSK                                0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_SHFT                                   0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_BMSK                                0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_SHFT                                   0xd
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WDOG_EN_BMSK                                           0x1000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WDOG_EN_SHFT                                              0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_BMSK                                   0x800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_SHFT                                     0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALT_SD_PORT_FOR_BOOT_BMSK                               0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALT_SD_PORT_FOR_BOOT_SHFT                                 0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                      0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                        0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_BMSK                                0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_SHFT                                  0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                           0xe0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                            0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                 0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                  0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_BMSK                                  0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_SHFT                                  0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_BMSK                                                0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE_SHFT                                                0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_BMSK                                         0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_SHFT                                         0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                      0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                      0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000034)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OFFS                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000034)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                        0xff800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                              0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                        0x780000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                            0x13
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                             0x40000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                                0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_BMSK                              0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_SHFT                                 0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_BMSK                               0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_SHFT                                  0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                                 0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                    0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                                 0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                    0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_BMSK                              0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_SHFT                                 0xd
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_BMSK                               0x1000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_SHFT                                  0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                                 0x800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                   0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                                 0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                   0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_DISABLE_BMSK                                     0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_DISABLE_SHFT                                       0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE0_DISABLE_BMSK                                     0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE0_DISABLE_SHFT                                       0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_BMSK                               0x80
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_SHFT                                0x7
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_BMSK                                   0x40
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_SHFT                                    0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_WCSS_NIDEN_DISABLE_BMSK                              0x20
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_WCSS_NIDEN_DISABLE_SHFT                               0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_BMSK                                   0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_SHFT                                    0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_BMSK                                   0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_SHFT                                   0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_BMSK                                    0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_SHFT                                    0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_BMSK                                    0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_SHFT                                    0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ALL_DEBUG_DISABLE_BMSK                                    0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ALL_DEBUG_DISABLE_SHFT                                    0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000038)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OFFS                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000038)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_BMSK                                0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_SHFT                                      0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_BMSK                                         0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_SHFT                                            0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000003c)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OFFS                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000003c)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_PERIPH_VID_BMSK                                    0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_PERIPH_VID_SHFT                                          0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_PERIPH_PID_BMSK                                        0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_PERIPH_PID_SHFT                                           0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000040)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000040)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_VU_EN_BMSK                             0xfc000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_VU_EN_SHFT                                   0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_NAV_EN_BMSK                                   0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_NAV_EN_SHFT                                        0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_LDO_EN_BMSK                                   0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_LDO_EN_SHFT                                        0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_ECS_EN_BMSK                                    0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_ECS_EN_SHFT                                        0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_WCDMA_EN_BMSK                            0x600000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_WCDMA_EN_SHFT                                0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_TDSCDMA_EN_BMSK                          0x180000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_TDSCDMA_EN_SHFT                              0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_EN_BMSK                               0x60000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_EN_SHFT                                  0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT2_EN_BMSK                    0x18000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT2_EN_SHFT                        0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT1_EN_BMSK                     0x6000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT1_EN_SHFT                        0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_MIMO_EN_BMSK                          0x1800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_MIMO_EN_SHFT                             0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_EN_BMSK                                0x600
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_EN_SHFT                                  0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_DC_EN_BMSK                             0x180
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_DC_EN_SHFT                               0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_GENRAN_EN_BMSK                               0x60
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_GENRAN_EN_SHFT                                0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_EN_BMSK                                      0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_EN_SHFT                                       0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DO_EN_BMSK                                    0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DO_EN_SHFT                                    0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_1X_EN_BMSK                                    0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_1X_EN_SHFT                                    0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000044)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000044)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                  0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                        0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE_BMSK                                        0x7ff00000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE_SHFT                                              0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_MDSP_FW_EN_BMSK                                 0xfff00
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_MDSP_FW_EN_SHFT                                     0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_SPARE_BMSK                                         0xc0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_SPARE_SHFT                                          0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_VU_EN_BMSK                                   0x3f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_VU_EN_SHFT                                    0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000048)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000048)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ACC_DISABLE_BMSK                                  0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ACC_DISABLE_SHFT                                        0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SHADOW_READ_DISABLE_BMSK                          0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SHADOW_READ_DISABLE_SHFT                                0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                       0x30000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                             0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_SMT_PERM_ENABLE_BMSK                          0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_SMT_PERM_ENABLE_SHFT                               0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_RCP_BYPASS_ENABLE_BMSK                        0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_RCP_BYPASS_ENABLE_SHFT                             0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_SMMU_BYPASS_DISABLE_BMSK                      0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_SMMU_BYPASS_DISABLE_SHFT                           0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_GPIO_SLIMBUS_PD_DISABLE_BMSK                  0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_GPIO_SLIMBUS_PD_DISABLE_SHFT                       0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_PCIE20_RC_DISABLE_BMSK                         0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_FUSE_PCIE20_RC_DISABLE_SHFT                             0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_PCIE_DISABLE_BMSK                              0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_MSMC_PCIE_DISABLE_SHFT                                  0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_NDINT_DISABLE_BMSK                                  0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_NDINT_DISABLE_SHFT                                      0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                        0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                            0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_BMSK                          0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_SHFT                             0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_BMSK                           0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_SHFT                              0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_BMSK                            0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_SHFT                               0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_BMSK                            0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_SHFT                               0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_BMSK                          0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_SHFT                             0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_BMSK                           0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_SHFT                              0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                            0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                               0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                            0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                               0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_BMSK                                 0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_SHFT                                   0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_BMSK                                 0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_SHFT                                   0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                          0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                            0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_BMSK                              0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_SHFT                                0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_WCSS_NIDEN_DISABLE_BMSK                          0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_WCSS_NIDEN_DISABLE_SHFT                           0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_BMSK                               0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_SHFT                                0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                              0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                               0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_BMSK                               0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_SHFT                                0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_BMSK                                0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_SHFT                                0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QDI_SPMI_DISABLE_BMSK                                    0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QDI_SPMI_DISABLE_SHFT                                    0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                     0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                         0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                         0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000004c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000004c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                       0xf8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                             0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                      0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                           0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                             0x3ffc000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                   0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SPARE1_BMSK                                           0x3c00
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SPARE1_SHFT                                              0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_BMSK                           0x3e0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_SHFT                             0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                             0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                              0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000050)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000050)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK             0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                    0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000054)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000054)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE3_BMSK                                       0xf0000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE3_SHFT                                             0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APSS_BOOT_TRIGGER_DISABLE_BMSK                     0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APSS_BOOT_TRIGGER_DISABLE_SHFT                          0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_BMSK                            0x7800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_SHFT                                 0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                              0x780000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                  0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_CFG_BMSK                               0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_CFG_SHFT                                  0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_DELAY_BMSK                             0x30000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_DELAY_SHFT                                0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_BMSK               0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_SHFT                  0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_USB_SS_ENABLE_BMSK                                    0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_USB_SS_ENABLE_SHFT                                       0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_BMSK                              0x3000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_SHFT                                 0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_BMSK                                0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_SHFT                                  0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MSA_ENA_BMSK                                           0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MSA_ENA_SHFT                                             0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_FORCE_MSA_AUTH_EN_BMSK                                 0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_FORCE_MSA_AUTH_EN_SHFT                                   0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE_8_7_BMSK                                         0x180
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE_8_7_SHFT                                           0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_BMSK                                0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_SHFT                                 0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_BMSK                             0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_SHFT                              0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                  0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                   0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000058 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OFFS(n)                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000058 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                            3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                         0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000005c + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OFFS(n)                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000005c + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                            3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                         0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000078 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OFFS(n)                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000078 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                            3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                         0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000007c + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OFFS(n)                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000007c + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                            3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                         0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000098)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000098)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT4_BMSK                                   0xff000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT4_SHFT                                         0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT3_BMSK                                     0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT3_SHFT                                         0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT2_BMSK                                       0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT2_SHFT                                          0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT1_BMSK                                         0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT1_SHFT                                          0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000009c)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000009c)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SPARE0_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SPARE0_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_FEC_VALUE_BMSK                                   0x7f000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_FEC_VALUE_SHFT                                         0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT7_BMSK                                     0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT7_SHFT                                         0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT6_BMSK                                       0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT6_SHFT                                          0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT5_BMSK                                         0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT5_SHFT                                          0x0

#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a0)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000a0)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT4_BMSK                                    0xff000000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT4_SHFT                                          0x18
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT3_BMSK                                      0xff0000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT3_SHFT                                          0x10
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT2_BMSK                                        0xff00
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT2_SHFT                                           0x8
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT1_BMSK                                          0xff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT1_SHFT                                           0x0

#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a4)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000a4)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SPARE0_BMSK                                       0x80000000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SPARE0_SHFT                                             0x1f
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_FEC_VALUE_BMSK                                    0x7f000000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_FEC_VALUE_SHFT                                          0x18
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT7_BMSK                                      0xff0000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT7_SHFT                                          0x10
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT6_BMSK                                        0xff00
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT6_SHFT                                           0x8
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT5_BMSK                                          0xff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT5_SHFT                                           0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n)                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_OFFS(n)                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000a8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_MAXn                                                           3
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_HASH_DATA0_BMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_HASH_DATA0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n)                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000ac + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_OFFS(n)                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000ac + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_MAXn                                                           3
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                         0xffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                              0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000c8)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000c8)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_HASH_DATA0_BMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_HASH_DATA0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000cc)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000cc)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_RMSK                                                  0xff000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_FEC_VALUE_SHFT                                              0x18

#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d0)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000d0)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS2_POINT1_BMSK                                      0xf0000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS2_POINT1_SHFT                                            0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS1_POINT1_BMSK                                       0xf000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS1_POINT1_SHFT                                            0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS0_POINT1_BMSK                                        0xf00000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS0_POINT1_SHFT                                            0x14
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS_BASE1_BMSK                                           0xffc00
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS_BASE1_SHFT                                               0xa
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS_BASE0_BMSK                                             0x3ff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_TSENS_BASE0_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d4)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000d4)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_FEC_BMSK                                                0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_FEC_SHFT                                                      0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_USB_PHY_TUNING_BMSK                                       0xf00000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_USB_PHY_TUNING_SHFT                                           0x14
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_SPARE_BMSK                                                 0xff800
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_SPARE_SHFT                                                     0xb
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS_CAL_SEL_BMSK                                           0x700
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS_CAL_SEL_SHFT                                             0x8
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS4_POINT1_BMSK                                            0xf0
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS4_POINT1_SHFT                                             0x4
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS3_POINT1_BMSK                                             0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_TSENS3_POINT1_SHFT                                             0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d8)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000d8)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_0_BMSK                    0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_0_SHFT                          0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_SVS2_BMSK                                0x7c000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_SVS2_SHFT                                      0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_1_BMSK                     0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_1_SHFT                          0x19
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_SVS_BMSK                                  0x1f00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_SVS_SHFT                                       0x14
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_2_BMSK                       0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_2_SHFT                          0x13
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_NOM_BMSK                                    0x7c000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_NOM_SHFT                                        0xe
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_3_BMSK                        0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_3_SHFT                           0xd
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_TUR_BMSK                                     0x1f00
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR0_TARG_VOLT_TUR_SHFT                                        0x8
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR1_TARG_VOLT_OFFSET_SVS2_BMSK                               0x80
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR1_TARG_VOLT_OFFSET_SVS2_SHFT                                0x7
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_TARG_VOLT_OFFSET_NOM_BMSK                                0x78
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_TARG_VOLT_OFFSET_NOM_SHFT                                 0x3
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_TARG_VOLT_OFFSET_TUR_BMSK                                 0x7
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_CPR2_TARG_VOLT_OFFSET_TUR_SHFT                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000dc)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000dc)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_FEC_BMSK                                                0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_FEC_SHFT                                                      0x18
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_0_BMSK                      0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_0_SHFT                          0x17
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_TARG_VOLT_SVS2_BMSK                                  0x7c0000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_TARG_VOLT_SVS2_SHFT                                      0x12
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_1_BMSK                       0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_1_SHFT                          0x11
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_TARG_VOLT_SVS_BMSK                                    0x1f000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_TARG_VOLT_SVS_SHFT                                        0xc
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_2_BMSK                         0x800
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_2_SHFT                           0xb
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_TARG_VOLT_NOM_BMSK                                      0x7c0
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_TARG_VOLT_NOM_SHFT                                        0x6
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_3_BMSK                          0x20
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_3_SHFT                           0x5
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_TARG_VOLT_TUR_BMSK                                       0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CPR1_TARG_VOLT_TUR_SHFT                                        0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e0)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000e0)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_TARGET1_BMSK                                        0xf0000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_TARGET1_SHFT                                              0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_TARGET0_BMSK                                         0xfc00000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_TARGET0_SHFT                                              0x16
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_ENABLE_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_LDO_ENABLE_SHFT                                               0x15
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS2_BMSK                           0x1e0000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS2_SHFT                               0x11
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS_BMSK                             0x1c000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS_SHFT                                 0xe
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_TARG_VOLT_NOM_BMSK                                     0x3e00
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_TARG_VOLT_NOM_SHFT                                        0x9
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS_BIT_3_BMSK                         0x100
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS_BIT_3_SHFT                           0x8
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_TARG_VOLT_TUR_BMSK                                       0xf8
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR2_TARG_VOLT_TUR_SHFT                                        0x3
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR_REV_BMSK                                                   0x7
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_CPR_REV_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e4)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000e4)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_FEC_BMSK                                                0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_FEC_SHFT                                                      0x18
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_SVS2_BMSK                           0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_SVS2_SHFT                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_SVS_BMSK                            0x3c0000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_SVS_SHFT                                0x12
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_NOM_BMSK                             0x3c000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_NOM_SHFT                                 0xe
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_TUR_BMSK                              0x3c00
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_TUR_SHFT                                 0xa
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_SPARE_BMSK                                                   0x300
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_SPARE_SHFT                                                     0x8
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_GNSS_ADC_CH0_VCM_BMSK                                         0xc0
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_GNSS_ADC_CH0_VCM_SHFT                                          0x6
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_GNSS_ADC_CH0_LDO_BMSK                                         0x30
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_GNSS_ADC_CH0_LDO_SHFT                                          0x4
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_GNSS_ADC_CH0_VREF_BMSK                                         0xc
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_GNSS_ADC_CH0_VREF_SHFT                                         0x2
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_LDO_TARGET1_BMSK                                               0x3
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_LDO_TARGET1_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e8)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000e8)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_BHS_SLOPE_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_BHS_SLOPE_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_BHS_INTERCEPT_BMSK                                  0x7fe00000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_BHS_INTERCEPT_SHFT                                        0x15
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_LDO_SLOPE_BMSK                                        0x1ff800
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_LDO_SLOPE_SHFT                                             0xb
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_LDO_INTERCEPT_BMSK                                       0x7fe
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_LDO_INTERCEPT_SHFT                                         0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_ENABLE_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ECS_ENABLE_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000ec)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000ec)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_FEC_BMSK                                                0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_FEC_SHFT                                                      0x18
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_Q6SS0_LDO_VREF_TRIM_BMSK                                  0xf80000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_Q6SS0_LDO_VREF_TRIM_SHFT                                      0x13
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_TUR_BIT_3_BMSK                       0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_TUR_BIT_3_SHFT                          0x12
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR1_TARG_VOLT_OFFSET_SVS2_BIT_1_BMSK                      0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_CPR1_TARG_VOLT_OFFSET_SVS2_BIT_1_SHFT                         0x11
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ECS_MISC_BMSK                                              0x1fe00
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ECS_MISC_SHFT                                                  0x9
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ECS_BHS_SLOPE_BMSK                                           0x1ff
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ECS_BHS_SLOPE_SHFT                                             0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f0)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000f0)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M2_BMSK                                            0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M2_SHFT                                                  0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M1_BMSK                                            0x38000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M1_SHFT                                                  0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M0_BMSK                                             0x7000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B1M0_SHFT                                                  0x18
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_G_B1_BMSK                                                 0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_G_B1_SHFT                                                     0x15
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_SAR_LDO_ERR0_BMSK                                         0x180000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_SAR_LDO_ERR0_SHFT                                             0x13
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CLK_LDO_ERR0_BMSK                                          0x60000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_CLK_LDO_ERR0_SHFT                                             0x11
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_VREF_ERR_B0_BMSK                                           0x18000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_VREF_ERR_B0_SHFT                                               0xf
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M3_BMSK                                                0x7000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M3_SHFT                                                   0xc
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M2_BMSK                                                 0xe00
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M2_SHFT                                                   0x9
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M1_BMSK                                                 0x1c0
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M1_SHFT                                                   0x6
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M0_BMSK                                                  0x38
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_PH_B0M0_SHFT                                                   0x3
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_G_B0_BMSK                                                      0x7
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_G_B0_SHFT                                                      0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f4)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000f4)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_FEC_BMSK                                                0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_FEC_SHFT                                                      0x18
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M3_BMSK                                              0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M3_SHFT                                                  0x16
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M2_BMSK                                              0x380000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M2_SHFT                                                  0x13
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M1_BMSK                                               0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M1_SHFT                                                  0x10
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M0_BMSK                                                0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B2M0_SHFT                                                   0xd
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_G_B2_BMSK                                                   0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_G_B2_SHFT                                                      0xa
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_SAR_LDO_ERR1_BMSK                                            0x300
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_SAR_LDO_ERR1_SHFT                                              0x8
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CLK_LDO_ERR1_BMSK                                             0xc0
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_CLK_LDO_ERR1_SHFT                                              0x6
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_VREF_ERR_B1_BMSK                                              0x30
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_VREF_ERR_B1_SHFT                                               0x4
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B1M3_BMSK                                                   0xe
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B1M3_SHFT                                                   0x1
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B1M2_BMSK                                                   0x1
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_PH_B1M2_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f8)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000f8)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B4M0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B4M0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_G_B4_BMSK                                               0x70000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_G_B4_SHFT                                                     0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_SAR_LDO_ERR3_BMSK                                        0xc000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_SAR_LDO_ERR3_SHFT                                             0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CLK_LDO_ERR3_BMSK                                        0x3000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CLK_LDO_ERR3_SHFT                                             0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_VREF_ERR_B3_BMSK                                          0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_VREF_ERR_B3_SHFT                                              0x16
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M3_BMSK                                              0x380000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M3_SHFT                                                  0x13
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M2_BMSK                                               0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M2_SHFT                                                  0x10
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M1_BMSK                                                0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M1_SHFT                                                   0xd
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M0_BMSK                                                0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B3M0_SHFT                                                   0xa
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_G_B3_BMSK                                                    0x380
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_G_B3_SHFT                                                      0x7
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_SAR_LDO_ERR2_BMSK                                             0x60
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_SAR_LDO_ERR2_SHFT                                              0x5
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CLK_LDO_ERR2_BMSK                                             0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CLK_LDO_ERR2_SHFT                                              0x3
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_VREF_ERR_B2_BMSK                                               0x6
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_VREF_ERR_B2_SHFT                                               0x1
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B2M3_BMSK                                                   0x1
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_PH_B2M3_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000fc)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000fc)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_FEC_BMSK                                                0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_FEC_SHFT                                                      0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B5M1_BMSK                                              0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B5M1_SHFT                                                  0x17
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B5M0_BMSK                                              0x700000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B5M0_SHFT                                                  0x14
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_G_B5_BMSK                                                  0xe0000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_G_B5_SHFT                                                     0x11
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_SAR_LDO_ERR4_BMSK                                          0x18000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_SAR_LDO_ERR4_SHFT                                              0xf
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CLK_LDO_ERR4_BMSK                                           0x6000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CLK_LDO_ERR4_SHFT                                              0xd
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_VREF_ERR_B4_BMSK                                            0x1800
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_VREF_ERR_B4_SHFT                                               0xb
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M3_BMSK                                                 0x700
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M3_SHFT                                                   0x8
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M2_BMSK                                                  0xe0
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M2_SHFT                                                   0x5
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M1_BMSK                                                  0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M1_SHFT                                                   0x2
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M0_BMSK                                                   0x3
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_PH_B4M0_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000100)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000100)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CLK_LDO_ERR6_BMSK                                       0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CLK_LDO_ERR6_SHFT                                             0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_ERR_B6_BMSK                                        0x60000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_ERR_B6_SHFT                                              0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M3_BMSK                                            0x1c000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M3_SHFT                                                  0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M2_BMSK                                             0x3800000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M2_SHFT                                                  0x17
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M1_BMSK                                              0x700000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M1_SHFT                                                  0x14
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M0_BMSK                                               0xe0000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B6M0_SHFT                                                  0x11
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_G_B6_BMSK                                                  0x1c000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_G_B6_SHFT                                                      0xe
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_SAR_LDO_ERR5_BMSK                                           0x3000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_SAR_LDO_ERR5_SHFT                                              0xc
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CLK_LDO_ERR5_BMSK                                            0xc00
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CLK_LDO_ERR5_SHFT                                              0xa
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_ERR_B5_BMSK                                             0x300
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_VREF_ERR_B5_SHFT                                               0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M3_BMSK                                                  0xe0
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M3_SHFT                                                   0x5
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M2_BMSK                                                  0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M2_SHFT                                                   0x2
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M1_BMSK                                                   0x3
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_PH_B5M1_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000104)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OFFS                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000104)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_FEC_BMSK                                                0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_FEC_SHFT                                                      0x18
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_BMSK                                   0xff0000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_SHFT                                       0x10
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_BMSK                                     0xff00
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_SHFT                                        0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_AVG_ERROR_BMSK                             0x80
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_AVG_ERROR_SHFT                              0x7
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_AVG_ERROR_BMSK                             0x40
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_AVG_ERROR_SHFT                              0x6
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC_0_1_FUSEFLAG_BMSK                                 0x20
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC_0_1_FUSEFLAG_SHFT                                  0x5
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_OVERFLOW_BMSK                              0x10
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_OVERFLOW_SHFT                               0x4
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_OVERFLOW_BMSK                               0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_OVERFLOW_SHFT                               0x3
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_SAR_LDO_ERR6_BMSK                                              0x6
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_SAR_LDO_ERR6_SHFT                                              0x1
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CLK_LDO_ERR6_BMSK                                              0x1
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_CLK_LDO_ERR6_SHFT                                              0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000108 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_OFFS(n)                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000108 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_MAXn                                                       15
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_REDUN_DATA_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_REDUN_DATA_SHFT                                           0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000010c + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_OFFS(n)                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000010c + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_MAXn                                                       15
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                      0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                           0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000188)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000188)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_MEM_ACCEL_COMPILER_BMSK                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_MEM_ACCEL_COMPILER_SHFT                                  0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000018c)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000018c)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_RMSK                                              0x7fffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_FEC_VALUE_BMSK                                    0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_FEC_VALUE_SHFT                                          0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_SPARE0_BMSK                                         0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_SPARE0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000190)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000190)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_CUSTOM_BMSK                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_CUSTOM_SHFT                                    0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000194)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000194)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_RMSK                                              0x7fffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_FEC_VALUE_BMSK                                    0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_FEC_VALUE_SHFT                                          0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_SPARE0_BMSK                                         0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_SPARE0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000198)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000198)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_CUSTOM_BMSK                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_CUSTOM_SHFT                                    0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000019c)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000019c)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_RMSK                                              0x7fffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_FEC_VALUE_BMSK                                    0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_FEC_VALUE_SHFT                                          0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_SPARE0_BMSK                                         0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_SPARE0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a0)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001a0)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_MEM_ACCEL_COMPILER_BMSK                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_LSB_MEM_ACCEL_COMPILER_SHFT                                  0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a4)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001a4)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_RMSK                                              0x7fffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_FEC_VALUE_BMSK                                    0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_FEC_VALUE_SHFT                                          0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_SPARE0_BMSK                                         0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW19_MSB_SPARE0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a8)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001a8)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_MEM_ACCEL_CUSTOM_BMSK                             0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_LSB_MEM_ACCEL_CUSTOM_SHFT                                    0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001ac)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001ac)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_RMSK                                              0x7fffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_FEC_VALUE_BMSK                                    0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_FEC_VALUE_SHFT                                          0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_SPARE0_BMSK                                         0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW20_MSB_SPARE0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OFFS(n)                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001b0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_MAXn                                                        31
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_DATA_BMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_DATA_SHFT                                            0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OFFS(n)                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001b4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_MAXn                                                        31
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_SPARE3_BMSK                                           0xfe0000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_SPARE3_SHFT                                               0x11
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_ADDR_BMSK                                        0x1fffe
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_ADDR_SHFT                                            0x1
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                              0x1
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                              0x0

#define HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002b0)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OFFS                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002b0)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_LSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION31_FEC_EN_BMSK                                        0x80000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION31_FEC_EN_SHFT                                              0x1f
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION30_FEC_EN_BMSK                                        0x40000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION30_FEC_EN_SHFT                                              0x1e
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION29_FEC_EN_BMSK                                        0x20000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION29_FEC_EN_SHFT                                              0x1d
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION28_FEC_EN_BMSK                                        0x10000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION28_FEC_EN_SHFT                                              0x1c
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                         0x8000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                              0x1b
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                         0x4000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                              0x1a
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                         0x2000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                              0x19
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                         0x1000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                              0x18
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                          0x800000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                              0x17
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                          0x400000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                              0x16
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                          0x200000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                              0x15
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                          0x100000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                              0x14
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                           0x80000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                              0x13
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                           0x40000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                              0x12
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                           0x20000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                              0x11
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                           0x10000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                              0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                            0x8000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                               0xf
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                            0x4000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                               0xe
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                            0x2000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                               0xd
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                            0x1000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                               0xc
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                             0x800
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                               0xb
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                             0x400
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                               0xa
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                              0x200
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                                0x9
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                              0x100
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                                0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                               0x80
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                                0x7
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                               0x40
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                                0x6
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                               0x20
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                                0x5
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                               0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                                0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                                0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                                0x3
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                                0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                                0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                                0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                                0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                                0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                                0x0

#define HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002b4)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OFFS                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002b4)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_MSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_FEC_EN_REDUNDANCY_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_FEC_EN_REDUNDANCY_SHFT                                             0x0

#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002b8)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002b8)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG16_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG16_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG16_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_SPARE0_BMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_LSB_SPARE0_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002bc)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002bc)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG16_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG16_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG16_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_SPARE1_BMSK                                              0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG16_MSB_SPARE1_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002c0)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002c0)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG17_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG17_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG17_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_SPARE0_BMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_LSB_SPARE0_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002c4)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002c4)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG17_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG17_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG17_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_SPARE1_BMSK                                              0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG17_MSB_SPARE1_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002c8)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002c8)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG18_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG18_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG18_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_SPARE0_BMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_SPARE0_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002cc)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002cc)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG18_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG18_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG18_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_SPARE1_BMSK                                              0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_SPARE1_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002d0)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002d0)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_SPARE0_BMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_SPARE0_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002d4)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002d4)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE1_BMSK                                              0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE1_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002d8)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002d8)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG20_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG20_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG20_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_SPARE0_BMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_SPARE0_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002dc)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002dc)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG20_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG20_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG20_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_SPARE1_BMSK                                              0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_SPARE1_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002e0)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002e0)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG21_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG21_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG21_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_SPARE0_BMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_SPARE0_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002e4)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000002e4)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG21_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG21_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG21_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_SPARE0_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_SPARE0_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_SPARE1_BMSK                                              0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_SPARE1_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000400 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_OFFS(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000400 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_MAXn                                                              39
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n), HWIO_QFPROM_RAW_ACC_PRIVATEn_RMSK)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ACC_PRIVATE_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ACC_PRIVATE_SHFT                                                 0x0

#define HWIO_ACC_IR_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002000)
#define HWIO_ACC_IR_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002000)
#define HWIO_ACC_IR_RMSK                                                                             0x1f
#define HWIO_ACC_IR_OUT(v)      \
        out_dword(HWIO_ACC_IR_ADDR,v)
#define HWIO_ACC_IR_INSTRUCTION_BMSK                                                                 0x1f
#define HWIO_ACC_IR_INSTRUCTION_SHFT                                                                  0x0

#define HWIO_ACC_DR_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002004)
#define HWIO_ACC_DR_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002004)
#define HWIO_ACC_DR_RMSK                                                                       0xffffffff
#define HWIO_ACC_DR_IN          \
        in_dword_masked(HWIO_ACC_DR_ADDR, HWIO_ACC_DR_RMSK)
#define HWIO_ACC_DR_INM(m)      \
        in_dword_masked(HWIO_ACC_DR_ADDR, m)
#define HWIO_ACC_DR_OUT(v)      \
        out_dword(HWIO_ACC_DR_ADDR,v)
#define HWIO_ACC_DR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_ACC_DR_ADDR,m,v,HWIO_ACC_DR_IN)
#define HWIO_ACC_DR_DR_BMSK                                                                    0xffffffff
#define HWIO_ACC_DR_DR_SHFT                                                                           0x0

#define HWIO_ACC_VERID_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002008)
#define HWIO_ACC_VERID_OFFS                                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002008)
#define HWIO_ACC_VERID_RMSK                                                                        0xffff
#define HWIO_ACC_VERID_IN          \
        in_dword_masked(HWIO_ACC_VERID_ADDR, HWIO_ACC_VERID_RMSK)
#define HWIO_ACC_VERID_INM(m)      \
        in_dword_masked(HWIO_ACC_VERID_ADDR, m)
#define HWIO_ACC_VERID_FWVERID_BMSK                                                                0xff00
#define HWIO_ACC_VERID_FWVERID_SHFT                                                                   0x8
#define HWIO_ACC_VERID_HWVERID_BMSK                                                                  0xff
#define HWIO_ACC_VERID_HWVERID_SHFT                                                                   0x0

#define HWIO_ACC_FEATSETn_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_OFFS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_RMSK                                                                 0xffffffff
#define HWIO_ACC_FEATSETn_MAXn                                                                          7
#define HWIO_ACC_FEATSETn_INI(n)        \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(n), HWIO_ACC_FEATSETn_RMSK)
#define HWIO_ACC_FEATSETn_INMI(n,mask)    \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(n), mask)
#define HWIO_ACC_FEATSETn_FEAT_BMSK                                                            0xffffffff
#define HWIO_ACC_FEATSETn_FEAT_SHFT                                                                   0x0

#define HWIO_ACC_STATE_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002038)
#define HWIO_ACC_STATE_OFFS                                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002038)
#define HWIO_ACC_STATE_RMSK                                                                           0x7
#define HWIO_ACC_STATE_IN          \
        in_dword_masked(HWIO_ACC_STATE_ADDR, HWIO_ACC_STATE_RMSK)
#define HWIO_ACC_STATE_INM(m)      \
        in_dword_masked(HWIO_ACC_STATE_ADDR, m)
#define HWIO_ACC_STATE_ACC_READY_BMSK                                                                 0x4
#define HWIO_ACC_STATE_ACC_READY_SHFT                                                                 0x2
#define HWIO_ACC_STATE_ACC_LOCKED_BMSK                                                                0x2
#define HWIO_ACC_STATE_ACC_LOCKED_SHFT                                                                0x1
#define HWIO_ACC_STATE_ACC_STOP_BMSK                                                                  0x1
#define HWIO_ACC_STATE_ACC_STOP_SHFT                                                                  0x0

#define HWIO_QFPROM_BLOW_TIMER_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000203c)
#define HWIO_QFPROM_BLOW_TIMER_OFFS                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000203c)
#define HWIO_QFPROM_BLOW_TIMER_RMSK                                                                 0xfff
#define HWIO_QFPROM_BLOW_TIMER_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, HWIO_QFPROM_BLOW_TIMER_RMSK)
#define HWIO_QFPROM_BLOW_TIMER_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, m)
#define HWIO_QFPROM_BLOW_TIMER_OUT(v)      \
        out_dword(HWIO_QFPROM_BLOW_TIMER_ADDR,v)
#define HWIO_QFPROM_BLOW_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BLOW_TIMER_ADDR,m,v,HWIO_QFPROM_BLOW_TIMER_IN)
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_BMSK                                                      0xfff
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_SHFT                                                        0x0

#define HWIO_QFPROM_TEST_CTRL_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002040)
#define HWIO_QFPROM_TEST_CTRL_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002040)
#define HWIO_QFPROM_TEST_CTRL_RMSK                                                                    0xf
#define HWIO_QFPROM_TEST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, HWIO_QFPROM_TEST_CTRL_RMSK)
#define HWIO_QFPROM_TEST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, m)
#define HWIO_QFPROM_TEST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_TEST_CTRL_ADDR,v)
#define HWIO_QFPROM_TEST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_TEST_CTRL_ADDR,m,v,HWIO_QFPROM_TEST_CTRL_IN)
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_BMSK                                                        0x8
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_SHFT                                                        0x3
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_BMSK                                                         0x4
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_SHFT                                                         0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_BMSK                                                         0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_SHFT                                                         0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_BMSK                                                   0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_SHFT                                                   0x0

#define HWIO_QFPROM_ACCEL_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002044)
#define HWIO_QFPROM_ACCEL_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002044)
#define HWIO_QFPROM_ACCEL_RMSK                                                                      0xfff
#define HWIO_QFPROM_ACCEL_IN          \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, HWIO_QFPROM_ACCEL_RMSK)
#define HWIO_QFPROM_ACCEL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, m)
#define HWIO_QFPROM_ACCEL_OUT(v)      \
        out_dword(HWIO_QFPROM_ACCEL_ADDR,v)
#define HWIO_QFPROM_ACCEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_ACCEL_ADDR,m,v,HWIO_QFPROM_ACCEL_IN)
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_BMSK                                                      0x800
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_SHFT                                                        0xb
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_BMSK                                                    0x700
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_SHFT                                                      0x8
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_BMSK                                                          0xff
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_SHFT                                                           0x0

#define HWIO_QFPROM_BLOW_STATUS_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_OFFS                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_RMSK                                                                  0x3
#define HWIO_QFPROM_BLOW_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, HWIO_QFPROM_BLOW_STATUS_RMSK)
#define HWIO_QFPROM_BLOW_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, m)
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_BMSK                                                    0x2
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_SHFT                                                    0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_BMSK                                                      0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_SHFT                                                      0x0

#define HWIO_QFPROM_ROM_ERROR_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000204c)
#define HWIO_QFPROM_ROM_ERROR_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000204c)
#define HWIO_QFPROM_ROM_ERROR_RMSK                                                                    0x1
#define HWIO_QFPROM_ROM_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, HWIO_QFPROM_ROM_ERROR_RMSK)
#define HWIO_QFPROM_ROM_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, m)
#define HWIO_QFPROM_ROM_ERROR_ERROR_BMSK                                                              0x1
#define HWIO_QFPROM_ROM_ERROR_ERROR_SHFT                                                              0x0

#define HWIO_QFPROM_BIST_CTRL_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_RMSK                                                                   0x7f
#define HWIO_QFPROM_BIST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, HWIO_QFPROM_BIST_CTRL_RMSK)
#define HWIO_QFPROM_BIST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, m)
#define HWIO_QFPROM_BIST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_BIST_CTRL_ADDR,v)
#define HWIO_QFPROM_BIST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BIST_CTRL_ADDR,m,v,HWIO_QFPROM_BIST_CTRL_IN)
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_BMSK                                                       0x7c
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_SHFT                                                        0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_BMSK                                                         0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_SHFT                                                         0x1
#define HWIO_QFPROM_BIST_CTRL_START_BMSK                                                              0x1
#define HWIO_QFPROM_BIST_CTRL_START_SHFT                                                              0x0

#define HWIO_QFPROM_BIST_ERROR_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_OFFS                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_RMSK                                                            0xffffffff
#define HWIO_QFPROM_BIST_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, HWIO_QFPROM_BIST_ERROR_RMSK)
#define HWIO_QFPROM_BIST_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR_ERROR_BMSK                                                      0xffffffff
#define HWIO_QFPROM_BIST_ERROR_ERROR_SHFT                                                             0x0

#define HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_OFFS(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_RMSK                                                       0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_MAXn                                                                7
#define HWIO_QFPROM_HASH_SIGNATUREn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), HWIO_QFPROM_HASH_SIGNATUREn_RMSK)
#define HWIO_QFPROM_HASH_SIGNATUREn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), mask)
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_BMSK                                            0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_SHFT                                                   0x0

#define HWIO_HW_KEY_STATUS_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002080)
#define HWIO_HW_KEY_STATUS_OFFS                                                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002080)
#define HWIO_HW_KEY_STATUS_RMSK                                                                      0x1f
#define HWIO_HW_KEY_STATUS_IN          \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, HWIO_HW_KEY_STATUS_RMSK)
#define HWIO_HW_KEY_STATUS_INM(m)      \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, m)
#define HWIO_HW_KEY_STATUS_KDF_DONE_BMSK                                                             0x10
#define HWIO_HW_KEY_STATUS_KDF_DONE_SHFT                                                              0x4
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_BMSK                                                      0x8
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_SHFT                                                      0x3
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_BMSK                                                     0x4
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_SHFT                                                     0x2
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_BMSK                                          0x2
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_SHFT                                          0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_BMSK                                          0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_SHFT                                          0x0

#define HWIO_RESET_JDR_STATUS_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002084)
#define HWIO_RESET_JDR_STATUS_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002084)
#define HWIO_RESET_JDR_STATUS_RMSK                                                                    0x3
#define HWIO_RESET_JDR_STATUS_IN          \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, HWIO_RESET_JDR_STATUS_RMSK)
#define HWIO_RESET_JDR_STATUS_INM(m)      \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, m)
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_BMSK                                                        0x2
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_SHFT                                                        0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_BMSK                                               0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_SHFT                                               0x0

#define HWIO_FEC_ESR_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002090)
#define HWIO_FEC_ESR_OFFS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002090)
#define HWIO_FEC_ESR_RMSK                                                                           0xfff
#define HWIO_FEC_ESR_IN          \
        in_dword_masked(HWIO_FEC_ESR_ADDR, HWIO_FEC_ESR_RMSK)
#define HWIO_FEC_ESR_INM(m)      \
        in_dword_masked(HWIO_FEC_ESR_ADDR, m)
#define HWIO_FEC_ESR_OUT(v)      \
        out_dword(HWIO_FEC_ESR_ADDR,v)
#define HWIO_FEC_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FEC_ESR_ADDR,m,v,HWIO_FEC_ESR_IN)
#define HWIO_FEC_ESR_CORR_SW_ACC_BMSK                                                               0x800
#define HWIO_FEC_ESR_CORR_SW_ACC_SHFT                                                                 0xb
#define HWIO_FEC_ESR_CORR_BOOT_ROM_BMSK                                                             0x200
#define HWIO_FEC_ESR_CORR_BOOT_ROM_SHFT                                                               0x9
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_BMSK                                                           0x100
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_SHFT                                                             0x8
#define HWIO_FEC_ESR_CORR_MULT_BMSK                                                                  0x80
#define HWIO_FEC_ESR_CORR_MULT_SHFT                                                                   0x7
#define HWIO_FEC_ESR_CORR_SEEN_BMSK                                                                  0x40
#define HWIO_FEC_ESR_CORR_SEEN_SHFT                                                                   0x6
#define HWIO_FEC_ESR_ERR_SW_ACC_BMSK                                                                 0x20
#define HWIO_FEC_ESR_ERR_SW_ACC_SHFT                                                                  0x5
#define HWIO_FEC_ESR_ERR_BOOT_ROM_BMSK                                                                0x8
#define HWIO_FEC_ESR_ERR_BOOT_ROM_SHFT                                                                0x3
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_BMSK                                                              0x4
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_SHFT                                                              0x2
#define HWIO_FEC_ESR_ERR_MULT_BMSK                                                                    0x2
#define HWIO_FEC_ESR_ERR_MULT_SHFT                                                                    0x1
#define HWIO_FEC_ESR_ERR_SEEN_BMSK                                                                    0x1
#define HWIO_FEC_ESR_ERR_SEEN_SHFT                                                                    0x0

#define HWIO_FEC_EAR_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002094)
#define HWIO_FEC_EAR_OFFS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002094)
#define HWIO_FEC_EAR_RMSK                                                                      0xffffffff
#define HWIO_FEC_EAR_IN          \
        in_dword_masked(HWIO_FEC_EAR_ADDR, HWIO_FEC_EAR_RMSK)
#define HWIO_FEC_EAR_INM(m)      \
        in_dword_masked(HWIO_FEC_EAR_ADDR, m)
#define HWIO_FEC_EAR_CORR_ADDR_BMSK                                                            0xffff0000
#define HWIO_FEC_EAR_CORR_ADDR_SHFT                                                                  0x10
#define HWIO_FEC_EAR_ERR_ADDR_BMSK                                                                 0xffff
#define HWIO_FEC_EAR_ERR_ADDR_SHFT                                                                    0x0

#define HWIO_QFPROM0_MATCH_STATUS_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002098)
#define HWIO_QFPROM0_MATCH_STATUS_OFFS                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002098)
#define HWIO_QFPROM0_MATCH_STATUS_RMSK                                                         0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_ADDR, HWIO_QFPROM0_MATCH_STATUS_RMSK)
#define HWIO_QFPROM0_MATCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_ADDR, m)
#define HWIO_QFPROM0_MATCH_STATUS_FLAG_BMSK                                                    0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_FLAG_SHFT                                                           0x0

#define HWIO_QFPROM1_MATCH_STATUS_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000209c)
#define HWIO_QFPROM1_MATCH_STATUS_OFFS                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000209c)
#define HWIO_QFPROM1_MATCH_STATUS_RMSK                                                         0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_ADDR, HWIO_QFPROM1_MATCH_STATUS_RMSK)
#define HWIO_QFPROM1_MATCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_ADDR, m)
#define HWIO_QFPROM1_MATCH_STATUS_FLAG_BMSK                                                    0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_FLAG_SHFT                                                           0x0

#define HWIO_QFPROM_CORR_JTAG_ID_ADDR                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004000)
#define HWIO_QFPROM_CORR_JTAG_ID_OFFS                                                          (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004000)
#define HWIO_QFPROM_CORR_JTAG_ID_RMSK                                                          0xffffffff
#define HWIO_QFPROM_CORR_JTAG_ID_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_ADDR, HWIO_QFPROM_CORR_JTAG_ID_RMSK)
#define HWIO_QFPROM_CORR_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_ADDR, m)
#define HWIO_QFPROM_CORR_JTAG_ID_IDDQ_CX_ON_BMSK                                               0xc0000000
#define HWIO_QFPROM_CORR_JTAG_ID_IDDQ_CX_ON_SHFT                                                     0x1e
#define HWIO_QFPROM_CORR_JTAG_ID_IDDQ_RC_BMSK                                                  0x30000000
#define HWIO_QFPROM_CORR_JTAG_ID_IDDQ_RC_SHFT                                                        0x1c
#define HWIO_QFPROM_CORR_JTAG_ID_FEATURE_ID_BMSK                                                0xff00000
#define HWIO_QFPROM_CORR_JTAG_ID_FEATURE_ID_SHFT                                                     0x14
#define HWIO_QFPROM_CORR_JTAG_ID_JTAG_ID_BMSK                                                     0xfffff
#define HWIO_QFPROM_CORR_JTAG_ID_JTAG_ID_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_PTE1_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004004)
#define HWIO_QFPROM_CORR_PTE1_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004004)
#define HWIO_QFPROM_CORR_PTE1_RMSK                                                             0xffffffff
#define HWIO_QFPROM_CORR_PTE1_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, HWIO_QFPROM_CORR_PTE1_RMSK)
#define HWIO_QFPROM_CORR_PTE1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, m)
#define HWIO_QFPROM_CORR_PTE1_IDDQ_BIN_BMSK                                                    0xe0000000
#define HWIO_QFPROM_CORR_PTE1_IDDQ_BIN_SHFT                                                          0x1d
#define HWIO_QFPROM_CORR_PTE1_IDDQ_MULTIPLIER_BMSK                                             0x1c000000
#define HWIO_QFPROM_CORR_PTE1_IDDQ_MULTIPLIER_SHFT                                                   0x1a
#define HWIO_QFPROM_CORR_PTE1_PROCESS_NODE_ID_BMSK                                              0x2000000
#define HWIO_QFPROM_CORR_PTE1_PROCESS_NODE_ID_SHFT                                                   0x19
#define HWIO_QFPROM_CORR_PTE1_IDDQ_MX_OFF_BMSK                                                  0x1f80000
#define HWIO_QFPROM_CORR_PTE1_IDDQ_MX_OFF_SHFT                                                       0x13
#define HWIO_QFPROM_CORR_PTE1_IDDQ_CX_OFF_BMSK                                                    0x7f000
#define HWIO_QFPROM_CORR_PTE1_IDDQ_CX_OFF_SHFT                                                        0xc
#define HWIO_QFPROM_CORR_PTE1_IDDQ_MX_ON_BMSK                                                       0xfe0
#define HWIO_QFPROM_CORR_PTE1_IDDQ_MX_ON_SHFT                                                         0x5
#define HWIO_QFPROM_CORR_PTE1_IDDQ_CX_ON_BMSK                                                        0x1f
#define HWIO_QFPROM_CORR_PTE1_IDDQ_CX_ON_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_SERIAL_NUM_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004008)
#define HWIO_QFPROM_CORR_SERIAL_NUM_OFFS                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004008)
#define HWIO_QFPROM_CORR_SERIAL_NUM_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE2_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000400c)
#define HWIO_QFPROM_CORR_PTE2_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000400c)
#define HWIO_QFPROM_CORR_PTE2_RMSK                                                             0xffffffff
#define HWIO_QFPROM_CORR_PTE2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_ADDR, HWIO_QFPROM_CORR_PTE2_RMSK)
#define HWIO_QFPROM_CORR_PTE2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_ADDR, m)
#define HWIO_QFPROM_CORR_PTE2_PTE_IDDQ_SPARE_BMSK                                              0xfc000000
#define HWIO_QFPROM_CORR_PTE2_PTE_IDDQ_SPARE_SHFT                                                    0x1a
#define HWIO_QFPROM_CORR_PTE2_IDDQ_MSS_OFF_BMSK                                                 0x3f80000
#define HWIO_QFPROM_CORR_PTE2_IDDQ_MSS_OFF_SHFT                                                      0x13
#define HWIO_QFPROM_CORR_PTE2_IDDQ_MSS_ON_BMSK                                                    0x7f800
#define HWIO_QFPROM_CORR_PTE2_IDDQ_MSS_ON_SHFT                                                        0xb
#define HWIO_QFPROM_CORR_PTE2_FOUNDRY_ID_BMSK                                                       0x700
#define HWIO_QFPROM_CORR_PTE2_FOUNDRY_ID_SHFT                                                         0x8
#define HWIO_QFPROM_CORR_PTE2_LOGIC_RETENTION_BMSK                                                   0xe0
#define HWIO_QFPROM_CORR_PTE2_LOGIC_RETENTION_SHFT                                                    0x5
#define HWIO_QFPROM_CORR_PTE2_SPEED_BIN_BMSK                                                         0x1c
#define HWIO_QFPROM_CORR_PTE2_SPEED_BIN_SHFT                                                          0x2
#define HWIO_QFPROM_CORR_PTE2_MX_RET_BIN_BMSK                                                         0x3
#define HWIO_QFPROM_CORR_PTE2_MX_RET_BIN_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004010)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004010)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE_BMSK                                             0xffc00000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE_SHFT                                                   0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE21_BMSK                                             0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE21_SHFT                                                 0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE20_BMSK                                             0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE20_SHFT                                                 0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_BMSK                                              0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_SHFT                                                 0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_BMSK                                              0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_SHFT                                                 0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE17_BMSK                                              0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE17_SHFT                                                 0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE16_BMSK                                              0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE16_SHFT                                                 0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_BMSK                                                0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_SHFT                                                   0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                        0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                           0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_BMSK                                            0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_SHFT                                               0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_BMSK                                                 0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_SHFT                                                    0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_BMSK                                                0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_SHFT                                                  0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_BMSK                                            0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_SHFT                                              0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                           0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                             0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                 0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                   0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                  0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                   0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                             0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                              0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                              0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                               0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                         0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                          0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                          0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                          0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                          0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                          0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                               0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                               0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_BMSK                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004014)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004014)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE_BMSK                                             0xffc00000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE_SHFT                                                   0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE21_BMSK                                             0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE21_SHFT                                                 0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE20_BMSK                                             0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE20_SHFT                                                 0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_BMSK                                              0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_SHFT                                                 0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_BMSK                                              0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_SHFT                                                 0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE17_BMSK                                              0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE17_SHFT                                                 0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE16_BMSK                                              0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE16_SHFT                                                 0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_BMSK                                                0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_SHFT                                                   0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                        0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                           0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_BMSK                                            0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_SHFT                                               0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_BMSK                                                 0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_SHFT                                                    0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_BMSK                                                0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_SHFT                                                  0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_BMSK                                            0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_SHFT                                              0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                           0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                             0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                 0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                   0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                  0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                   0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                             0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                              0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                              0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                               0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_BMSK                                         0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_SHFT                                          0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_BMSK                                          0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_SHFT                                          0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_BMSK                                          0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_SHFT                                          0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                               0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                               0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_BMSK                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004018)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004018)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_PIL_SUBSYSTEM0_BMSK                               0xfc000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_PIL_SUBSYSTEM0_SHFT                                     0x1a
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_TZ_BMSK                                            0x3fff000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_TZ_SHFT                                                  0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_SBL1_BMSK                                              0xffe
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_SBL1_SHFT                                                0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_BMSK                                0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_SHFT                                0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000401c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000401c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_APPSBL0_BMSK                                      0xfffc0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_APPSBL0_SHFT                                            0x12
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_BMSK                                  0x3ffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_SHFT                                      0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004020)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004020)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_APPSBL1_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_APPSBL1_SHFT                                             0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004024)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004024)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_BMSK                      0xff000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_SHFT                            0x18
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_HYPERVISOR_BMSK                                     0xfff000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_HYPERVISOR_SHFT                                          0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_BMSK                                               0xff0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_SHFT                                                 0x4
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_APPSBL2_BMSK                                             0xf
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_APPSBL2_SHFT                                             0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004028)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004028)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_BMSK                                          0xffff0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_SHFT                                                0x10
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_BMSK                                              0xffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000402c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000402c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SPARE0_BMSK                                       0xffffff00
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SPARE0_SHFT                                              0x8
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                      0xff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                       0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004030)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004030)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_BMSK                                       0xff000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_SHFT                                             0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_BMSK                             0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG21_SECURE_SHFT                                 0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                                0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                    0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                            0x3c0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                                0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_BMSK                              0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG20_SECURE_SHFT                                 0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_BMSK                              0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG19_SECURE_SHFT                                 0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_BMSK                               0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG18_SECURE_SHFT                                  0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_BMSK                               0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG17_SECURE_SHFT                                  0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_BMSK                               0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_REG16_SECURE_SHFT                                  0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_BMSK                                          0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_SHFT                                             0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_BMSK                                  0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPDM_SECURE_MODE_SHFT                                    0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALT_SD_PORT_FOR_BOOT_BMSK                              0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALT_SD_PORT_FOR_BOOT_SHFT                                0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                     0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                       0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_BMSK                               0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_SHFT                                 0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                          0xe0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                           0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                 0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_BMSK                                 0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_SHFT                                 0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_BMSK                                               0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE_SHFT                                               0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_BMSK                                        0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_SHFT                                        0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                     0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                     0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004034)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004034)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                       0xff800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                             0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                       0x780000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                           0x13
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                            0x40000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                               0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_BMSK                             0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_SHFT                                0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_BMSK                              0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_SHFT                                 0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                                0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                   0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                                0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                   0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_BMSK                             0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_SHFT                                0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_BMSK                              0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_SHFT                                 0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                                0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                  0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                                0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                  0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_DISABLE_BMSK                                    0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_DISABLE_SHFT                                      0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE0_DISABLE_BMSK                                    0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE0_DISABLE_SHFT                                      0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_BMSK                              0x80
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_VENUS_0_DBGEN_DISABLE_SHFT                               0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_BMSK                                  0x40
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DAPEN_DISABLE_SHFT                                   0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_WCSS_NIDEN_DISABLE_BMSK                             0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_WCSS_NIDEN_DISABLE_SHFT                              0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_BMSK                                  0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RPM_DBGEN_DISABLE_SHFT                                   0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_BMSK                                  0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WCSS_DBGEN_DISABLE_SHFT                                  0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_BMSK                                   0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_NIDEN_DISABLE_SHFT                                   0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_BMSK                                   0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_MSS_DBGEN_DISABLE_SHFT                                   0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ALL_DEBUG_DISABLE_BMSK                                   0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ALL_DEBUG_DISABLE_SHFT                                   0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004038)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004038)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_BMSK                               0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_PRODUCT_ID_SHFT                                     0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_BMSK                                        0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_SHFT                                           0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000403c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OFFS                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000403c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_VID_BMSK                                   0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_VID_SHFT                                         0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_PID_BMSK                                       0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PERIPH_PID_SHFT                                          0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004040)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004040)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_VU_EN_BMSK                            0xfc000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_VU_EN_SHFT                                  0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_NAV_EN_BMSK                                  0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_NAV_EN_SHFT                                       0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_LDO_EN_BMSK                                  0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_LDO_EN_SHFT                                       0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_ECS_EN_BMSK                                   0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_ECS_EN_SHFT                                       0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_WCDMA_EN_BMSK                           0x600000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_WCDMA_EN_SHFT                               0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_TDSCDMA_EN_BMSK                         0x180000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_TDSCDMA_EN_SHFT                             0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_EN_BMSK                              0x60000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_EN_SHFT                                 0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT2_EN_BMSK                   0x18000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT2_EN_SHFT                       0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT1_EN_BMSK                    0x6000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_LTE_ABOVE_CAT1_EN_SHFT                       0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_MIMO_EN_BMSK                         0x1800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_MIMO_EN_SHFT                            0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_EN_BMSK                               0x600
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_EN_SHFT                                 0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_DC_EN_BMSK                            0x180
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_HSPA_DC_EN_SHFT                              0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_GENRAN_EN_BMSK                              0x60
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_GENRAN_EN_SHFT                               0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_EN_BMSK                                     0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_EN_SHFT                                      0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DO_EN_BMSK                                   0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_DO_EN_SHFT                                   0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_1X_EN_BMSK                                   0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MSMC_MODEM_1X_EN_SHFT                                   0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004044)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004044)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                 0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                       0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE_BMSK                                       0x7ff00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE_SHFT                                             0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_MDSP_FW_EN_BMSK                                0xfff00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_MDSP_FW_EN_SHFT                                    0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_SPARE_BMSK                                        0xc0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_SPARE_SHFT                                         0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_VU_EN_BMSK                                  0x3f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MSMC_MODEM_VU_EN_SHFT                                   0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004048)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004048)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ACC_DISABLE_BMSK                                 0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ACC_DISABLE_SHFT                                       0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SHADOW_READ_DISABLE_BMSK                         0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SHADOW_READ_DISABLE_SHFT                               0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                      0x30000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                            0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_SMT_PERM_ENABLE_BMSK                         0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_SMT_PERM_ENABLE_SHFT                              0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_RCP_BYPASS_ENABLE_BMSK                       0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_RCP_BYPASS_ENABLE_SHFT                            0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_SMMU_BYPASS_DISABLE_BMSK                     0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_SMMU_BYPASS_DISABLE_SHFT                          0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_GPIO_SLIMBUS_PD_DISABLE_BMSK                 0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_GPIO_SLIMBUS_PD_DISABLE_SHFT                      0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_PCIE20_RC_DISABLE_BMSK                        0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_FUSE_PCIE20_RC_DISABLE_SHFT                            0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_PCIE_DISABLE_BMSK                             0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_MSMC_PCIE_DISABLE_SHFT                                 0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_NDINT_DISABLE_BMSK                                 0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_NDINT_DISABLE_SHFT                                     0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                       0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                           0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_BMSK                         0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_SHFT                            0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_BMSK                          0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_SHFT                             0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_BMSK                           0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_SHFT                              0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_BMSK                           0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_SHFT                              0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_BMSK                         0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_SHFT                            0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_BMSK                          0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_SHFT                             0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                           0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                              0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                           0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                              0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_BMSK                                0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_SHFT                                  0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_BMSK                                0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_SHFT                                  0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                         0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                           0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_BMSK                             0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_SHFT                               0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_WCSS_NIDEN_DISABLE_BMSK                         0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_WCSS_NIDEN_DISABLE_SHFT                          0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_BMSK                              0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_SHFT                               0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                             0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                              0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_BMSK                              0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_SHFT                               0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_BMSK                               0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_SHFT                               0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QDI_SPMI_DISABLE_BMSK                                   0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QDI_SPMI_DISABLE_SHFT                                   0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                    0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                        0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                        0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000404c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000404c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                      0xf8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                            0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                     0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                          0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                            0x3ffc000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                  0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SPARE1_BMSK                                          0x3c00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SPARE1_SHFT                                             0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_BMSK                          0x3e0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_MODEM_PBL_PATCH_VERSION_SHFT                            0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                            0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                             0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004050)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004050)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK            0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                   0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004054)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004054)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE3_BMSK                                      0xf0000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE3_SHFT                                            0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APSS_BOOT_TRIGGER_DISABLE_BMSK                    0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APSS_BOOT_TRIGGER_DISABLE_SHFT                         0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_BMSK                           0x7800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_PBL_PLL_CTRL_SHFT                                0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                             0x780000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                 0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_CFG_BMSK                              0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_CFG_SHFT                                 0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_DELAY_BMSK                            0x30000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FSM_DELAY_SHFT                               0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_BMSK              0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_SHFT                 0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_USB_SS_ENABLE_BMSK                                   0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_USB_SS_ENABLE_SHFT                                      0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_BMSK                             0x3000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_SHFT                                0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_BMSK                               0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_SHFT                                 0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MSA_ENA_BMSK                                          0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MSA_ENA_SHFT                                            0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_FORCE_MSA_AUTH_EN_BMSK                                0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_FORCE_MSA_AUTH_EN_SHFT                                  0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE_8_7_BMSK                                        0x180
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE_8_7_SHFT                                          0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_BMSK                               0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_MODEM_BOOT_FROM_ROM_SHFT                                0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_BMSK                            0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_SHFT                             0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                 0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                  0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004058 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OFFS(n)                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004058 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                           3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                        0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                               0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000405c + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OFFS(n)                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000405c + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                           3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                        0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                               0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004078 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OFFS(n)                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004078 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                           3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                        0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                               0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000407c + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OFFS(n)                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000407c + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                           3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                        0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                               0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004098)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_OFFS                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004098)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT4_BMSK                                  0xff000000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT4_SHFT                                        0x18
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT3_BMSK                                    0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT3_SHFT                                        0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT2_BMSK                                      0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT2_SHFT                                         0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT1_BMSK                                        0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_SEC_BOOT1_SHFT                                         0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000409c)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_OFFS                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000409c)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_RMSK                                              0xffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT7_BMSK                                    0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT7_SHFT                                        0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT6_BMSK                                      0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT6_SHFT                                         0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT5_BMSK                                        0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_SEC_BOOT5_SHFT                                         0x0

#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a0)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040a0)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT4_BMSK                                   0xff000000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT4_SHFT                                         0x18
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT3_BMSK                                     0xff0000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT3_SHFT                                         0x10
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT2_BMSK                                       0xff00
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT2_SHFT                                          0x8
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT1_BMSK                                         0xff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_SEC_BOOT1_SHFT                                          0x0

#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a4)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040a4)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_RMSK                                               0xffffff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT7_BMSK                                     0xff0000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT7_SHFT                                         0x10
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT6_BMSK                                       0xff00
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT6_SHFT                                          0x8
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT5_BMSK                                         0xff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_MSB_SEC_BOOT5_SHFT                                          0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_OFFS(n)                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040a8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_MAXn                                                          3
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_HASH_DATA0_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_HASH_DATA0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040ac + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_OFFS(n)                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040ac + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_MAXn                                                          3
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                        0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                             0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040c8)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_OFFS                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040c8)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_HASH_DATA0_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_HASH_DATA0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040cc)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_OFFS                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040cc)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_QFPROM_CORR_PK_HASH_ROW4_MSB_BMSK                      0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_QFPROM_CORR_PK_HASH_ROW4_MSB_SHFT                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040d0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS2_POINT1_BMSK                                     0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS2_POINT1_SHFT                                           0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS1_POINT1_BMSK                                      0xf000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS1_POINT1_SHFT                                           0x18
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS0_POINT1_BMSK                                       0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS0_POINT1_SHFT                                           0x14
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_BASE1_BMSK                                          0xffc00
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_BASE1_SHFT                                              0xa
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_BASE0_BMSK                                            0x3ff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_TSENS_BASE0_SHFT                                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040d4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_FEC_BMSK                                               0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_USB_PHY_TUNING_BMSK                                      0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_USB_PHY_TUNING_SHFT                                          0x14
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_SPARE_BMSK                                                0xff800
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_SPARE_SHFT                                                    0xb
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS_CAL_SEL_BMSK                                          0x700
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS_CAL_SEL_SHFT                                            0x8
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS4_POINT1_BMSK                                           0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS4_POINT1_SHFT                                            0x4
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS3_POINT1_BMSK                                            0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_TSENS3_POINT1_SHFT                                            0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d8)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040d8)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_0_BMSK                   0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_0_SHFT                         0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_SVS2_BMSK                               0x7c000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_SVS2_SHFT                                     0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_1_BMSK                    0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_1_SHFT                         0x19
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_SVS_BMSK                                 0x1f00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_SVS_SHFT                                      0x14
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_2_BMSK                      0x80000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_2_SHFT                         0x13
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_NOM_BMSK                                   0x7c000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_NOM_SHFT                                       0xe
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_3_BMSK                       0x2000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_OFFSET_TUR_BIT_3_SHFT                          0xd
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_TUR_BMSK                                    0x1f00
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR0_TARG_VOLT_TUR_SHFT                                       0x8
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR1_TARG_VOLT_OFFSET_SVS2_BMSK                              0x80
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR1_TARG_VOLT_OFFSET_SVS2_SHFT                               0x7
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_TARG_VOLT_OFFSET_NOM_BMSK                               0x78
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_TARG_VOLT_OFFSET_NOM_SHFT                                0x3
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_TARG_VOLT_OFFSET_TUR_BMSK                                0x7
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_CPR2_TARG_VOLT_OFFSET_TUR_SHFT                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040dc)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040dc)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_FEC_BMSK                                               0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_0_BMSK                     0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_0_SHFT                         0x17
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_TARG_VOLT_SVS2_BMSK                                 0x7c0000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_TARG_VOLT_SVS2_SHFT                                     0x12
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_1_BMSK                      0x20000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_1_SHFT                         0x11
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_TARG_VOLT_SVS_BMSK                                   0x1f000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_TARG_VOLT_SVS_SHFT                                       0xc
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_2_BMSK                        0x800
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_2_SHFT                          0xb
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_TARG_VOLT_NOM_BMSK                                     0x7c0
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_TARG_VOLT_NOM_SHFT                                       0x6
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_3_BMSK                         0x20
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR0_TARG_VOLT_OFFSET_NOM_BIT_3_SHFT                          0x5
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_TARG_VOLT_TUR_BMSK                                      0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CPR1_TARG_VOLT_TUR_SHFT                                       0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e0)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040e0)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_TARGET1_BMSK                                       0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_TARGET1_SHFT                                             0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_TARGET0_BMSK                                        0xfc00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_TARGET0_SHFT                                             0x16
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_ENABLE_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_LDO_ENABLE_SHFT                                              0x15
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS2_BMSK                          0x1e0000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS2_SHFT                              0x11
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS_BMSK                            0x1c000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS_SHFT                                0xe
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_TARG_VOLT_NOM_BMSK                                    0x3e00
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_TARG_VOLT_NOM_SHFT                                       0x9
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS_BIT_3_BMSK                        0x100
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR0_TARG_VOLT_OFFSET_SVS_BIT_3_SHFT                          0x8
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_TARG_VOLT_TUR_BMSK                                      0xf8
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR2_TARG_VOLT_TUR_SHFT                                       0x3
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR_REV_BMSK                                                  0x7
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_CPR_REV_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e4)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040e4)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_FEC_BMSK                                               0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_SVS2_BMSK                          0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_SVS2_SHFT                              0x16
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_SVS_BMSK                           0x3c0000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_SVS_SHFT                               0x12
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_NOM_BMSK                            0x3c000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_NOM_SHFT                                0xe
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_TUR_BMSK                             0x3c00
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_CPR1_TARG_VOLT_OFFSET_TUR_SHFT                                0xa
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_SPARE_BMSK                                                  0x300
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_SPARE_SHFT                                                    0x8
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_GNSS_ADC_CH0_VCM_BMSK                                        0xc0
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_GNSS_ADC_CH0_VCM_SHFT                                         0x6
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_GNSS_ADC_CH0_LDO_BMSK                                        0x30
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_GNSS_ADC_CH0_LDO_SHFT                                         0x4
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_GNSS_ADC_CH0_VREF_BMSK                                        0xc
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_GNSS_ADC_CH0_VREF_SHFT                                        0x2
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_LDO_TARGET1_BMSK                                              0x3
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_LDO_TARGET1_SHFT                                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e8)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040e8)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_BHS_SLOPE_BMSK                                     0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_BHS_SLOPE_SHFT                                           0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_BHS_INTERCEPT_BMSK                                 0x7fe00000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_BHS_INTERCEPT_SHFT                                       0x15
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_LDO_SLOPE_BMSK                                       0x1ff800
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_LDO_SLOPE_SHFT                                            0xb
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_LDO_INTERCEPT_BMSK                                      0x7fe
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_LDO_INTERCEPT_SHFT                                        0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_ENABLE_BMSK                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ECS_ENABLE_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040ec)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040ec)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_FEC_BMSK                                               0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_Q6SS0_LDO_VREF_TRIM_BMSK                                 0xf80000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_Q6SS0_LDO_VREF_TRIM_SHFT                                     0x13
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_TUR_BIT_3_BMSK                      0x40000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR2_TARG_VOLT_OFFSET_TUR_BIT_3_SHFT                         0x12
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR1_TARG_VOLT_OFFSET_SVS2_BIT_1_BMSK                     0x20000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_CPR1_TARG_VOLT_OFFSET_SVS2_BIT_1_SHFT                        0x11
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ECS_MISC_BMSK                                             0x1fe00
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ECS_MISC_SHFT                                                 0x9
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ECS_BHS_SLOPE_BMSK                                          0x1ff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ECS_BHS_SLOPE_SHFT                                            0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f0)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040f0)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M2_BMSK                                           0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M2_SHFT                                                 0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M1_BMSK                                           0x38000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M1_SHFT                                                 0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M0_BMSK                                            0x7000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B1M0_SHFT                                                 0x18
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_G_B1_BMSK                                                0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_G_B1_SHFT                                                    0x15
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_SAR_LDO_ERR0_BMSK                                        0x180000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_SAR_LDO_ERR0_SHFT                                            0x13
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CLK_LDO_ERR0_BMSK                                         0x60000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_CLK_LDO_ERR0_SHFT                                            0x11
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_VREF_ERR_B0_BMSK                                          0x18000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_VREF_ERR_B0_SHFT                                              0xf
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M3_BMSK                                               0x7000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M3_SHFT                                                  0xc
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M2_BMSK                                                0xe00
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M2_SHFT                                                  0x9
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M1_BMSK                                                0x1c0
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M1_SHFT                                                  0x6
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M0_BMSK                                                 0x38
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_PH_B0M0_SHFT                                                  0x3
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_G_B0_BMSK                                                     0x7
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_G_B0_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f4)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040f4)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_FEC_BMSK                                               0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M3_BMSK                                             0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M3_SHFT                                                 0x16
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M2_BMSK                                             0x380000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M2_SHFT                                                 0x13
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M1_BMSK                                              0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M1_SHFT                                                 0x10
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M0_BMSK                                               0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B2M0_SHFT                                                  0xd
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_G_B2_BMSK                                                  0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_G_B2_SHFT                                                     0xa
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_SAR_LDO_ERR1_BMSK                                           0x300
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_SAR_LDO_ERR1_SHFT                                             0x8
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CLK_LDO_ERR1_BMSK                                            0xc0
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_CLK_LDO_ERR1_SHFT                                             0x6
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_VREF_ERR_B1_BMSK                                             0x30
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_VREF_ERR_B1_SHFT                                              0x4
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B1M3_BMSK                                                  0xe
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B1M3_SHFT                                                  0x1
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B1M2_BMSK                                                  0x1
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_PH_B1M2_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f8)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040f8)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B4M0_BMSK                                           0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B4M0_SHFT                                                 0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_G_B4_BMSK                                              0x70000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_G_B4_SHFT                                                    0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_SAR_LDO_ERR3_BMSK                                       0xc000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_SAR_LDO_ERR3_SHFT                                            0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CLK_LDO_ERR3_BMSK                                       0x3000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CLK_LDO_ERR3_SHFT                                            0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_VREF_ERR_B3_BMSK                                         0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_VREF_ERR_B3_SHFT                                             0x16
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M3_BMSK                                             0x380000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M3_SHFT                                                 0x13
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M2_BMSK                                              0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M2_SHFT                                                 0x10
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M1_BMSK                                               0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M1_SHFT                                                  0xd
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M0_BMSK                                               0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B3M0_SHFT                                                  0xa
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_G_B3_BMSK                                                   0x380
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_G_B3_SHFT                                                     0x7
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_SAR_LDO_ERR2_BMSK                                            0x60
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_SAR_LDO_ERR2_SHFT                                             0x5
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CLK_LDO_ERR2_BMSK                                            0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CLK_LDO_ERR2_SHFT                                             0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_VREF_ERR_B2_BMSK                                              0x6
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_VREF_ERR_B2_SHFT                                              0x1
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B2M3_BMSK                                                  0x1
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_PH_B2M3_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040fc)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040fc)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_FEC_BMSK                                               0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B5M1_BMSK                                             0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B5M1_SHFT                                                 0x17
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B5M0_BMSK                                             0x700000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B5M0_SHFT                                                 0x14
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_G_B5_BMSK                                                 0xe0000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_G_B5_SHFT                                                    0x11
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_SAR_LDO_ERR4_BMSK                                         0x18000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_SAR_LDO_ERR4_SHFT                                             0xf
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CLK_LDO_ERR4_BMSK                                          0x6000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CLK_LDO_ERR4_SHFT                                             0xd
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_VREF_ERR_B4_BMSK                                           0x1800
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_VREF_ERR_B4_SHFT                                              0xb
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M3_BMSK                                                0x700
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M3_SHFT                                                  0x8
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M2_BMSK                                                 0xe0
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M2_SHFT                                                  0x5
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M1_BMSK                                                 0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M1_SHFT                                                  0x2
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M0_BMSK                                                  0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_PH_B4M0_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004100)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004100)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CLK_LDO_ERR6_BMSK                                      0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CLK_LDO_ERR6_SHFT                                            0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_ERR_B6_BMSK                                       0x60000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_ERR_B6_SHFT                                             0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M3_BMSK                                           0x1c000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M3_SHFT                                                 0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M2_BMSK                                            0x3800000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M2_SHFT                                                 0x17
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M1_BMSK                                             0x700000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M1_SHFT                                                 0x14
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M0_BMSK                                              0xe0000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B6M0_SHFT                                                 0x11
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_G_B6_BMSK                                                 0x1c000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_G_B6_SHFT                                                     0xe
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_SAR_LDO_ERR5_BMSK                                          0x3000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_SAR_LDO_ERR5_SHFT                                             0xc
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CLK_LDO_ERR5_BMSK                                           0xc00
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CLK_LDO_ERR5_SHFT                                             0xa
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_ERR_B5_BMSK                                            0x300
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_VREF_ERR_B5_SHFT                                              0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M3_BMSK                                                 0xe0
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M3_SHFT                                                  0x5
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M2_BMSK                                                 0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M2_SHFT                                                  0x2
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M1_BMSK                                                  0x3
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_PH_B5M1_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004104)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_OFFS                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004104)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_FEC_BMSK                                               0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_FEC_SHFT                                                     0x18
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_BMSK                                  0xff0000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_SHFT                                      0x10
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_BMSK                                    0xff00
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_SHFT                                       0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_AVG_ERROR_BMSK                            0x80
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_AVG_ERROR_SHFT                             0x7
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_AVG_ERROR_BMSK                            0x40
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_AVG_ERROR_SHFT                             0x6
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC_0_1_FUSEFLAG_BMSK                                0x20
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC_0_1_FUSEFLAG_SHFT                                 0x5
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_OVERFLOW_BMSK                             0x10
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC1_CALIB_OVERFLOW_SHFT                              0x4
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_OVERFLOW_BMSK                              0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_MODEM_TXDAC0_CALIB_OVERFLOW_SHFT                              0x3
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_SAR_LDO_ERR6_BMSK                                             0x6
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_SAR_LDO_ERR6_SHFT                                             0x1
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CLK_LDO_ERR6_BMSK                                             0x1
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_CLK_LDO_ERR6_SHFT                                             0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004108 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_OFFS(n)                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004108 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_MAXn                                                      15
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000410c + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_OFFS(n)                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000410c + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK                                                0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_MAXn                                                      15
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004188)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004188)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_MEM_ACCEL_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000418c)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000418c)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_RMSK                                               0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_MEM_ACCEL_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004190)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004190)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004194)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004194)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_RMSK                                               0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_MEM_ACCEL_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004198)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004198)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000419c)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000419c)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_RMSK                                               0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_MEM_ACCEL_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a0)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041a0)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_MEM_ACCEL_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_LSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a4)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041a4)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_RMSK                                               0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_MEM_ACCEL_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW19_MSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a8)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041a8)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_MEM_ACCEL_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_LSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ac)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_OFFS                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041ac)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_RMSK                                               0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_MEM_ACCEL_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW20_MSB_MEM_ACCEL_SHFT                                          0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_OFFS(n)                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_MAXn                                                       31
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_BMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_SHFT                                           0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_OFFS(n)                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK                                                 0xffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_MAXn                                                       31
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_SPARE3_BMSK                                          0xfe0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_SPARE3_SHFT                                              0x11
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_ADDR_BMSK                                       0x1fffe
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_ADDR_SHFT                                           0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                             0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                             0x0

#define HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042b0)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_OFFS                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042b0)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR,m,v,HWIO_QFPROM_CORR_FEC_EN_LSB_IN)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION31_FEC_EN_BMSK                                       0x80000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION31_FEC_EN_SHFT                                             0x1f
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_BMSK                                       0x40000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_SHFT                                             0x1e
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_BMSK                                       0x20000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_SHFT                                             0x1d
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_BMSK                                       0x10000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_SHFT                                             0x1c
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                        0x8000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                             0x1b
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                        0x4000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                             0x1a
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                        0x2000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                             0x19
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                        0x1000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                             0x18
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                         0x800000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                             0x17
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                         0x400000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                             0x16
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                         0x200000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                             0x15
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                         0x100000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                             0x14
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                          0x80000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                             0x13
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                          0x40000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                             0x12
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                          0x20000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                             0x11
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                          0x10000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                             0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                           0x8000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                              0xf
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                           0x4000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                              0xe
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                           0x2000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                              0xd
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                           0x1000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                              0xc
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                            0x800
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                              0xb
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                            0x400
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                              0xa
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                             0x200
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                               0x9
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                             0x100
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                               0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                              0x80
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                               0x7
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                              0x40
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                               0x6
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                              0x20
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                               0x5
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                              0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                               0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                               0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                               0x3
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                               0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                               0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                               0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                               0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                               0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                               0x0

#define HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042b4)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_OFFS                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042b4)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_FEC_EN_REDUNDANCY_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_FEC_EN_REDUNDANCY_SHFT                                            0x0

#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042b8)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042b8)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG16_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042bc)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042bc)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG16_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG16_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_SPARE0_BMSK                                           0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_SPARE0_SHFT                                                 0x1f
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG16_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042c0)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042c0)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG17_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042c4)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042c4)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG17_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG17_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG17_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042c8)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042c8)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG18_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042cc)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042cc)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG18_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042d0)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042d0)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042d4)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042d4)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG19_MSB_IN)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042d8)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042d8)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG20_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG20_LSB_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG20_LSB_IN)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042dc)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042dc)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG20_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG20_MSB_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG20_MSB_IN)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042e0)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042e0)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG21_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG21_LSB_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG21_LSB_IN)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_SPARE0_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_SPARE0_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042e4)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_OFFS                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000042e4)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG21_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG21_MSB_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG21_MSB_IN)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_SPARE1_BMSK                                             0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_SPARE1_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n)                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004400 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_OFFS(n)                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004400 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_MAXn                                                             39
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n), HWIO_QFPROM_CORR_ACC_PRIVATEn_RMSK)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ACC_PRIVATE_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ACC_PRIVATE_SHFT                                                0x0

#define HWIO_SEC_CTRL_HW_VERSION_ADDR                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_OFFS                                                          (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_RMSK                                                          0xffffffff
#define HWIO_SEC_CTRL_HW_VERSION_IN          \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_ADDR, HWIO_SEC_CTRL_HW_VERSION_RMSK)
#define HWIO_SEC_CTRL_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_ADDR, m)
#define HWIO_SEC_CTRL_HW_VERSION_MAJOR_BMSK                                                    0xf0000000
#define HWIO_SEC_CTRL_HW_VERSION_MAJOR_SHFT                                                          0x1c
#define HWIO_SEC_CTRL_HW_VERSION_MINOR_BMSK                                                     0xfff0000
#define HWIO_SEC_CTRL_HW_VERSION_MINOR_SHFT                                                          0x10
#define HWIO_SEC_CTRL_HW_VERSION_STEP_BMSK                                                         0xffff
#define HWIO_SEC_CTRL_HW_VERSION_STEP_SHFT                                                            0x0

#define HWIO_FEATURE_CONFIG0_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006004)
#define HWIO_FEATURE_CONFIG0_OFFS                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006004)
#define HWIO_FEATURE_CONFIG0_RMSK                                                              0xffffffff
#define HWIO_FEATURE_CONFIG0_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, HWIO_FEATURE_CONFIG0_RMSK)
#define HWIO_FEATURE_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, m)
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_VU_EN_BMSK                                             0xfc000000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_VU_EN_SHFT                                                   0x1a
#define HWIO_FEATURE_CONFIG0_MSMC_NAV_EN_BMSK                                                   0x2000000
#define HWIO_FEATURE_CONFIG0_MSMC_NAV_EN_SHFT                                                        0x19
#define HWIO_FEATURE_CONFIG0_MSMC_LDO_EN_BMSK                                                   0x1000000
#define HWIO_FEATURE_CONFIG0_MSMC_LDO_EN_SHFT                                                        0x18
#define HWIO_FEATURE_CONFIG0_MSMC_ECS_EN_BMSK                                                    0x800000
#define HWIO_FEATURE_CONFIG0_MSMC_ECS_EN_SHFT                                                        0x17
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_WCDMA_EN_BMSK                                            0x600000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_WCDMA_EN_SHFT                                                0x15
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_TDSCDMA_EN_BMSK                                          0x180000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_TDSCDMA_EN_SHFT                                              0x13
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_EN_BMSK                                               0x60000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_EN_SHFT                                                  0x11
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_ABOVE_CAT2_EN_BMSK                                    0x18000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_ABOVE_CAT2_EN_SHFT                                        0xf
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_ABOVE_CAT1_EN_BMSK                                     0x6000
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_LTE_ABOVE_CAT1_EN_SHFT                                        0xd
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_MIMO_EN_BMSK                                          0x1800
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_MIMO_EN_SHFT                                             0xb
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_EN_BMSK                                                0x600
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_EN_SHFT                                                  0x9
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_DC_EN_BMSK                                             0x180
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_HSPA_DC_EN_SHFT                                               0x7
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_GENRAN_EN_BMSK                                               0x60
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_GENRAN_EN_SHFT                                                0x5
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_EN_BMSK                                                      0x10
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_EN_SHFT                                                       0x4
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_DO_EN_BMSK                                                    0xc
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_DO_EN_SHFT                                                    0x2
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_1X_EN_BMSK                                                    0x3
#define HWIO_FEATURE_CONFIG0_MSMC_MODEM_1X_EN_SHFT                                                    0x0

#define HWIO_FEATURE_CONFIG1_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006008)
#define HWIO_FEATURE_CONFIG1_OFFS                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006008)
#define HWIO_FEATURE_CONFIG1_RMSK                                                              0xffffffff
#define HWIO_FEATURE_CONFIG1_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, HWIO_FEATURE_CONFIG1_RMSK)
#define HWIO_FEATURE_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, m)
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                  0x80000000
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                        0x1f
#define HWIO_FEATURE_CONFIG1_SPARE_BMSK                                                        0x7ff00000
#define HWIO_FEATURE_CONFIG1_SPARE_SHFT                                                              0x14
#define HWIO_FEATURE_CONFIG1_MSMC_MDSP_FW_EN_BMSK                                                 0xfff00
#define HWIO_FEATURE_CONFIG1_MSMC_MDSP_FW_EN_SHFT                                                     0x8
#define HWIO_FEATURE_CONFIG1_MSMC_SPARE_BMSK                                                         0xc0
#define HWIO_FEATURE_CONFIG1_MSMC_SPARE_SHFT                                                          0x6
#define HWIO_FEATURE_CONFIG1_MSMC_MODEM_VU_EN_BMSK                                                   0x3f
#define HWIO_FEATURE_CONFIG1_MSMC_MODEM_VU_EN_SHFT                                                    0x0

#define HWIO_FEATURE_CONFIG2_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_OFFS                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_RMSK                                                              0xffffffff
#define HWIO_FEATURE_CONFIG2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, HWIO_FEATURE_CONFIG2_RMSK)
#define HWIO_FEATURE_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, m)
#define HWIO_FEATURE_CONFIG2_ACC_DISABLE_BMSK                                                  0x80000000
#define HWIO_FEATURE_CONFIG2_ACC_DISABLE_SHFT                                                        0x1f
#define HWIO_FEATURE_CONFIG2_SHADOW_READ_DISABLE_BMSK                                          0x40000000
#define HWIO_FEATURE_CONFIG2_SHADOW_READ_DISABLE_SHFT                                                0x1e
#define HWIO_FEATURE_CONFIG2_SPARE1_BMSK                                                       0x30000000
#define HWIO_FEATURE_CONFIG2_SPARE1_SHFT                                                             0x1c
#define HWIO_FEATURE_CONFIG2_FUSE_SMT_PERM_ENABLE_BMSK                                          0x8000000
#define HWIO_FEATURE_CONFIG2_FUSE_SMT_PERM_ENABLE_SHFT                                               0x1b
#define HWIO_FEATURE_CONFIG2_FUSE_RCP_BYPASS_ENABLE_BMSK                                        0x4000000
#define HWIO_FEATURE_CONFIG2_FUSE_RCP_BYPASS_ENABLE_SHFT                                             0x1a
#define HWIO_FEATURE_CONFIG2_MSMC_SMMU_BYPASS_DISABLE_BMSK                                      0x2000000
#define HWIO_FEATURE_CONFIG2_MSMC_SMMU_BYPASS_DISABLE_SHFT                                           0x19
#define HWIO_FEATURE_CONFIG2_MSMC_GPIO_SLIMBUS_PD_DISABLE_BMSK                                  0x1000000
#define HWIO_FEATURE_CONFIG2_MSMC_GPIO_SLIMBUS_PD_DISABLE_SHFT                                       0x18
#define HWIO_FEATURE_CONFIG2_FUSE_PCIE20_RC_DISABLE_BMSK                                         0x800000
#define HWIO_FEATURE_CONFIG2_FUSE_PCIE20_RC_DISABLE_SHFT                                             0x17
#define HWIO_FEATURE_CONFIG2_MSMC_PCIE_DISABLE_BMSK                                              0x400000
#define HWIO_FEATURE_CONFIG2_MSMC_PCIE_DISABLE_SHFT                                                  0x16
#define HWIO_FEATURE_CONFIG2_NDINT_DISABLE_BMSK                                                  0x200000
#define HWIO_FEATURE_CONFIG2_NDINT_DISABLE_SHFT                                                      0x15
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_BMSK                                        0x100000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_SHFT                                            0x14
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_BMSK                                          0x80000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_SHFT                                             0x13
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_BMSK                                           0x40000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_SHFT                                              0x12
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_BMSK                                            0x20000
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_SHFT                                               0x11
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_BMSK                                            0x10000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_SHFT                                               0x10
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_BMSK                                          0x8000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_SHFT                                             0xf
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_BMSK                                           0x4000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_SHFT                                              0xe
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_BMSK                                            0x2000
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_SHFT                                               0xd
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_BMSK                                            0x1000
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_SHFT                                               0xc
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_BMSK                                                 0x800
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_SHFT                                                   0xb
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_BMSK                                                 0x400
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_SHFT                                                   0xa
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_BMSK                                          0x200
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_SHFT                                            0x9
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_BMSK                                              0x100
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_SHFT                                                0x8
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_BMSK                                          0x80
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_SHFT                                           0x7
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_BMSK                                               0x40
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_SHFT                                                0x6
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_BMSK                                              0x20
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_SHFT                                               0x5
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_BMSK                                               0x10
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_SHFT                                                0x4
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_BMSK                                                0x8
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_SHFT                                                0x3
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_BMSK                                                    0x4
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_SHFT                                                    0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_BMSK                                                     0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_SHFT                                                     0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_BMSK                                                         0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_SHFT                                                         0x0

#define HWIO_FEATURE_CONFIG3_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006010)
#define HWIO_FEATURE_CONFIG3_OFFS                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006010)
#define HWIO_FEATURE_CONFIG3_RMSK                                                              0xffffffff
#define HWIO_FEATURE_CONFIG3_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, HWIO_FEATURE_CONFIG3_RMSK)
#define HWIO_FEATURE_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, m)
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_BMSK                                       0xf8000000
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_SHFT                                             0x1b
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_BMSK                                      0x4000000
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_SHFT                                           0x1a
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_BMSK                                             0x3ffc000
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_SHFT                                                   0xe
#define HWIO_FEATURE_CONFIG3_SPARE1_BMSK                                                           0x3c00
#define HWIO_FEATURE_CONFIG3_SPARE1_SHFT                                                              0xa
#define HWIO_FEATURE_CONFIG3_MODEM_PBL_PATCH_VERSION_BMSK                                           0x3e0
#define HWIO_FEATURE_CONFIG3_MODEM_PBL_PATCH_VERSION_SHFT                                             0x5
#define HWIO_FEATURE_CONFIG3_APPS_PBL_PATCH_VERSION_BMSK                                             0x1f
#define HWIO_FEATURE_CONFIG3_APPS_PBL_PATCH_VERSION_SHFT                                              0x0

#define HWIO_FEATURE_CONFIG4_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006014)
#define HWIO_FEATURE_CONFIG4_OFFS                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006014)
#define HWIO_FEATURE_CONFIG4_RMSK                                                              0xffffffff
#define HWIO_FEATURE_CONFIG4_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, HWIO_FEATURE_CONFIG4_RMSK)
#define HWIO_FEATURE_CONFIG4_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, m)
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK                             0xffffffff
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                                    0x0

#define HWIO_FEATURE_CONFIG5_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006018)
#define HWIO_FEATURE_CONFIG5_OFFS                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006018)
#define HWIO_FEATURE_CONFIG5_RMSK                                                              0xffffffff
#define HWIO_FEATURE_CONFIG5_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, HWIO_FEATURE_CONFIG5_RMSK)
#define HWIO_FEATURE_CONFIG5_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, m)
#define HWIO_FEATURE_CONFIG5_SPARE3_BMSK                                                       0xf0000000
#define HWIO_FEATURE_CONFIG5_SPARE3_SHFT                                                             0x1c
#define HWIO_FEATURE_CONFIG5_APSS_BOOT_TRIGGER_DISABLE_BMSK                                     0x8000000
#define HWIO_FEATURE_CONFIG5_APSS_BOOT_TRIGGER_DISABLE_SHFT                                          0x1b
#define HWIO_FEATURE_CONFIG5_MODEM_PBL_PLL_CTRL_BMSK                                            0x7800000
#define HWIO_FEATURE_CONFIG5_MODEM_PBL_PLL_CTRL_SHFT                                                 0x17
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_BMSK                                              0x780000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_SHFT                                                  0x13
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FSM_CFG_BMSK                                               0x40000
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FSM_CFG_SHFT                                                  0x12
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FSM_DELAY_BMSK                                             0x30000
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FSM_DELAY_SHFT                                                0x10
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_BMSK                               0x8000
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_SHFT                                  0xf
#define HWIO_FEATURE_CONFIG5_USB_SS_ENABLE_BMSK                                                    0x4000
#define HWIO_FEATURE_CONFIG5_USB_SS_ENABLE_SHFT                                                       0xe
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_BMSK                                              0x3000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_SHFT                                                 0xc
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FROM_ROM_BMSK                                                0x800
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FROM_ROM_SHFT                                                  0xb
#define HWIO_FEATURE_CONFIG5_MSA_ENA_BMSK                                                           0x400
#define HWIO_FEATURE_CONFIG5_MSA_ENA_SHFT                                                             0xa
#define HWIO_FEATURE_CONFIG5_FORCE_MSA_AUTH_EN_BMSK                                                 0x200
#define HWIO_FEATURE_CONFIG5_FORCE_MSA_AUTH_EN_SHFT                                                   0x9
#define HWIO_FEATURE_CONFIG5_SPARE_8_7_BMSK                                                         0x180
#define HWIO_FEATURE_CONFIG5_SPARE_8_7_SHFT                                                           0x7
#define HWIO_FEATURE_CONFIG5_MODEM_BOOT_FROM_ROM_BMSK                                                0x40
#define HWIO_FEATURE_CONFIG5_MODEM_BOOT_FROM_ROM_SHFT                                                 0x6
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_BMSK                                             0x20
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_SHFT                                              0x5
#define HWIO_FEATURE_CONFIG5_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                                  0x1f
#define HWIO_FEATURE_CONFIG5_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                                   0x0

#define HWIO_OEM_CONFIG0_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000601c)
#define HWIO_OEM_CONFIG0_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000601c)
#define HWIO_OEM_CONFIG0_RMSK                                                                  0xffffffff
#define HWIO_OEM_CONFIG0_IN          \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, HWIO_OEM_CONFIG0_RMSK)
#define HWIO_OEM_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, m)
#define HWIO_OEM_CONFIG0_SPARE0_BMSK                                                           0xff000000
#define HWIO_OEM_CONFIG0_SPARE0_SHFT                                                                 0x18
#define HWIO_OEM_CONFIG0_SPARE_REG21_SECURE_BMSK                                                 0x800000
#define HWIO_OEM_CONFIG0_SPARE_REG21_SECURE_SHFT                                                     0x17
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_BMSK                                                    0x400000
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_SHFT                                                        0x16
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_BMSK                                                0x3c0000
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_SHFT                                                    0x12
#define HWIO_OEM_CONFIG0_SPARE_REG20_SECURE_BMSK                                                  0x20000
#define HWIO_OEM_CONFIG0_SPARE_REG20_SECURE_SHFT                                                     0x11
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_BMSK                                                  0x10000
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_SHFT                                                     0x10
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_BMSK                                                   0x8000
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_SHFT                                                      0xf
#define HWIO_OEM_CONFIG0_SPARE_REG17_SECURE_BMSK                                                   0x4000
#define HWIO_OEM_CONFIG0_SPARE_REG17_SECURE_SHFT                                                      0xe
#define HWIO_OEM_CONFIG0_SPARE_REG16_SECURE_BMSK                                                   0x2000
#define HWIO_OEM_CONFIG0_SPARE_REG16_SECURE_SHFT                                                      0xd
#define HWIO_OEM_CONFIG0_WDOG_EN_BMSK                                                              0x1000
#define HWIO_OEM_CONFIG0_WDOG_EN_SHFT                                                                 0xc
#define HWIO_OEM_CONFIG0_SPDM_SECURE_MODE_BMSK                                                      0x800
#define HWIO_OEM_CONFIG0_SPDM_SECURE_MODE_SHFT                                                        0xb
#define HWIO_OEM_CONFIG0_ALT_SD_PORT_FOR_BOOT_BMSK                                                  0x400
#define HWIO_OEM_CONFIG0_ALT_SD_PORT_FOR_BOOT_SHFT                                                    0xa
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                         0x200
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                           0x9
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_BMSK                                                   0x100
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_SHFT                                                     0x8
#define HWIO_OEM_CONFIG0_FAST_BOOT_BMSK                                                              0xe0
#define HWIO_OEM_CONFIG0_FAST_BOOT_SHFT                                                               0x5
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_BMSK                                                    0x10
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_SHFT                                                     0x4
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_BMSK                                                     0x8
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_SHFT                                                     0x3
#define HWIO_OEM_CONFIG0_SPARE_BMSK                                                                   0x4
#define HWIO_OEM_CONFIG0_SPARE_SHFT                                                                   0x2
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_BMSK                                                            0x2
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_SHFT                                                            0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_BMSK                                                         0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_SHFT                                                         0x0

#define HWIO_OEM_CONFIG1_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006020)
#define HWIO_OEM_CONFIG1_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006020)
#define HWIO_OEM_CONFIG1_RMSK                                                                  0xffffffff
#define HWIO_OEM_CONFIG1_IN          \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, HWIO_OEM_CONFIG1_RMSK)
#define HWIO_OEM_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, m)
#define HWIO_OEM_CONFIG1_SPARE1_BMSK                                                           0xff800000
#define HWIO_OEM_CONFIG1_SPARE1_SHFT                                                                 0x17
#define HWIO_OEM_CONFIG1_ANTI_ROLLBACK_FEATURE_EN_BMSK                                           0x780000
#define HWIO_OEM_CONFIG1_ANTI_ROLLBACK_FEATURE_EN_SHFT                                               0x13
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_BMSK                                                0x40000
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_SHFT                                                   0x12
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_BMSK                                                 0x20000
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_SHFT                                                    0x11
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_BMSK                                                  0x10000
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_SHFT                                                     0x10
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_BMSK                                                    0x8000
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_SHFT                                                       0xf
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_BMSK                                                    0x4000
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_SHFT                                                       0xe
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_BMSK                                                 0x2000
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_SHFT                                                    0xd
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_BMSK                                                  0x1000
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_SHFT                                                     0xc
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_BMSK                                                    0x800
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_SHFT                                                      0xb
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_BMSK                                                    0x400
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_SHFT                                                      0xa
#define HWIO_OEM_CONFIG1_SPARE1_DISABLE_BMSK                                                        0x200
#define HWIO_OEM_CONFIG1_SPARE1_DISABLE_SHFT                                                          0x9
#define HWIO_OEM_CONFIG1_SPARE0_DISABLE_BMSK                                                        0x100
#define HWIO_OEM_CONFIG1_SPARE0_DISABLE_SHFT                                                          0x8
#define HWIO_OEM_CONFIG1_VENUS_0_DBGEN_DISABLE_BMSK                                                  0x80
#define HWIO_OEM_CONFIG1_VENUS_0_DBGEN_DISABLE_SHFT                                                   0x7
#define HWIO_OEM_CONFIG1_RPM_DAPEN_DISABLE_BMSK                                                      0x40
#define HWIO_OEM_CONFIG1_RPM_DAPEN_DISABLE_SHFT                                                       0x6
#define HWIO_OEM_CONFIG1_RPM_WCSS_NIDEN_DISABLE_BMSK                                                 0x20
#define HWIO_OEM_CONFIG1_RPM_WCSS_NIDEN_DISABLE_SHFT                                                  0x5
#define HWIO_OEM_CONFIG1_RPM_DBGEN_DISABLE_BMSK                                                      0x10
#define HWIO_OEM_CONFIG1_RPM_DBGEN_DISABLE_SHFT                                                       0x4
#define HWIO_OEM_CONFIG1_WCSS_DBGEN_DISABLE_BMSK                                                      0x8
#define HWIO_OEM_CONFIG1_WCSS_DBGEN_DISABLE_SHFT                                                      0x3
#define HWIO_OEM_CONFIG1_MSS_NIDEN_DISABLE_BMSK                                                       0x4
#define HWIO_OEM_CONFIG1_MSS_NIDEN_DISABLE_SHFT                                                       0x2
#define HWIO_OEM_CONFIG1_MSS_DBGEN_DISABLE_BMSK                                                       0x2
#define HWIO_OEM_CONFIG1_MSS_DBGEN_DISABLE_SHFT                                                       0x1
#define HWIO_OEM_CONFIG1_ALL_DEBUG_DISABLE_BMSK                                                       0x1
#define HWIO_OEM_CONFIG1_ALL_DEBUG_DISABLE_SHFT                                                       0x0

#define HWIO_OEM_CONFIG2_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006024)
#define HWIO_OEM_CONFIG2_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006024)
#define HWIO_OEM_CONFIG2_RMSK                                                                  0xffffffff
#define HWIO_OEM_CONFIG2_IN          \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, HWIO_OEM_CONFIG2_RMSK)
#define HWIO_OEM_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, m)
#define HWIO_OEM_CONFIG2_OEM_PRODUCT_ID_BMSK                                                   0xffff0000
#define HWIO_OEM_CONFIG2_OEM_PRODUCT_ID_SHFT                                                         0x10
#define HWIO_OEM_CONFIG2_OEM_HW_ID_BMSK                                                            0xffff
#define HWIO_OEM_CONFIG2_OEM_HW_ID_SHFT                                                               0x0

#define HWIO_OEM_CONFIG3_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006028)
#define HWIO_OEM_CONFIG3_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006028)
#define HWIO_OEM_CONFIG3_RMSK                                                                  0xffffffff
#define HWIO_OEM_CONFIG3_IN          \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, HWIO_OEM_CONFIG3_RMSK)
#define HWIO_OEM_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, m)
#define HWIO_OEM_CONFIG3_PERIPH_VID_BMSK                                                       0xffff0000
#define HWIO_OEM_CONFIG3_PERIPH_VID_SHFT                                                             0x10
#define HWIO_OEM_CONFIG3_PERIPH_PID_BMSK                                                           0xffff
#define HWIO_OEM_CONFIG3_PERIPH_PID_SHFT                                                              0x0

#define HWIO_BOOT_CONFIG_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000602c)
#define HWIO_BOOT_CONFIG_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000602c)
#define HWIO_BOOT_CONFIG_RMSK                                                                       0x3ff
#define HWIO_BOOT_CONFIG_IN          \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, HWIO_BOOT_CONFIG_RMSK)
#define HWIO_BOOT_CONFIG_INM(m)      \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, m)
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_BMSK                                                         0x200
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_SHFT                                                           0x9
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_BMSK                                                     0x100
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_SHFT                                                       0x8
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_BMSK                                                    0xc0
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_SHFT                                                     0x6
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_BMSK                                                     0x20
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_SHFT                                                      0x5
#define HWIO_BOOT_CONFIG_FAST_BOOT_BMSK                                                              0x1e
#define HWIO_BOOT_CONFIG_FAST_BOOT_SHFT                                                               0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_BMSK                                                                 0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_SHFT                                                                 0x0

#define HWIO_SECURE_BOOTn_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006030 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_OFFS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006030 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_RMSK                                                                      0x17f
#define HWIO_SECURE_BOOTn_MAXn                                                                          7
#define HWIO_SECURE_BOOTn_INI(n)        \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), HWIO_SECURE_BOOTn_RMSK)
#define HWIO_SECURE_BOOTn_INMI(n,mask)    \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), mask)
#define HWIO_SECURE_BOOTn_FUSE_SRC_BMSK                                                             0x100
#define HWIO_SECURE_BOOTn_FUSE_SRC_SHFT                                                               0x8
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_BMSK                                                        0x40
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_SHFT                                                         0x6
#define HWIO_SECURE_BOOTn_AUTH_EN_BMSK                                                               0x20
#define HWIO_SECURE_BOOTn_AUTH_EN_SHFT                                                                0x5
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_BMSK                                                       0x10
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHFT                                                        0x4
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_BMSK                                                      0xf
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_SHFT                                                      0x0

#define HWIO_OVERRIDE_0_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006058)
#define HWIO_OVERRIDE_0_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006058)
#define HWIO_OVERRIDE_0_RMSK                                                                   0x10000003
#define HWIO_OVERRIDE_0_IN          \
        in_dword_masked(HWIO_OVERRIDE_0_ADDR, HWIO_OVERRIDE_0_RMSK)
#define HWIO_OVERRIDE_0_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_0_ADDR, m)
#define HWIO_OVERRIDE_0_OUT(v)      \
        out_dword(HWIO_OVERRIDE_0_ADDR,v)
#define HWIO_OVERRIDE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_0_ADDR,m,v,HWIO_OVERRIDE_0_IN)
#define HWIO_OVERRIDE_0_TX_DISABLE_BMSK                                                        0x10000000
#define HWIO_OVERRIDE_0_TX_DISABLE_SHFT                                                              0x1c
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_BMSK                                                      0x2
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_SHFT                                                      0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_BMSK                                                0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_SHFT                                                0x0

#define HWIO_OVERRIDE_1_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000605c)
#define HWIO_OVERRIDE_1_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000605c)
#define HWIO_OVERRIDE_1_RMSK                                                                   0xffffffff
#define HWIO_OVERRIDE_1_IN          \
        in_dword_masked(HWIO_OVERRIDE_1_ADDR, HWIO_OVERRIDE_1_RMSK)
#define HWIO_OVERRIDE_1_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_1_ADDR, m)
#define HWIO_OVERRIDE_1_OUT(v)      \
        out_dword(HWIO_OVERRIDE_1_ADDR,v)
#define HWIO_OVERRIDE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_1_ADDR,m,v,HWIO_OVERRIDE_1_IN)
#define HWIO_OVERRIDE_1_OVERRIDE_1_BMSK                                                        0xffffffff
#define HWIO_OVERRIDE_1_OVERRIDE_1_SHFT                                                               0x0

#define HWIO_OVERRIDE_2_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006060)
#define HWIO_OVERRIDE_2_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006060)
#define HWIO_OVERRIDE_2_RMSK                                                                     0x3ffc00
#define HWIO_OVERRIDE_2_IN          \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, HWIO_OVERRIDE_2_RMSK)
#define HWIO_OVERRIDE_2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, m)
#define HWIO_OVERRIDE_2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_2_ADDR,v)
#define HWIO_OVERRIDE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_2_ADDR,m,v,HWIO_OVERRIDE_2_IN)
#define HWIO_OVERRIDE_2_OVRID_DAP_DEVICEEN_DISABLE_BMSK                                          0x200000
#define HWIO_OVERRIDE_2_OVRID_DAP_DEVICEEN_DISABLE_SHFT                                              0x15
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_BMSK                                             0x100000
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_SHFT                                                 0x14
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_BMSK                                              0x80000
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_SHFT                                                 0x13
#define HWIO_OVERRIDE_2_OVRID_APPS_NIDEN_DISABLE_BMSK                                             0x40000
#define HWIO_OVERRIDE_2_OVRID_APPS_NIDEN_DISABLE_SHFT                                                0x12
#define HWIO_OVERRIDE_2_OVRID_APPS_DBGEN_DISABLE_BMSK                                             0x20000
#define HWIO_OVERRIDE_2_OVRID_APPS_DBGEN_DISABLE_SHFT                                                0x11
#define HWIO_OVERRIDE_2_OVRID_SPARE1_DISABLE_BMSK                                                 0x10000
#define HWIO_OVERRIDE_2_OVRID_SPARE1_DISABLE_SHFT                                                    0x10
#define HWIO_OVERRIDE_2_OVRID_SPARE0_DISABLE_BMSK                                                  0x8000
#define HWIO_OVERRIDE_2_OVRID_SPARE0_DISABLE_SHFT                                                     0xf
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_BMSK                                           0x4000
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_SHFT                                              0xe
#define HWIO_OVERRIDE_2_OVRID_RPM_DAPEN_DISABLE_BMSK                                               0x2000
#define HWIO_OVERRIDE_2_OVRID_RPM_DAPEN_DISABLE_SHFT                                                  0xd
#define HWIO_OVERRIDE_2_OVRID_RPM_WCSS_NIDEN_DISABLE_BMSK                                          0x1000
#define HWIO_OVERRIDE_2_OVRID_RPM_WCSS_NIDEN_DISABLE_SHFT                                             0xc
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_BMSK                                                0x800
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_SHFT                                                  0xb
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_BMSK                                               0x400
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_SHFT                                                 0xa

#define HWIO_OVERRIDE_3_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006064)
#define HWIO_OVERRIDE_3_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006064)
#define HWIO_OVERRIDE_3_RMSK                                                                         0x1f
#define HWIO_OVERRIDE_3_IN          \
        in_dword_masked(HWIO_OVERRIDE_3_ADDR, HWIO_OVERRIDE_3_RMSK)
#define HWIO_OVERRIDE_3_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_3_ADDR, m)
#define HWIO_OVERRIDE_3_OUT(v)      \
        out_dword(HWIO_OVERRIDE_3_ADDR,v)
#define HWIO_OVERRIDE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_3_ADDR,m,v,HWIO_OVERRIDE_3_IN)
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_BMSK                                               0x10
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_SHFT                                                0x4
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_BMSK                                                 0x8
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_SHFT                                                 0x3
#define HWIO_OVERRIDE_3_OVRID_APPS_SPNIDEN_DISABLE_BMSK                                               0x4
#define HWIO_OVERRIDE_3_OVRID_APPS_SPNIDEN_DISABLE_SHFT                                               0x2
#define HWIO_OVERRIDE_3_OVRID_APPS_SPIDEN_DISABLE_BMSK                                                0x2
#define HWIO_OVERRIDE_3_OVRID_APPS_SPIDEN_DISABLE_SHFT                                                0x1
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_BMSK                                                   0x1
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_SHFT                                                   0x0

#define HWIO_OVERRIDE_4_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006068)
#define HWIO_OVERRIDE_4_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006068)
#define HWIO_OVERRIDE_4_RMSK                                                                          0x3
#define HWIO_OVERRIDE_4_IN          \
        in_dword_masked(HWIO_OVERRIDE_4_ADDR, HWIO_OVERRIDE_4_RMSK)
#define HWIO_OVERRIDE_4_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_4_ADDR, m)
#define HWIO_OVERRIDE_4_OUT(v)      \
        out_dword(HWIO_OVERRIDE_4_ADDR,v)
#define HWIO_OVERRIDE_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_4_ADDR,m,v,HWIO_OVERRIDE_4_IN)
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_BMSK                                                  0x2
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_SHFT                                                  0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_BMSK                                                  0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_SHFT                                                  0x0

#define HWIO_CAPT_SEC_GPIO_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000606c)
#define HWIO_CAPT_SEC_GPIO_OFFS                                                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000606c)
#define HWIO_CAPT_SEC_GPIO_RMSK                                                                   0x1ffff
#define HWIO_CAPT_SEC_GPIO_IN          \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, HWIO_CAPT_SEC_GPIO_RMSK)
#define HWIO_CAPT_SEC_GPIO_INM(m)      \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, m)
#define HWIO_CAPT_SEC_GPIO_OUT(v)      \
        out_dword(HWIO_CAPT_SEC_GPIO_ADDR,v)
#define HWIO_CAPT_SEC_GPIO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CAPT_SEC_GPIO_ADDR,m,v,HWIO_CAPT_SEC_GPIO_IN)
#define HWIO_CAPT_SEC_GPIO_SDC_EMMC_MODE1P2_EN_BMSK                                               0x10000
#define HWIO_CAPT_SEC_GPIO_SDC_EMMC_MODE1P2_EN_SHFT                                                  0x10
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_BMSK                                                0x8000
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_SHFT                                                   0xf
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_BMSK                                                   0x7fff
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_SHFT                                                      0x0

#define HWIO_APP_PROC_CFG_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006070)
#define HWIO_APP_PROC_CFG_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006070)
#define HWIO_APP_PROC_CFG_RMSK                                                                   0xffffff
#define HWIO_APP_PROC_CFG_IN          \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, HWIO_APP_PROC_CFG_RMSK)
#define HWIO_APP_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, m)
#define HWIO_APP_PROC_CFG_OUT(v)      \
        out_dword(HWIO_APP_PROC_CFG_ADDR,v)
#define HWIO_APP_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APP_PROC_CFG_ADDR,m,v,HWIO_APP_PROC_CFG_IN)
#define HWIO_APP_PROC_CFG_WCSS_DBG_NIDEN_BMSK                                                    0x800000
#define HWIO_APP_PROC_CFG_WCSS_DBG_NIDEN_SHFT                                                        0x17
#define HWIO_APP_PROC_CFG_RPM_DBG_NIDEN_BMSK                                                     0x400000
#define HWIO_APP_PROC_CFG_RPM_DBG_NIDEN_SHFT                                                         0x16
#define HWIO_APP_PROC_CFG_DAP_DBG_NIDEN_BMSK                                                     0x200000
#define HWIO_APP_PROC_CFG_DAP_DBG_NIDEN_SHFT                                                         0x15
#define HWIO_APP_PROC_CFG_DAP_DBG_SPNIDEN_BMSK                                                   0x100000
#define HWIO_APP_PROC_CFG_DAP_DBG_SPNIDEN_SHFT                                                       0x14
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC3_BMSK                                                     0xf0000
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC3_SHFT                                                        0x10
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC2_BMSK                                                      0xf000
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC2_SHFT                                                         0xc
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC1_BMSK                                                       0xf00
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC1_SHFT                                                         0x8
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC0_BMSK                                                        0xf0
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC0_SHFT                                                         0x4
#define HWIO_APP_PROC_CFG_APPS_DBG_NIDEN_BMSK                                                         0x8
#define HWIO_APP_PROC_CFG_APPS_DBG_NIDEN_SHFT                                                         0x3
#define HWIO_APP_PROC_CFG_APPS_DBG_SPNIDEN_BMSK                                                       0x4
#define HWIO_APP_PROC_CFG_APPS_DBG_SPNIDEN_SHFT                                                       0x2
#define HWIO_APP_PROC_CFG_APPS_CP15_DISABLE_BMSK                                                      0x2
#define HWIO_APP_PROC_CFG_APPS_CP15_DISABLE_SHFT                                                      0x1
#define HWIO_APP_PROC_CFG_APPS_CFG_NMFI_BMSK                                                          0x1
#define HWIO_APP_PROC_CFG_APPS_CFG_NMFI_SHFT                                                          0x0

#define HWIO_MSS_PROC_CFG_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006074)
#define HWIO_MSS_PROC_CFG_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006074)
#define HWIO_MSS_PROC_CFG_RMSK                                                                        0x1
#define HWIO_MSS_PROC_CFG_IN          \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, HWIO_MSS_PROC_CFG_RMSK)
#define HWIO_MSS_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, m)
#define HWIO_MSS_PROC_CFG_OUT(v)      \
        out_dword(HWIO_MSS_PROC_CFG_ADDR,v)
#define HWIO_MSS_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_PROC_CFG_ADDR,m,v,HWIO_MSS_PROC_CFG_IN)
#define HWIO_MSS_PROC_CFG_MSS_DBG_NIDEN_BMSK                                                          0x1
#define HWIO_MSS_PROC_CFG_MSS_DBG_NIDEN_SHFT                                                          0x0

#define HWIO_QFPROM_CLK_CTL_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006078)
#define HWIO_QFPROM_CLK_CTL_OFFS                                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006078)
#define HWIO_QFPROM_CLK_CTL_RMSK                                                                      0x1
#define HWIO_QFPROM_CLK_CTL_IN          \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_ADDR, HWIO_QFPROM_CLK_CTL_RMSK)
#define HWIO_QFPROM_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_ADDR, m)
#define HWIO_QFPROM_CLK_CTL_OUT(v)      \
        out_dword(HWIO_QFPROM_CLK_CTL_ADDR,v)
#define HWIO_QFPROM_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CLK_CTL_ADDR,m,v,HWIO_QFPROM_CLK_CTL_IN)
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_BMSK                                                             0x1
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_SHFT                                                             0x0

#define HWIO_JTAG_ID_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000607c)
#define HWIO_JTAG_ID_OFFS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000607c)
#define HWIO_JTAG_ID_RMSK                                                                      0xffffffff
#define HWIO_JTAG_ID_IN          \
        in_dword_masked(HWIO_JTAG_ID_ADDR, HWIO_JTAG_ID_RMSK)
#define HWIO_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_JTAG_ID_ADDR, m)
#define HWIO_JTAG_ID_JTAG_ID_BMSK                                                              0xffffffff
#define HWIO_JTAG_ID_JTAG_ID_SHFT                                                                     0x0

#define HWIO_OEM_ID_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006080)
#define HWIO_OEM_ID_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006080)
#define HWIO_OEM_ID_RMSK                                                                       0xffffffff
#define HWIO_OEM_ID_IN          \
        in_dword_masked(HWIO_OEM_ID_ADDR, HWIO_OEM_ID_RMSK)
#define HWIO_OEM_ID_INM(m)      \
        in_dword_masked(HWIO_OEM_ID_ADDR, m)
#define HWIO_OEM_ID_OEM_ID_BMSK                                                                0xffff0000
#define HWIO_OEM_ID_OEM_ID_SHFT                                                                      0x10
#define HWIO_OEM_ID_OEM_PRODUCT_ID_BMSK                                                            0xffff
#define HWIO_OEM_ID_OEM_PRODUCT_ID_SHFT                                                               0x0

#define HWIO_TEST_BUS_SEL_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006084)
#define HWIO_TEST_BUS_SEL_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006084)
#define HWIO_TEST_BUS_SEL_RMSK                                                                        0xf
#define HWIO_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, HWIO_TEST_BUS_SEL_RMSK)
#define HWIO_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, m)
#define HWIO_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_TEST_BUS_SEL_ADDR,v)
#define HWIO_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TEST_BUS_SEL_ADDR,m,v,HWIO_TEST_BUS_SEL_IN)
#define HWIO_TEST_BUS_SEL_TEST_EN_BMSK                                                                0x8
#define HWIO_TEST_BUS_SEL_TEST_EN_SHFT                                                                0x3
#define HWIO_TEST_BUS_SEL_TEST_SELECT_BMSK                                                            0x7
#define HWIO_TEST_BUS_SEL_TEST_SELECT_SHFT                                                            0x0

#define HWIO_SPDM_DYN_SECURE_MODE_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006088)
#define HWIO_SPDM_DYN_SECURE_MODE_OFFS                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006088)
#define HWIO_SPDM_DYN_SECURE_MODE_RMSK                                                                0x1
#define HWIO_SPDM_DYN_SECURE_MODE_IN          \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_ADDR, HWIO_SPDM_DYN_SECURE_MODE_RMSK)
#define HWIO_SPDM_DYN_SECURE_MODE_INM(m)      \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_ADDR, m)
#define HWIO_SPDM_DYN_SECURE_MODE_OUT(v)      \
        out_dword(HWIO_SPDM_DYN_SECURE_MODE_ADDR,v)
#define HWIO_SPDM_DYN_SECURE_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPDM_DYN_SECURE_MODE_ADDR,m,v,HWIO_SPDM_DYN_SECURE_MODE_IN)
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_BMSK                                                    0x1
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                                   (TLMM_BASE      + 0x00000000)
#define TLMM_CSR_REG_BASE_OFFS                                                              0x00000000

#define HWIO_TLMM_GPIO_CFGn_ADDR(n)                                                         (TLMM_CSR_REG_BASE      + 0x00000000 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_CFGn_OFFS(n)                                                         (TLMM_CSR_REG_BASE_OFFS + 0x00000000 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_CFGn_RMSK                                                                 0x7ff
#define HWIO_TLMM_GPIO_CFGn_MAXn                                                                    99
#define HWIO_TLMM_GPIO_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), HWIO_TLMM_GPIO_CFGn_RMSK)
#define HWIO_TLMM_GPIO_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_CFGn_INI(n))
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_BMSK                                                   0x400
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_SHFT                                                     0xa
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_BMSK                                                         0x200
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_SHFT                                                           0x9
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_BMSK                                                    0x1c0
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_SHFT                                                      0x6
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_BMSK                                                         0x3c
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_SHFT                                                          0x2
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_BMSK                                                         0x3
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_IN_OUTn_ADDR(n)                                                      (TLMM_CSR_REG_BASE      + 0x00000004 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_OFFS(n)                                                      (TLMM_CSR_REG_BASE_OFFS + 0x00000004 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_RMSK                                                                0x3
#define HWIO_TLMM_GPIO_IN_OUTn_MAXn                                                                 99
#define HWIO_TLMM_GPIO_IN_OUTn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), HWIO_TLMM_GPIO_IN_OUTn_RMSK)
#define HWIO_TLMM_GPIO_IN_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),val)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),mask,val,HWIO_TLMM_GPIO_IN_OUTn_INI(n))
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_BMSK                                                       0x2
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_SHFT                                                       0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_BMSK                                                        0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_SHFT                                                        0x0

#define HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n)                                                    (TLMM_CSR_REG_BASE      + 0x00000008 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_CFGn_OFFS(n)                                                    (TLMM_CSR_REG_BASE_OFFS + 0x00000008 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_CFGn_RMSK                                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFGn_MAXn                                                               99
#define HWIO_TLMM_GPIO_INTR_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), HWIO_TLMM_GPIO_INTR_CFGn_RMSK)
#define HWIO_TLMM_GPIO_INTR_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_CFGn_INI(n))
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_BMSK                                                0x100
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_SHFT                                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_BMSK                                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_SHFT                                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_BMSK                                          0x10
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_SHFT                                           0x4
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_BMSK                                                0xc
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_SHFT                                                0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_BMSK                                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_SHFT                                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_BMSK                                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_SHFT                                                  0x0

#define HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n)                                                 (TLMM_CSR_REG_BASE      + 0x0000000c + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_OFFS(n)                                                 (TLMM_CSR_REG_BASE_OFFS + 0x0000000c + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_RMSK                                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_MAXn                                                            99
#define HWIO_TLMM_GPIO_INTR_STATUSn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), HWIO_TLMM_GPIO_INTR_STATUSn_RMSK)
#define HWIO_TLMM_GPIO_INTR_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_STATUSn_INI(n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_BMSK                                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_SHFT                                               0x0

#define HWIO_TLMM_CLK_GATE_EN_ADDR                                                          (TLMM_CSR_REG_BASE      + 0x00100000)
#define HWIO_TLMM_CLK_GATE_EN_OFFS                                                          (TLMM_CSR_REG_BASE_OFFS + 0x00100000)
#define HWIO_TLMM_CLK_GATE_EN_RMSK                                                                 0x7
#define HWIO_TLMM_CLK_GATE_EN_IN          \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, HWIO_TLMM_CLK_GATE_EN_RMSK)
#define HWIO_TLMM_CLK_GATE_EN_INM(m)      \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, m)
#define HWIO_TLMM_CLK_GATE_EN_OUT(v)      \
        out_dword(HWIO_TLMM_CLK_GATE_EN_ADDR,v)
#define HWIO_TLMM_CLK_GATE_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_CLK_GATE_EN_ADDR,m,v,HWIO_TLMM_CLK_GATE_EN_IN)
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_BMSK                                                     0x4
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_SHFT                                                     0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_BMSK                                                 0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_SHFT                                                 0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_BMSK                                                    0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_SHFT                                                    0x0

#define HWIO_TLMM_IE_CTRL_DISABLE_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x00100004)
#define HWIO_TLMM_IE_CTRL_DISABLE_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x00100004)
#define HWIO_TLMM_IE_CTRL_DISABLE_RMSK                                                             0x3
#define HWIO_TLMM_IE_CTRL_DISABLE_IN          \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, HWIO_TLMM_IE_CTRL_DISABLE_RMSK)
#define HWIO_TLMM_IE_CTRL_DISABLE_INM(m)      \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, m)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUT(v)      \
        out_dword(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,v)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,m,v,HWIO_TLMM_IE_CTRL_DISABLE_IN)
#define HWIO_TLMM_IE_CTRL_DISABLE_SLIMBUS_GPIO_CORE_IE_CTRL_BMSK                                   0x2
#define HWIO_TLMM_IE_CTRL_DISABLE_SLIMBUS_GPIO_CORE_IE_CTRL_SHFT                                   0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_BMSK                                             0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_SHFT                                             0x0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR                                                  (TLMM_CSR_REG_BASE      + 0x00100008)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OFFS                                                  (TLMM_CSR_REG_BASE_OFFS + 0x00100008)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK                                                   0x7ffffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN          \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_61_BMSK                                           0x4000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_61_SHFT                                                0x1a
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_15_BMSK                                           0x2000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_15_SHFT                                                0x19
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_56_BMSK                                           0x1000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_56_SHFT                                                0x18
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_55_BMSK                                            0x800000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_55_SHFT                                                0x17
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_54_BMSK                                            0x400000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_54_SHFT                                                0x16
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_53_BMSK                                            0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_53_SHFT                                                0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_51_BMSK                                            0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_51_SHFT                                                0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_BMSK                                             0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_SHFT                                                0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_43_BMSK                                             0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_43_SHFT                                                0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_25_BMSK                                             0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_25_SHFT                                                0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_24_BMSK                                             0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_24_SHFT                                                0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_22_BMSK                                              0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_22_SHFT                                                 0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_BMSK                                              0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_SHFT                                                 0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_19_BMSK                                              0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_19_SHFT                                                 0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_18_BMSK                                              0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_18_SHFT                                                 0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_17_BMSK                                               0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_17_SHFT                                                 0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_16_BMSK                                               0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_16_SHFT                                                 0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_14_BMSK                                               0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_14_SHFT                                                 0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_BMSK                                               0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_SHFT                                                 0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_12_BMSK                                                0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_12_SHFT                                                 0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_11_BMSK                                                0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_11_SHFT                                                 0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_10_BMSK                                                0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_10_SHFT                                                 0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_9_BMSK                                                 0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_9_SHFT                                                  0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_6_BMSK                                                  0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_6_SHFT                                                  0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_BMSK                                                  0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_SHFT                                                  0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_2_BMSK                                                  0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_2_SHFT                                                  0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_BMSK                                                  0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_SHFT                                                  0x0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR                                                  (TLMM_CSR_REG_BASE      + 0x0010000c)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OFFS                                                  (TLMM_CSR_REG_BASE_OFFS + 0x0010000c)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK                                                   0x7ffffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN          \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR, HWIO_TLMM_MPM_WAKEUP_INT_EN_1_RMSK)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_1_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_1_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_3_BMSK                                       0x4000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_3_SHFT                                            0x1a
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_1_BMSK                                       0x2000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SDC1_DATA_1_SHFT                                            0x19
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_BMSK                                            0x1000000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_SRST_N_SHFT                                                 0x18
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_95_BMSK                                            0x800000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_95_SHFT                                                0x17
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_94_BMSK                                            0x400000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_94_SHFT                                                0x16
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_93_BMSK                                            0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_93_SHFT                                                0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_92_BMSK                                            0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_92_SHFT                                                0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_91_BMSK                                             0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_91_SHFT                                                0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_88_BMSK                                             0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_88_SHFT                                                0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_87_BMSK                                             0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_87_SHFT                                                0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_86_BMSK                                             0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_86_SHFT                                                0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_85_BMSK                                              0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_85_SHFT                                                 0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_84_BMSK                                              0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_84_SHFT                                                 0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_83_BMSK                                              0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_83_SHFT                                                 0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_82_BMSK                                              0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_82_SHFT                                                 0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_77_BMSK                                               0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_77_SHFT                                                 0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_70_BMSK                                               0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_70_SHFT                                                 0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_68_BMSK                                               0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_68_SHFT                                                 0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_67_BMSK                                               0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_67_SHFT                                                 0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_66_BMSK                                                0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_66_SHFT                                                 0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_65_BMSK                                                0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_65_SHFT                                                 0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_64_BMSK                                                0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_64_SHFT                                                 0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_63_BMSK                                                0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_63_SHFT                                                 0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_62_BMSK                                                 0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_62_SHFT                                                 0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_60_BMSK                                                 0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_60_SHFT                                                 0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_81_BMSK                                                 0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_81_SHFT                                                 0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_58_BMSK                                                 0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_1_GPIO_58_SHFT                                                 0x0

#define HWIO_TLMM_INT_JTAG_CTL_ADDR                                                         (TLMM_CSR_REG_BASE      + 0x00110000)
#define HWIO_TLMM_INT_JTAG_CTL_OFFS                                                         (TLMM_CSR_REG_BASE_OFFS + 0x00110000)
#define HWIO_TLMM_INT_JTAG_CTL_RMSK                                                                0xf
#define HWIO_TLMM_INT_JTAG_CTL_IN          \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, HWIO_TLMM_INT_JTAG_CTL_RMSK)
#define HWIO_TLMM_INT_JTAG_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, m)
#define HWIO_TLMM_INT_JTAG_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_INT_JTAG_CTL_ADDR,v)
#define HWIO_TLMM_INT_JTAG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_INT_JTAG_CTL_ADDR,m,v,HWIO_TLMM_INT_JTAG_CTL_IN)
#define HWIO_TLMM_INT_JTAG_CTL_APSS_TAP_ENA_BMSK                                                   0x8
#define HWIO_TLMM_INT_JTAG_CTL_APSS_TAP_ENA_SHFT                                                   0x3
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_BMSK                                                   0x4
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_SHFT                                                   0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_BMSK                                                    0x2
#define HWIO_TLMM_INT_JTAG_CTL_ACC_TAP_ENA_SHFT                                                    0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_BMSK                                                    0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_SHFT                                                    0x0

#define HWIO_TLMM_ETM_MODE_ADDR                                                             (TLMM_CSR_REG_BASE      + 0x00110004)
#define HWIO_TLMM_ETM_MODE_OFFS                                                             (TLMM_CSR_REG_BASE_OFFS + 0x00110004)
#define HWIO_TLMM_ETM_MODE_RMSK                                                                    0x3
#define HWIO_TLMM_ETM_MODE_IN          \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, HWIO_TLMM_ETM_MODE_RMSK)
#define HWIO_TLMM_ETM_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, m)
#define HWIO_TLMM_ETM_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_ETM_MODE_ADDR,v)
#define HWIO_TLMM_ETM_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_ETM_MODE_ADDR,m,v,HWIO_TLMM_ETM_MODE_IN)
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC1_BMSK                                                    0x3
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC1_SHFT                                                    0x0

#define HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x00110008)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x00110008)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK                                                             0x7
#define HWIO_TLMM_DBG_BUS_OUT_SEL_IN          \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_INM(m)      \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, m)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUT(v)      \
        out_dword(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,v)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,m,v,HWIO_TLMM_DBG_BUS_OUT_SEL_IN)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_QDSS_ETM_BYTE_SHIFT_BMSK                                         0x4
#define HWIO_TLMM_DBG_BUS_OUT_SEL_QDSS_ETM_BYTE_SHIFT_SHFT                                         0x2
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_BMSK                                                    0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_SHFT                                                    0x0

#define HWIO_TLMM_CHIP_MODE_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x0011000c)
#define HWIO_TLMM_CHIP_MODE_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x0011000c)
#define HWIO_TLMM_CHIP_MODE_RMSK                                                                   0x3
#define HWIO_TLMM_CHIP_MODE_IN          \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, HWIO_TLMM_CHIP_MODE_RMSK)
#define HWIO_TLMM_CHIP_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, m)
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_BMSK                                                         0x2
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_SHFT                                                         0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_BMSK                                                         0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_SHFT                                                         0x0

#define HWIO_TLMM_SPARE_ADDR                                                                (TLMM_CSR_REG_BASE      + 0x0010e000)
#define HWIO_TLMM_SPARE_OFFS                                                                (TLMM_CSR_REG_BASE_OFFS + 0x0010e000)
#define HWIO_TLMM_SPARE_RMSK                                                                0xffffffff
#define HWIO_TLMM_SPARE_IN          \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR, HWIO_TLMM_SPARE_RMSK)
#define HWIO_TLMM_SPARE_INM(m)      \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR, m)
#define HWIO_TLMM_SPARE_OUT(v)      \
        out_dword(HWIO_TLMM_SPARE_ADDR,v)
#define HWIO_TLMM_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SPARE_ADDR,m,v,HWIO_TLMM_SPARE_IN)
#define HWIO_TLMM_SPARE_SPARE_BMSK                                                          0xffffff00
#define HWIO_TLMM_SPARE_SPARE_SHFT                                                                 0x8
#define HWIO_TLMM_SPARE_MISC_BMSK                                                                 0xff
#define HWIO_TLMM_SPARE_MISC_SHFT                                                                  0x0

#define HWIO_SPARE1_ADDR                                                                    (TLMM_CSR_REG_BASE      + 0x0011001c)
#define HWIO_SPARE1_OFFS                                                                    (TLMM_CSR_REG_BASE_OFFS + 0x0011001c)
#define HWIO_SPARE1_RMSK                                                                    0xffffffff
#define HWIO_SPARE1_IN          \
        in_dword_masked(HWIO_SPARE1_ADDR, HWIO_SPARE1_RMSK)
#define HWIO_SPARE1_INM(m)      \
        in_dword_masked(HWIO_SPARE1_ADDR, m)
#define HWIO_SPARE1_OUT(v)      \
        out_dword(HWIO_SPARE1_ADDR,v)
#define HWIO_SPARE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPARE1_ADDR,m,v,HWIO_SPARE1_IN)
#define HWIO_SPARE1_MISC1_BMSK                                                              0xfffffffe
#define HWIO_SPARE1_MISC1_SHFT                                                                     0x1
#define HWIO_SPARE1_SDC1_CLK_ONTO_DEBUG_BUS_BMSK                                                   0x1
#define HWIO_SPARE1_SDC1_CLK_ONTO_DEBUG_BUS_SHFT                                                   0x0

#define HWIO_SPARE2_ADDR                                                                    (TLMM_CSR_REG_BASE      + 0x00110020)
#define HWIO_SPARE2_OFFS                                                                    (TLMM_CSR_REG_BASE_OFFS + 0x00110020)
#define HWIO_SPARE2_RMSK                                                                    0xffffffff
#define HWIO_SPARE2_IN          \
        in_dword_masked(HWIO_SPARE2_ADDR, HWIO_SPARE2_RMSK)
#define HWIO_SPARE2_INM(m)      \
        in_dword_masked(HWIO_SPARE2_ADDR, m)
#define HWIO_SPARE2_OUT(v)      \
        out_dword(HWIO_SPARE2_ADDR,v)
#define HWIO_SPARE2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPARE2_ADDR,m,v,HWIO_SPARE2_IN)
#define HWIO_SPARE2_MISC2_BMSK                                                              0xffffffff
#define HWIO_SPARE2_MISC2_SHFT                                                                     0x0

#define HWIO_TLMM_HW_REVISION_NUMBER_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x00110010)
#define HWIO_TLMM_HW_REVISION_NUMBER_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x00110010)
#define HWIO_TLMM_HW_REVISION_NUMBER_RMSK                                                   0xffffffff
#define HWIO_TLMM_HW_REVISION_NUMBER_IN          \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, HWIO_TLMM_HW_REVISION_NUMBER_RMSK)
#define HWIO_TLMM_HW_REVISION_NUMBER_INM(m)      \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, m)
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_BMSK                                        0xf0000000
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_SHFT                                              0x1c
#define HWIO_TLMM_HW_REVISION_NUMBER_PARTNUM_BMSK                                            0xffff000
#define HWIO_TLMM_HW_REVISION_NUMBER_PARTNUM_SHFT                                                  0xc
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_BMSK                                        0xffe
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_SHFT                                          0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_BMSK                                                0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_SHFT                                                0x0

#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR                                                (TLMM_CSR_REG_BASE      + 0x00110014)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OFFS                                                (TLMM_CSR_REG_BASE_OFFS + 0x00110014)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_RMSK                                                       0x1
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_IN          \
        in_dword_masked(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR, HWIO_TLMM_PERIPH_CHAR_TEST_MODE_RMSK)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR, m)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR,v)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR,m,v,HWIO_TLMM_PERIPH_CHAR_TEST_MODE_IN)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_BMSK                                        0x1
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_SHFT                                        0x0

#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x00111000)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x00111000)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_RMSK                                                          0x7
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_IN          \
        in_dword_masked(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR, HWIO_TLMM_EBI2_EMMC_GPIO_CFG_RMSK)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_INM(m)      \
        in_dword_masked(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR, m)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OUT(v)      \
        out_dword(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR,v)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR,m,v,HWIO_TLMM_EBI2_EMMC_GPIO_CFG_IN)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EMMC_BOOT_SELECT_BMSK                                         0x4
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EMMC_BOOT_SELECT_SHFT                                         0x2
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BOOT_SELECT_BMSK                                         0x2
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BOOT_SELECT_SHFT                                         0x1
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_PBL_DEBUG_BMSK                                                0x1
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_PBL_DEBUG_SHFT                                                0x0

#define HWIO_TLMM_RFFE_CTL_ADDR                                                             (TLMM_CSR_REG_BASE      + 0x00108000)
#define HWIO_TLMM_RFFE_CTL_OFFS                                                             (TLMM_CSR_REG_BASE_OFFS + 0x00108000)
#define HWIO_TLMM_RFFE_CTL_RMSK                                                              0xfffffff
#define HWIO_TLMM_RFFE_CTL_IN          \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, HWIO_TLMM_RFFE_CTL_RMSK)
#define HWIO_TLMM_RFFE_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, m)
#define HWIO_TLMM_RFFE_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RFFE_CTL_ADDR,v)
#define HWIO_TLMM_RFFE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RFFE_CTL_ADDR,m,v,HWIO_TLMM_RFFE_CTL_IN)
#define HWIO_TLMM_RFFE_CTL_RFFE7_DATA_SR_CTL_EN_BMSK                                         0xc000000
#define HWIO_TLMM_RFFE_CTL_RFFE7_DATA_SR_CTL_EN_SHFT                                              0x1a
#define HWIO_TLMM_RFFE_CTL_RFFE7_CLK_SR_CTL_EN_BMSK                                          0x3000000
#define HWIO_TLMM_RFFE_CTL_RFFE7_CLK_SR_CTL_EN_SHFT                                               0x18
#define HWIO_TLMM_RFFE_CTL_RFFE6_DATA_SR_CTL_EN_BMSK                                          0xc00000
#define HWIO_TLMM_RFFE_CTL_RFFE6_DATA_SR_CTL_EN_SHFT                                              0x16
#define HWIO_TLMM_RFFE_CTL_RFFE6_CLK_SR_CTL_EN_BMSK                                           0x300000
#define HWIO_TLMM_RFFE_CTL_RFFE6_CLK_SR_CTL_EN_SHFT                                               0x14
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SR_CTL_EN_BMSK                                           0xc0000
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SR_CTL_EN_SHFT                                              0x12
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SR_CTL_EN_BMSK                                            0x30000
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SR_CTL_EN_SHFT                                               0x10
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SR_CTL_EN_BMSK                                            0xc000
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SR_CTL_EN_SHFT                                               0xe
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SR_CTL_EN_BMSK                                             0x3000
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SR_CTL_EN_SHFT                                                0xc
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SR_CTL_EN_BMSK                                             0xc00
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SR_CTL_EN_SHFT                                               0xa
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SR_CTL_EN_BMSK                                              0x300
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SR_CTL_EN_SHFT                                                0x8
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_BMSK                                              0xc0
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_SHFT                                               0x6
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_BMSK                                               0x30
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_SHFT                                                0x4
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_BMSK                                               0xc
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_SHFT                                               0x2
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_BMSK                                                0x3
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_SHFT                                                0x0

#define HWIO_TLMM_RESOUT_HDRV_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x0010d000)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x0010d000)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RMSK                                                             0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_IN          \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, HWIO_TLMM_RESOUT_HDRV_CTL_RMSK)
#define HWIO_TLMM_RESOUT_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,m,v,HWIO_TLMM_RESOUT_HDRV_CTL_IN)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_BMSK                                               0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_SHFT                                               0x0

#define HWIO_TLMM_JTAG_HDRV_CTL_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x0010c000)
#define HWIO_TLMM_JTAG_HDRV_CTL_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x0010c000)
#define HWIO_TLMM_JTAG_HDRV_CTL_RMSK                                                             0x3ff
#define HWIO_TLMM_JTAG_HDRV_CTL_IN          \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, HWIO_TLMM_JTAG_HDRV_CTL_RMSK)
#define HWIO_TLMM_JTAG_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,m,v,HWIO_TLMM_JTAG_HDRV_CTL_IN)
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_BMSK                                                    0x300
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_SHFT                                                      0x8
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_BMSK                                                     0xc0
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_SHFT                                                      0x6
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_BMSK                                                     0x38
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_SHFT                                                      0x3
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_BMSK                                                      0x7
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_SHFT                                                      0x0

#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x0010b000)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x0010b000)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK                                                     0xffffff
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HYS_CTL_BMSK                              0x800000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HYS_CTL_SHFT                                  0x17
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HYS_CTL_BMSK                               0x400000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HYS_CTL_SHFT                                   0x16
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SR_CTL_EN_BMSK                            0x300000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SR_CTL_EN_SHFT                                0x14
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SR_CTL_EN_BMSK                              0xc0000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SR_CTL_EN_SHFT                                 0x12
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SEN_EN_BMSK                                0x20000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SEN_EN_SHFT                                   0x11
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SEN_EN_BMSK                                 0x10000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SEN_EN_SHFT                                    0x10
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_BMSK                                   0xc000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_SHFT                                      0xe
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_BMSK                                    0x3000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_SHFT                                       0xc
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_BMSK                                            0xe00
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_SHFT                                              0x9
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_BMSK                                            0x1c0
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_SHFT                                              0x6
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_BMSK                                     0x38
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_SHFT                                      0x3
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_BMSK                                       0x7
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_SHFT                                       0x0

#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x0010a000)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x0010a000)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK                                                       0xffff
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_HYS_CTL_BMSK                                          0x8000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_HYS_CTL_SHFT                                             0xf
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_BMSK                                         0x6000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_SHFT                                            0xd
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_BMSK                                         0x1800
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_SHFT                                            0xb
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_BMSK                                         0x600
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_SHFT                                           0x9
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_BMSK                                          0x1c0
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_SHFT                                            0x6
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_BMSK                                           0x38
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_SHFT                                            0x3
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_BMSK                                           0x7
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_SHFT                                           0x0

#define HWIO_TLMM_SLIMBUS_CTL_ADDR                                                          (TLMM_CSR_REG_BASE      + 0x00109000)
#define HWIO_TLMM_SLIMBUS_CTL_OFFS                                                          (TLMM_CSR_REG_BASE_OFFS + 0x00109000)
#define HWIO_TLMM_SLIMBUS_CTL_RMSK                                                                 0x3
#define HWIO_TLMM_SLIMBUS_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SLIMBUS_CTL_ADDR, HWIO_TLMM_SLIMBUS_CTL_RMSK)
#define HWIO_TLMM_SLIMBUS_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SLIMBUS_CTL_ADDR, m)
#define HWIO_TLMM_SLIMBUS_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SLIMBUS_CTL_ADDR,v)
#define HWIO_TLMM_SLIMBUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SLIMBUS_CTL_ADDR,m,v,HWIO_TLMM_SLIMBUS_CTL_IN)
#define HWIO_TLMM_SLIMBUS_CTL_SR_CTL_EN_BMSK                                                       0x3
#define HWIO_TLMM_SLIMBUS_CTL_SR_CTL_EN_SHFT                                                       0x0

#define HWIO_TLMM_MODE_PULL_CTL_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x00107000)
#define HWIO_TLMM_MODE_PULL_CTL_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x00107000)
#define HWIO_TLMM_MODE_PULL_CTL_RMSK                                                               0xf
#define HWIO_TLMM_MODE_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, HWIO_TLMM_MODE_PULL_CTL_RMSK)
#define HWIO_TLMM_MODE_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, m)
#define HWIO_TLMM_MODE_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_MODE_PULL_CTL_ADDR,v)
#define HWIO_TLMM_MODE_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MODE_PULL_CTL_ADDR,m,v,HWIO_TLMM_MODE_PULL_CTL_IN)
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_BMSK                                                   0xc
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_SHFT                                                   0x2
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_BMSK                                                   0x3
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_SHFT                                                   0x0

#define HWIO_RCP_SWITCH_CTL_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00112000)
#define HWIO_RCP_SWITCH_CTL_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00112000)
#define HWIO_RCP_SWITCH_CTL_RMSK                                                                  0x7f
#define HWIO_RCP_SWITCH_CTL_IN          \
        in_dword_masked(HWIO_RCP_SWITCH_CTL_ADDR, HWIO_RCP_SWITCH_CTL_RMSK)
#define HWIO_RCP_SWITCH_CTL_INM(m)      \
        in_dword_masked(HWIO_RCP_SWITCH_CTL_ADDR, m)
#define HWIO_RCP_SWITCH_CTL_OUT(v)      \
        out_dword(HWIO_RCP_SWITCH_CTL_ADDR,v)
#define HWIO_RCP_SWITCH_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RCP_SWITCH_CTL_ADDR,m,v,HWIO_RCP_SWITCH_CTL_IN)
#define HWIO_RCP_SWITCH_CTL_RCP_ATB_SEL_1_BMSK                                                    0x70
#define HWIO_RCP_SWITCH_CTL_RCP_ATB_SEL_1_SHFT                                                     0x4
#define HWIO_RCP_SWITCH_CTL_RCP_ATB_SEL_0_BMSK                                                     0xe
#define HWIO_RCP_SWITCH_CTL_RCP_ATB_SEL_0_SHFT                                                     0x1
#define HWIO_RCP_SWITCH_CTL_RCP_SWITCH_BYPASS_BMSK                                                 0x1
#define HWIO_RCP_SWITCH_CTL_RCP_SWITCH_BYPASS_SHFT                                                 0x0

#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0019c000)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_OFFS                                        (TLMM_CSR_REG_BASE_OFFS + 0x0019c000)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_RMSK                                        0xffffffff
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR, HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_RMSK)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_ADDR,m,v,HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_IN)
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_QDSD_HDRV_PULL_DEBUG_GPIO_REG_BMSK          0xffffffff
#define HWIO_TLMM_QDSD_HDRV_PULL_DEBUG_GPIO_CTL_QDSD_HDRV_PULL_DEBUG_GPIO_REG_SHFT                 0x0

#define HWIO_TLMM_QDSD_BOOT_CTL_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x0019d000)
#define HWIO_TLMM_QDSD_BOOT_CTL_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x0019d000)
#define HWIO_TLMM_QDSD_BOOT_CTL_RMSK                                                            0xffff
#define HWIO_TLMM_QDSD_BOOT_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_BOOT_CTL_ADDR, HWIO_TLMM_QDSD_BOOT_CTL_RMSK)
#define HWIO_TLMM_QDSD_BOOT_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_BOOT_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_BOOT_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_BOOT_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_BOOT_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_BOOT_CTL_ADDR,m,v,HWIO_TLMM_QDSD_BOOT_CTL_IN)
#define HWIO_TLMM_QDSD_BOOT_CTL_QDSD_BOOT_REG_BMSK                                              0xffff
#define HWIO_TLMM_QDSD_BOOT_CTL_QDSD_BOOT_REG_SHFT                                                 0x0

#define HWIO_TLMM_QDSD_CONFIG_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x0019e000)
#define HWIO_TLMM_QDSD_CONFIG_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x0019e000)
#define HWIO_TLMM_QDSD_CONFIG_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR, HWIO_TLMM_QDSD_CONFIG_CTL_RMSK)
#define HWIO_TLMM_QDSD_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_CONFIG_CTL_ADDR,m,v,HWIO_TLMM_QDSD_CONFIG_CTL_IN)
#define HWIO_TLMM_QDSD_CONFIG_CTL_QDSD_CONFIG_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_CONFIG_CTL_QDSD_CONFIG_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_STATUS_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x0019f000)
#define HWIO_TLMM_QDSD_STATUS_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x0019f000)
#define HWIO_TLMM_QDSD_STATUS_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_STATUS_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_STATUS_CTL_ADDR, HWIO_TLMM_QDSD_STATUS_CTL_RMSK)
#define HWIO_TLMM_QDSD_STATUS_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_STATUS_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_STATUS_CTL_QDSD_STATUS_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_STATUS_CTL_QDSD_STATUS_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_ADDR                                             (TLMM_CSR_REG_BASE      + 0x001a0000)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_OFFS                                             (TLMM_CSR_REG_BASE_OFFS + 0x001a0000)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_RMSK                                             0x3fffffff
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_ADDR, HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_QDSD_DEBUG_HDRV_PULL_REG_BMSK                    0x3fffffff
#define HWIO_TLMM_QDSD_DEBUG_HDRV_PULL_CTL_QDSD_DEBUG_HDRV_PULL_REG_SHFT                           0x0

#define HWIO_TLMM_QDSD_GPIO_CTL_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x001a1000)
#define HWIO_TLMM_QDSD_GPIO_CTL_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x001a1000)
#define HWIO_TLMM_QDSD_GPIO_CTL_RMSK                                                            0xffff
#define HWIO_TLMM_QDSD_GPIO_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_GPIO_CTL_ADDR, HWIO_TLMM_QDSD_GPIO_CTL_RMSK)
#define HWIO_TLMM_QDSD_GPIO_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_GPIO_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_GPIO_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_GPIO_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_GPIO_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_GPIO_CTL_ADDR,m,v,HWIO_TLMM_QDSD_GPIO_CTL_IN)
#define HWIO_TLMM_QDSD_GPIO_CTL_QDSD_GPIO_REG_BMSK                                              0xffff
#define HWIO_TLMM_QDSD_GPIO_CTL_QDSD_GPIO_REG_SHFT                                                 0x0

#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR                                                 (TLMM_CSR_REG_BASE      + 0x001a2000)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_OFFS                                                 (TLMM_CSR_REG_BASE_OFFS + 0x001a2000)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_RMSK                                                     0xffff
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR, HWIO_TLMM_QDSD_INTR_ENABLE_CTL_RMSK)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_INTR_ENABLE_CTL_ADDR,m,v,HWIO_TLMM_QDSD_INTR_ENABLE_CTL_IN)
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_QDSD_INTR_ENABLE_REG_BMSK                                0xffff
#define HWIO_TLMM_QDSD_INTR_ENABLE_CTL_QDSD_INTR_ENABLE_REG_SHFT                                   0x0

#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR                                                  (TLMM_CSR_REG_BASE      + 0x001a3000)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_OFFS                                                  (TLMM_CSR_REG_BASE_OFFS + 0x001a3000)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_RMSK                                                      0xffff
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR, HWIO_TLMM_QDSD_INTR_CLEAR_CTL_RMSK)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_INTR_CLEAR_CTL_ADDR,m,v,HWIO_TLMM_QDSD_INTR_CLEAR_CTL_IN)
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_QDSD_INTR_CLEAR_REG_BMSK                                  0xffff
#define HWIO_TLMM_QDSD_INTR_CLEAR_CTL_QDSD_INTR_CLEAR_REG_SHFT                                     0x0

#define HWIO_TLMM_QDSD_SPARE1_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x001a5000)
#define HWIO_TLMM_QDSD_SPARE1_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x001a5000)
#define HWIO_TLMM_QDSD_SPARE1_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE1_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR, HWIO_TLMM_QDSD_SPARE1_CTL_RMSK)
#define HWIO_TLMM_QDSD_SPARE1_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_SPARE1_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_SPARE1_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_SPARE1_CTL_ADDR,m,v,HWIO_TLMM_QDSD_SPARE1_CTL_IN)
#define HWIO_TLMM_QDSD_SPARE1_CTL_QDSD_SPARE1_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE1_CTL_QDSD_SPARE1_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_SPARE2_CTL_ADDR                                                      (TLMM_CSR_REG_BASE      + 0x001a6000)
#define HWIO_TLMM_QDSD_SPARE2_CTL_OFFS                                                      (TLMM_CSR_REG_BASE_OFFS + 0x001a6000)
#define HWIO_TLMM_QDSD_SPARE2_CTL_RMSK                                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE2_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR, HWIO_TLMM_QDSD_SPARE2_CTL_RMSK)
#define HWIO_TLMM_QDSD_SPARE2_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_SPARE2_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_SPARE2_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_SPARE2_CTL_ADDR,m,v,HWIO_TLMM_QDSD_SPARE2_CTL_IN)
#define HWIO_TLMM_QDSD_SPARE2_CTL_QDSD_SPARE2_REG_BMSK                                      0xffffffff
#define HWIO_TLMM_QDSD_SPARE2_CTL_QDSD_SPARE2_REG_SHFT                                             0x0

#define HWIO_TLMM_QDSD_HYS_CTL_ADDR                                                         (TLMM_CSR_REG_BASE      + 0x001a7000)
#define HWIO_TLMM_QDSD_HYS_CTL_OFFS                                                         (TLMM_CSR_REG_BASE_OFFS + 0x001a7000)
#define HWIO_TLMM_QDSD_HYS_CTL_RMSK                                                                0x1
#define HWIO_TLMM_QDSD_HYS_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_HYS_CTL_ADDR, HWIO_TLMM_QDSD_HYS_CTL_RMSK)
#define HWIO_TLMM_QDSD_HYS_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_HYS_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_HYS_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_HYS_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_HYS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_HYS_CTL_ADDR,m,v,HWIO_TLMM_QDSD_HYS_CTL_IN)
#define HWIO_TLMM_QDSD_HYS_CTL_QDSD_HYS_CTL_BMSK                                                   0x1
#define HWIO_TLMM_QDSD_HYS_CTL_QDSD_HYS_CTL_SHFT                                                   0x0

#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR                                               (TLMM_CSR_REG_BASE      + 0x001a4000)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_OFFS                                               (TLMM_CSR_REG_BASE_OFFS + 0x001a4000)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_RMSK                                               0xffffffff
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_IN          \
        in_dword_masked(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR, HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_RMSK)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR, m)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR,v)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_ADDR,m,v,HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_IN)
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_QDSD_TIMEOUT_VALUE_REG_BMSK                        0xffffffff
#define HWIO_TLMM_QDSD_TIMEOUT_VALUE_CTL_QDSD_TIMEOUT_VALUE_REG_SHFT                               0x0

#define HWIO_TLMM_GPIO_I2C_PAD_CTL_ADDR                                                     (TLMM_CSR_REG_BASE      + 0x0010f000)
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_OFFS                                                     (TLMM_CSR_REG_BASE_OFFS + 0x0010f000)
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_RMSK                                                           0x3f
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_I2C_PAD_CTL_ADDR, HWIO_TLMM_GPIO_I2C_PAD_CTL_RMSK)
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_I2C_PAD_CTL_ADDR, m)
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_I2C_PAD_CTL_ADDR,v)
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_I2C_PAD_CTL_ADDR,m,v,HWIO_TLMM_GPIO_I2C_PAD_CTL_IN)
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_HS_I2C_MODE_BMSK                                               0x30
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_HS_I2C_MODE_SHFT                                                0x4
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_I2C_A_BLSP3_DATA_SR_CTL_EN_BMSK                                 0xc
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_I2C_A_BLSP3_DATA_SR_CTL_EN_SHFT                                 0x2
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_I2C_A_BLSP3_CLK_SR_CTL_EN_BMSK                                  0x3
#define HWIO_TLMM_GPIO_I2C_PAD_CTL_I2C_A_BLSP3_CLK_SR_CTL_EN_SHFT                                  0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n)                                        (TLMM_CSR_REG_BASE      + 0x00105000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OFFS(n)                                        (TLMM_CSR_REG_BASE_OFFS + 0x00105000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK                                                0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_MAXn                                                    9
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_BMSK                                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_SHFT                                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_BMSK                                        0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_SHFT                                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n)                                        (TLMM_CSR_REG_BASE      + 0x00104000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OFFS(n)                                        (TLMM_CSR_REG_BASE_OFFS + 0x00104000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK                                                0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_MAXn                                                    5
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_BMSK                                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_SHFT                                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_BMSK                                        0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_SHFT                                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n)                                            (TLMM_CSR_REG_BASE      + 0x00103000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OFFS(n)                                            (TLMM_CSR_REG_BASE_OFFS + 0x00103000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK                                                    0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_MAXn                                                        0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_BMSK                                           0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_SHFT                                             0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_BMSK                                            0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_SHFT                                             0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n)                                           (TLMM_CSR_REG_BASE      + 0x00102000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_OFFS(n)                                           (TLMM_CSR_REG_BASE_OFFS + 0x00102000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_RMSK                                                   0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_MAXn                                                       7
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_POLARITY_BMSK                                          0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_POLARITY_SHFT                                            0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_GPIO_SEL_BMSK                                           0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_GPIO_SEL_SHFT                                            0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n)                                            (TLMM_CSR_REG_BASE      + 0x00101000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OFFS(n)                                            (TLMM_CSR_REG_BASE_OFFS + 0x00101000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK                                                    0x17f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_MAXn                                                        1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_BMSK                                           0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_SHFT                                             0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_BMSK                                            0x7f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_SHFT                                             0x0

#define HWIO_TLMM_GPIO_OUT_0_ADDR                                                           (TLMM_CSR_REG_BASE      + 0x00200000)
#define HWIO_TLMM_GPIO_OUT_0_OFFS                                                           (TLMM_CSR_REG_BASE_OFFS + 0x00200000)
#define HWIO_TLMM_GPIO_OUT_0_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, HWIO_TLMM_GPIO_OUT_0_RMSK)
#define HWIO_TLMM_GPIO_OUT_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_0_ADDR,m,v,HWIO_TLMM_GPIO_OUT_0_IN)
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_1_ADDR                                                           (TLMM_CSR_REG_BASE      + 0x00200004)
#define HWIO_TLMM_GPIO_OUT_1_OFFS                                                           (TLMM_CSR_REG_BASE_OFFS + 0x00200004)
#define HWIO_TLMM_GPIO_OUT_1_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, HWIO_TLMM_GPIO_OUT_1_RMSK)
#define HWIO_TLMM_GPIO_OUT_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_1_ADDR,m,v,HWIO_TLMM_GPIO_OUT_1_IN)
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_2_ADDR                                                           (TLMM_CSR_REG_BASE      + 0x00200008)
#define HWIO_TLMM_GPIO_OUT_2_OFFS                                                           (TLMM_CSR_REG_BASE_OFFS + 0x00200008)
#define HWIO_TLMM_GPIO_OUT_2_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR, HWIO_TLMM_GPIO_OUT_2_RMSK)
#define HWIO_TLMM_GPIO_OUT_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_2_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_2_ADDR,m,v,HWIO_TLMM_GPIO_OUT_2_IN)
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_2_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_3_ADDR                                                           (TLMM_CSR_REG_BASE      + 0x0020000c)
#define HWIO_TLMM_GPIO_OUT_3_OFFS                                                           (TLMM_CSR_REG_BASE_OFFS + 0x0020000c)
#define HWIO_TLMM_GPIO_OUT_3_RMSK                                                                  0xf
#define HWIO_TLMM_GPIO_OUT_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_3_ADDR, HWIO_TLMM_GPIO_OUT_3_RMSK)
#define HWIO_TLMM_GPIO_OUT_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_3_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_3_ADDR,m,v,HWIO_TLMM_GPIO_OUT_3_IN)
#define HWIO_TLMM_GPIO_OUT_3_GPIO_OUT_BMSK                                                         0xf
#define HWIO_TLMM_GPIO_OUT_3_GPIO_OUT_SHFT                                                         0x0

#define HWIO_TLMM_GPIO_OUT_CLR_0_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200020)
#define HWIO_TLMM_GPIO_OUT_CLR_0_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200020)
#define HWIO_TLMM_GPIO_OUT_CLR_0_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_CLR_1_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200024)
#define HWIO_TLMM_GPIO_OUT_CLR_1_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200024)
#define HWIO_TLMM_GPIO_OUT_CLR_1_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_CLR_2_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200028)
#define HWIO_TLMM_GPIO_OUT_CLR_2_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200028)
#define HWIO_TLMM_GPIO_OUT_CLR_2_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_2_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_CLR_3_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x0020002c)
#define HWIO_TLMM_GPIO_OUT_CLR_3_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x0020002c)
#define HWIO_TLMM_GPIO_OUT_CLR_3_RMSK                                                              0xf
#define HWIO_TLMM_GPIO_OUT_CLR_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_3_GPIO_OUT_CLR_BMSK                                                 0xf
#define HWIO_TLMM_GPIO_OUT_CLR_3_GPIO_OUT_CLR_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_0_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200040)
#define HWIO_TLMM_GPIO_OUT_SET_0_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200040)
#define HWIO_TLMM_GPIO_OUT_SET_0_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_1_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200044)
#define HWIO_TLMM_GPIO_OUT_SET_1_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200044)
#define HWIO_TLMM_GPIO_OUT_SET_1_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_2_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x00200048)
#define HWIO_TLMM_GPIO_OUT_SET_2_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x00200048)
#define HWIO_TLMM_GPIO_OUT_SET_2_RMSK                                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_2_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_BMSK                                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_2_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_3_ADDR                                                       (TLMM_CSR_REG_BASE      + 0x0020004c)
#define HWIO_TLMM_GPIO_OUT_SET_3_OFFS                                                       (TLMM_CSR_REG_BASE_OFFS + 0x0020004c)
#define HWIO_TLMM_GPIO_OUT_SET_3_RMSK                                                              0xf
#define HWIO_TLMM_GPIO_OUT_SET_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_3_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_3_GPIO_OUT_SET_BMSK                                                 0xf
#define HWIO_TLMM_GPIO_OUT_SET_3_GPIO_OUT_SET_SHFT                                                 0x0

#define HWIO_TLMM_GPIO_IN_0_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200060)
#define HWIO_TLMM_GPIO_IN_0_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200060)
#define HWIO_TLMM_GPIO_IN_0_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_IN_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, HWIO_TLMM_GPIO_IN_0_RMSK)
#define HWIO_TLMM_GPIO_IN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, m)
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_IN_1_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200064)
#define HWIO_TLMM_GPIO_IN_1_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200064)
#define HWIO_TLMM_GPIO_IN_1_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_IN_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, HWIO_TLMM_GPIO_IN_1_RMSK)
#define HWIO_TLMM_GPIO_IN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, m)
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_IN_2_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200068)
#define HWIO_TLMM_GPIO_IN_2_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200068)
#define HWIO_TLMM_GPIO_IN_2_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_IN_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR, HWIO_TLMM_GPIO_IN_2_RMSK)
#define HWIO_TLMM_GPIO_IN_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_2_ADDR, m)
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_IN_2_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_IN_3_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x0020006c)
#define HWIO_TLMM_GPIO_IN_3_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x0020006c)
#define HWIO_TLMM_GPIO_IN_3_RMSK                                                                   0xf
#define HWIO_TLMM_GPIO_IN_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_IN_3_ADDR, HWIO_TLMM_GPIO_IN_3_RMSK)
#define HWIO_TLMM_GPIO_IN_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_3_ADDR, m)
#define HWIO_TLMM_GPIO_IN_3_GPIO_IN_BMSK                                                           0xf
#define HWIO_TLMM_GPIO_IN_3_GPIO_IN_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_0_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200080)
#define HWIO_TLMM_GPIO_OE_0_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200080)
#define HWIO_TLMM_GPIO_OE_0_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, HWIO_TLMM_GPIO_OE_0_RMSK)
#define HWIO_TLMM_GPIO_OE_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, m)
#define HWIO_TLMM_GPIO_OE_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_0_ADDR,m,v,HWIO_TLMM_GPIO_OE_0_IN)
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_1_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200084)
#define HWIO_TLMM_GPIO_OE_1_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200084)
#define HWIO_TLMM_GPIO_OE_1_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_1_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, HWIO_TLMM_GPIO_OE_1_RMSK)
#define HWIO_TLMM_GPIO_OE_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, m)
#define HWIO_TLMM_GPIO_OE_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_1_ADDR,m,v,HWIO_TLMM_GPIO_OE_1_IN)
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_2_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200088)
#define HWIO_TLMM_GPIO_OE_2_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x00200088)
#define HWIO_TLMM_GPIO_OE_2_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_2_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR, HWIO_TLMM_GPIO_OE_2_RMSK)
#define HWIO_TLMM_GPIO_OE_2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_2_ADDR, m)
#define HWIO_TLMM_GPIO_OE_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_2_ADDR,m,v,HWIO_TLMM_GPIO_OE_2_IN)
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_2_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_3_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x0020008c)
#define HWIO_TLMM_GPIO_OE_3_OFFS                                                            (TLMM_CSR_REG_BASE_OFFS + 0x0020008c)
#define HWIO_TLMM_GPIO_OE_3_RMSK                                                                   0xf
#define HWIO_TLMM_GPIO_OE_3_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_3_ADDR, HWIO_TLMM_GPIO_OE_3_RMSK)
#define HWIO_TLMM_GPIO_OE_3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_3_ADDR, m)
#define HWIO_TLMM_GPIO_OE_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_3_ADDR,m,v,HWIO_TLMM_GPIO_OE_3_IN)
#define HWIO_TLMM_GPIO_OE_3_GPIO_OE_BMSK                                                           0xf
#define HWIO_TLMM_GPIO_OE_3_GPIO_OE_SHFT                                                           0x0

#define HWIO_TLMM_GPIO_OE_CLR_0_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000a0)
#define HWIO_TLMM_GPIO_OE_CLR_0_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000a0)
#define HWIO_TLMM_GPIO_OE_CLR_0_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_CLR_1_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000a4)
#define HWIO_TLMM_GPIO_OE_CLR_1_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000a4)
#define HWIO_TLMM_GPIO_OE_CLR_1_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_CLR_2_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000a8)
#define HWIO_TLMM_GPIO_OE_CLR_2_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000a8)
#define HWIO_TLMM_GPIO_OE_CLR_2_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_2_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_CLR_3_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000ac)
#define HWIO_TLMM_GPIO_OE_CLR_3_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000ac)
#define HWIO_TLMM_GPIO_OE_CLR_3_RMSK                                                               0xf
#define HWIO_TLMM_GPIO_OE_CLR_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_3_GPIO_OE_CLR_BMSK                                                   0xf
#define HWIO_TLMM_GPIO_OE_CLR_3_GPIO_OE_CLR_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_0_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000c0)
#define HWIO_TLMM_GPIO_OE_SET_0_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000c0)
#define HWIO_TLMM_GPIO_OE_SET_0_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_1_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000c4)
#define HWIO_TLMM_GPIO_OE_SET_1_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000c4)
#define HWIO_TLMM_GPIO_OE_SET_1_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_2_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000c8)
#define HWIO_TLMM_GPIO_OE_SET_2_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000c8)
#define HWIO_TLMM_GPIO_OE_SET_2_RMSK                                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_2_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_BMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_2_GPIO_OE_SET_SHFT                                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_3_ADDR                                                        (TLMM_CSR_REG_BASE      + 0x002000cc)
#define HWIO_TLMM_GPIO_OE_SET_3_OFFS                                                        (TLMM_CSR_REG_BASE_OFFS + 0x002000cc)
#define HWIO_TLMM_GPIO_OE_SET_3_RMSK                                                               0xf
#define HWIO_TLMM_GPIO_OE_SET_3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_3_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_3_GPIO_OE_SET_BMSK                                                   0xf
#define HWIO_TLMM_GPIO_OE_SET_3_GPIO_OE_SET_SHFT                                                   0x0


#endif /* __RBCPR_QFPROM_H__ */
