#ifndef __SECURITYCONTROLV2HWIO_H__
#define __SECURITYCONTROLV2HWIO_H__
/*
===========================================================================
*/
/**
  @file SecurityControlV2HWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE

  'Include' filters applied: <none>
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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/cpr/common/target/9x45/SecurityControlV2HWIO.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#ifndef SECURITY_CONTROL_V2_BASE
#define SECURITY_CONTROL_V2_BASE (SECURITY_CONTROL_BASE + 0x248000)
#endif

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE_V2
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_V2_REG_BASE                                                      (SECURITY_CONTROL_V2_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS                                                 0x00000000

#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_ADDR(n)                                             (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000000 + 0x4 * (n))
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_OFFS(n)                                             (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000000 + 0x4 * (n))
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_MAXn                                                        71
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_ADDR(n), HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_RMSK)
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_CRI_CM_PRIVATE_BMSK                                 0xffffffff
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_V2_CRI_CM_PRIVATE_SHFT                                        0x0

#define HWIO_QFPROM_RAW_JTAG_ID_V2_ADDR                                                        (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000120)
#define HWIO_QFPROM_RAW_JTAG_ID_V2_OFFS                                                        (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000120)
#define HWIO_QFPROM_RAW_JTAG_ID_V2_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_JTAG_ID_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_V2_ADDR, HWIO_QFPROM_RAW_JTAG_ID_V2_RMSK)
#define HWIO_QFPROM_RAW_JTAG_ID_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_V2_ADDR, m)
#define HWIO_QFPROM_RAW_JTAG_ID_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_JTAG_ID_V2_ADDR,v)
#define HWIO_QFPROM_RAW_JTAG_ID_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_JTAG_ID_V2_ADDR,m,v,HWIO_QFPROM_RAW_JTAG_ID_V2_IN)
#define HWIO_QFPROM_RAW_JTAG_ID_V2_IDDQ_CX_ON_BMSK                                             0xc0000000
#define HWIO_QFPROM_RAW_JTAG_ID_V2_IDDQ_CX_ON_SHFT                                                   0x1e
#define HWIO_QFPROM_RAW_JTAG_ID_V2_SPARE_BMSK                                                  0x20000000
#define HWIO_QFPROM_RAW_JTAG_ID_V2_SPARE_SHFT                                                        0x1d
#define HWIO_QFPROM_RAW_JTAG_ID_V2_MACCHIATO_EN_BMSK                                           0x10000000
#define HWIO_QFPROM_RAW_JTAG_ID_V2_MACCHIATO_EN_SHFT                                                 0x1c
#define HWIO_QFPROM_RAW_JTAG_ID_V2_FEATURE_ID_BMSK                                              0xff00000
#define HWIO_QFPROM_RAW_JTAG_ID_V2_FEATURE_ID_SHFT                                                   0x14
#define HWIO_QFPROM_RAW_JTAG_ID_V2_JTAG_ID_BMSK                                                   0xfffff
#define HWIO_QFPROM_RAW_JTAG_ID_V2_JTAG_ID_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_PTE1_V2_ADDR                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000124)
#define HWIO_QFPROM_RAW_PTE1_V2_OFFS                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000124)
#define HWIO_QFPROM_RAW_PTE1_V2_RMSK                                                           0xffffffff
#define HWIO_QFPROM_RAW_PTE1_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE1_V2_ADDR, HWIO_QFPROM_RAW_PTE1_V2_RMSK)
#define HWIO_QFPROM_RAW_PTE1_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE1_V2_ADDR, m)
#define HWIO_QFPROM_RAW_PTE1_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE1_V2_ADDR,v)
#define HWIO_QFPROM_RAW_PTE1_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE1_V2_ADDR,m,v,HWIO_QFPROM_RAW_PTE1_V2_IN)
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_BIN_BMSK                                                  0xe0000000
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_BIN_SHFT                                                        0x1d
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_MULTIPLIER_BMSK                                           0x1c000000
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_MULTIPLIER_SHFT                                                 0x1a
#define HWIO_QFPROM_RAW_PTE1_V2_PROCESS_NODE_ID_BMSK                                            0x2000000
#define HWIO_QFPROM_RAW_PTE1_V2_PROCESS_NODE_ID_SHFT                                                 0x19
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_MX_OFF_BMSK                                                0x1f80000
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_MX_OFF_SHFT                                                     0x13
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_CX_OFF_BMSK                                                  0x7f000
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_CX_OFF_SHFT                                                      0xc
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_MX_ON_BMSK                                                     0xfe0
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_MX_ON_SHFT                                                       0x5
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_CX_ON_BMSK                                                      0x1f
#define HWIO_QFPROM_RAW_PTE1_V2_IDDQ_CX_ON_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_SERIAL_NUM_V2_ADDR                                                     (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000128)
#define HWIO_QFPROM_RAW_SERIAL_NUM_V2_OFFS                                                     (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000128)
#define HWIO_QFPROM_RAW_SERIAL_NUM_V2_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_V2_ADDR, HWIO_QFPROM_RAW_SERIAL_NUM_V2_RMSK)
#define HWIO_QFPROM_RAW_SERIAL_NUM_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SERIAL_NUM_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SERIAL_NUM_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SERIAL_NUM_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SERIAL_NUM_V2_ADDR,m,v,HWIO_QFPROM_RAW_SERIAL_NUM_V2_IN)
#define HWIO_QFPROM_RAW_SERIAL_NUM_V2_SERIAL_NUM_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_V2_SERIAL_NUM_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_PTE2_V2_ADDR                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000012c)
#define HWIO_QFPROM_RAW_PTE2_V2_OFFS                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000012c)
#define HWIO_QFPROM_RAW_PTE2_V2_RMSK                                                           0xffffffff
#define HWIO_QFPROM_RAW_PTE2_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE2_V2_ADDR, HWIO_QFPROM_RAW_PTE2_V2_RMSK)
#define HWIO_QFPROM_RAW_PTE2_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE2_V2_ADDR, m)
#define HWIO_QFPROM_RAW_PTE2_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE2_V2_ADDR,v)
#define HWIO_QFPROM_RAW_PTE2_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE2_V2_ADDR,m,v,HWIO_QFPROM_RAW_PTE2_V2_IN)
#define HWIO_QFPROM_RAW_PTE2_V2_MX_RET_BIN_BMSK                                                0xe0000000
#define HWIO_QFPROM_RAW_PTE2_V2_MX_RET_BIN_SHFT                                                      0x1d
#define HWIO_QFPROM_RAW_PTE2_V2_IDDQ_SPARE_2_BMSK                                              0x10000000
#define HWIO_QFPROM_RAW_PTE2_V2_IDDQ_SPARE_2_SHFT                                                    0x1c
#define HWIO_QFPROM_RAW_PTE2_V2_IDDQ_RC_BMSK                                                    0xc000000
#define HWIO_QFPROM_RAW_PTE2_V2_IDDQ_RC_SHFT                                                         0x1a
#define HWIO_QFPROM_RAW_PTE2_V2_IDDQ_MSS_OFF_BMSK                                               0x3f80000
#define HWIO_QFPROM_RAW_PTE2_V2_IDDQ_MSS_OFF_SHFT                                                    0x13
#define HWIO_QFPROM_RAW_PTE2_V2_IDDQ_MSS_ON_BMSK                                                  0x7f800
#define HWIO_QFPROM_RAW_PTE2_V2_IDDQ_MSS_ON_SHFT                                                      0xb
#define HWIO_QFPROM_RAW_PTE2_V2_FOUNDRY_ID_BMSK                                                     0x700
#define HWIO_QFPROM_RAW_PTE2_V2_FOUNDRY_ID_SHFT                                                       0x8
#define HWIO_QFPROM_RAW_PTE2_V2_LOGIC_RETENTION_BMSK                                                 0xe0
#define HWIO_QFPROM_RAW_PTE2_V2_LOGIC_RETENTION_SHFT                                                  0x5
#define HWIO_QFPROM_RAW_PTE2_V2_SPEED_BIN_BMSK                                                       0x1c
#define HWIO_QFPROM_RAW_PTE2_V2_SPEED_BIN_SHFT                                                        0x2
#define HWIO_QFPROM_RAW_PTE2_V2_IDDQ_SPARE_BMSK                                                       0x3
#define HWIO_QFPROM_RAW_PTE2_V2_IDDQ_SPARE_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000130)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000130)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_RSVD0_BMSK                                           0xf8000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_RSVD0_SHFT                                                 0x1b
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE26_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE26_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE25_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE25_SHFT                                               0x19
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE24_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE24_SHFT                                               0x18
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE23_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE23_SHFT                                               0x17
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE22_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE22_SHFT                                               0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE21_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE21_SHFT                                               0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE20_BMSK                                           0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE20_SHFT                                               0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE19_BMSK                                            0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE19_SHFT                                               0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE18_BMSK                                            0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SPARE18_SHFT                                               0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_FEC_EN_BMSK                                             0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_FEC_EN_SHFT                                                0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_BOOT_ROM_PATCH_BMSK                                     0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_BOOT_ROM_PATCH_SHFT                                        0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_MEM_CONFIG_BMSK                                          0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_MEM_CONFIG_SHFT                                             0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_CALIB_BMSK                                               0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_CALIB_SHFT                                                  0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_PK_HASH_BMSK                                             0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_PK_HASH_SHFT                                                0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_CALIB2_BMSK                                              0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_CALIB2_SHFT                                                 0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_OEM_SEC_BOOT_BMSK                                         0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_OEM_SEC_BOOT_SHFT                                           0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SEC_KEY_DERIVATION_KEY_BMSK                               0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_SEC_KEY_DERIVATION_KEY_SHFT                                 0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_PRI_KEY_DERIVATION_KEY_BMSK                               0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_PRI_KEY_DERIVATION_KEY_SHFT                                 0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_CM_FEAT_CONFIG_BMSK                                       0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_CM_FEAT_CONFIG_SHFT                                         0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_FEAT_CONFIG_BMSK                                           0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_FEAT_CONFIG_SHFT                                            0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_OEM_CONFIG_BMSK                                            0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_OEM_CONFIG_SHFT                                             0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_3_BMSK                                       0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_3_SHFT                                        0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_2_BMSK                                       0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_2_SHFT                                        0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_1_BMSK                                        0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_1_SHFT                                        0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_RD_WR_PERM_BMSK                                             0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_RD_WR_PERM_SHFT                                             0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_PTE_BMSK                                                    0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_PTE_SHFT                                                    0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_CRI_CM_PRIVATE_BMSK                                         0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_V2_CRI_CM_PRIVATE_SHFT                                         0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000134)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000134)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_RSVD0_BMSK                                           0xf8000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_RSVD0_SHFT                                                 0x1b
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE26_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE26_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE25_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE25_SHFT                                               0x19
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE24_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE24_SHFT                                               0x18
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE23_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE23_SHFT                                               0x17
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE22_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE22_SHFT                                               0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE21_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE21_SHFT                                               0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE20_BMSK                                           0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE20_SHFT                                               0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE19_BMSK                                            0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE19_SHFT                                               0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE18_BMSK                                            0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SPARE18_SHFT                                               0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_FEC_EN_BMSK                                             0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_FEC_EN_SHFT                                                0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_BOOT_ROM_PATCH_BMSK                                     0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_BOOT_ROM_PATCH_SHFT                                        0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_MEM_CONFIG_BMSK                                          0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_MEM_CONFIG_SHFT                                             0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_CALIB_BMSK                                               0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_CALIB_SHFT                                                  0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_PK_HASH_BMSK                                             0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_PK_HASH_SHFT                                                0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_CALIB2_BMSK                                              0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_CALIB2_SHFT                                                 0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_OEM_SEC_BOOT_BMSK                                         0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_OEM_SEC_BOOT_SHFT                                           0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SEC_KEY_DERIVATION_KEY_BMSK                               0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_SEC_KEY_DERIVATION_KEY_SHFT                                 0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_PRI_KEY_DERIVATION_KEY_BMSK                               0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_PRI_KEY_DERIVATION_KEY_SHFT                                 0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_CM_FEAT_CONFIG_BMSK                                       0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_CM_FEAT_CONFIG_SHFT                                         0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_FEAT_CONFIG_BMSK                                           0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_FEAT_CONFIG_SHFT                                            0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_OEM_CONFIG_BMSK                                            0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_OEM_CONFIG_SHFT                                             0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_3_BMSK                                       0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_3_SHFT                                        0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_2_BMSK                                       0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_2_SHFT                                        0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_1_BMSK                                        0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_1_SHFT                                        0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_RD_WR_PERM_BMSK                                             0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_RD_WR_PERM_SHFT                                             0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_PTE_BMSK                                                    0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_PTE_SHFT                                                    0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_CRI_CM_PRIVATE_BMSK                                         0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_V2_CRI_CM_PRIVATE_SHFT                                         0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_ADDR                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000138)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_OFFS                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000138)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_PIL_SUBSYSTEM0_BMSK                             0xfc000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_PIL_SUBSYSTEM0_SHFT                                   0x1a
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_TZ_BMSK                                          0x3fff000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_TZ_SHFT                                                0xc
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_SBL1_BMSK                                            0xffe
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_SBL1_SHFT                                              0x1
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_RPMB_KEY_PROVISIONED_BMSK                              0x1
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_V2_RPMB_KEY_PROVISIONED_SHFT                              0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_ADDR                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000013c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_OFFS                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000013c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_APPSBL0_BMSK                                    0xfffc0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_APPSBL0_SHFT                                          0x12
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_PIL_SUBSYSTEM1_BMSK                                0x3ffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_V2_PIL_SUBSYSTEM1_SHFT                                    0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_ADDR                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000140)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_OFFS                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000140)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_APPSBL1_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_V2_APPSBL1_SHFT                                           0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_ADDR                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000144)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_OFFS                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000144)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_ROOT_CERT_PK_HASH_INDEX_BMSK                    0xff000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_ROOT_CERT_PK_HASH_INDEX_SHFT                          0x18
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_HYPERVISOR_BMSK                                   0xfff000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_HYPERVISOR_SHFT                                        0xc
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_RPM_BMSK                                             0xff0
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_RPM_SHFT                                               0x4
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_APPSBL2_BMSK                                           0xf
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_V2_APPSBL2_SHFT                                           0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_ADDR                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000148)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_OFFS                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000148)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_MSS_BMSK                                        0xffff0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_MSS_SHFT                                              0x10
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_MBA_BMSK                                            0xffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_V2_MBA_SHFT                                               0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_ADDR                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000014c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_OFFS                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000014c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_SPARE0_BMSK                                     0xffffff00
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_SPARE0_SHFT                                            0x8
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                    0xff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_V2_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                     0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ADDR                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000150)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_OFFS                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000150)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_DISABLE_CRASH_DUMP_OU_FIELD_BMSK                0x80000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_DISABLE_CRASH_DUMP_OU_FIELD_SHFT                      0x1f
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_DEBUG_POLICY_DISABLE_BMSK                       0x40000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_DEBUG_POLICY_DISABLE_SHFT                             0x1e
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_RSVD0_BMSK                                      0x30000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_RSVD0_SHFT                                            0x1c
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG26_SECURE_BMSK                          0x8000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG26_SECURE_SHFT                               0x1b
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG25_SECURE_BMSK                          0x4000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG25_SECURE_SHFT                               0x1a
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG24_SECURE_BMSK                          0x2000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG24_SECURE_SHFT                               0x19
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG23_SECURE_BMSK                          0x1000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG23_SECURE_SHFT                               0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG22_SECURE_BMSK                           0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG22_SECURE_SHFT                               0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_PBL_LOG_DISABLE_BMSK                              0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_PBL_LOG_DISABLE_SHFT                                  0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ROOT_CERT_TOTAL_NUM_BMSK                          0x3c0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ROOT_CERT_TOTAL_NUM_SHFT                              0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG21_SECURE_BMSK                            0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG21_SECURE_SHFT                               0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG20_SECURE_BMSK                            0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG20_SECURE_SHFT                               0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG19_SECURE_BMSK                             0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG19_SECURE_SHFT                                0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG18_SECURE_BMSK                             0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG18_SECURE_SHFT                                0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_RSVD1_BMSK                                          0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_RSVD1_SHFT                                             0xd
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_WDOG_EN_BMSK                                        0x1000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_WDOG_EN_SHFT                                           0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPDM_SECURE_MODE_BMSK                                0x800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPDM_SECURE_MODE_SHFT                                  0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ALT_SD_PORT_FOR_BOOT_BMSK                            0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ALT_SD_PORT_FOR_BOOT_SHFT                              0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                   0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                     0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SDC_EMMC_MODE1P2_EN_BMSK                             0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SDC_EMMC_MODE1P2_EN_SHFT                               0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_FAST_BOOT_BMSK                                        0xe0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_FAST_BOOT_SHFT                                         0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SDCC_MCLK_BOOT_FREQ_BMSK                              0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SDCC_MCLK_BOOT_FREQ_SHFT                               0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_FORCE_DLOAD_DISABLE_BMSK                               0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_FORCE_DLOAD_DISABLE_SHFT                               0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_BMSK                                             0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_SPARE_SHFT                                             0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ENUM_TIMEOUT_BMSK                                      0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_ENUM_TIMEOUT_SHFT                                      0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_E_DLOAD_DISABLE_BMSK                                   0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_V2_E_DLOAD_DISABLE_SHFT                                   0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_ADDR                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000154)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_OFFS                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000154)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_SPARE1_BMSK                                     0xff800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_SPARE1_SHFT                                           0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_ANTI_ROLLBACK_FEATURE_EN_BMSK                     0x780000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_ANTI_ROLLBACK_FEATURE_EN_SHFT                         0x13
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_DAP_DEVICEEN_DISABLE_BMSK                          0x40000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_DAP_DEVICEEN_DISABLE_SHFT                             0x12
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_DAP_SPNIDEN_DISABLE_BMSK                           0x20000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_DAP_SPNIDEN_DISABLE_SHFT                              0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_DAP_SPIDEN_DISABLE_BMSK                            0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_DAP_SPIDEN_DISABLE_SHFT                               0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_DAP_NIDEN_DISABLE_BMSK                              0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_DAP_NIDEN_DISABLE_SHFT                                 0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_DAP_DBGEN_DISABLE_BMSK                              0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_DAP_DBGEN_DISABLE_SHFT                                 0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_APPS_SPNIDEN_DISABLE_BMSK                           0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_APPS_SPNIDEN_DISABLE_SHFT                              0xd
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_APPS_SPIDEN_DISABLE_BMSK                            0x1000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_APPS_SPIDEN_DISABLE_SHFT                               0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_APPS_NIDEN_DISABLE_BMSK                              0x800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_APPS_NIDEN_DISABLE_SHFT                                0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_APPS_DBGEN_DISABLE_BMSK                              0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_APPS_DBGEN_DISABLE_SHFT                                0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_SPARE1_DISABLE_BMSK                                  0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_SPARE1_DISABLE_SHFT                                    0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_SPARE0_DISABLE_BMSK                                  0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_SPARE0_DISABLE_SHFT                                    0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_SPARE2_DISABLE_BMSK                                   0x80
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_SPARE2_DISABLE_SHFT                                    0x7
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_RPM_DAPEN_DISABLE_BMSK                                0x40
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_RPM_DAPEN_DISABLE_SHFT                                 0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_RPM_NIDEN_DISABLE_BMSK                                0x20
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_RPM_NIDEN_DISABLE_SHFT                                 0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_RPM_DBGEN_DISABLE_BMSK                                0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_RPM_DBGEN_DISABLE_SHFT                                 0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_SPARE3_DISABLE_BMSK                                    0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_SPARE3_DISABLE_SHFT                                    0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_MSS_NIDEN_DISABLE_BMSK                                 0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_MSS_NIDEN_DISABLE_SHFT                                 0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_MSS_DBGEN_DISABLE_BMSK                                 0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_MSS_DBGEN_DISABLE_SHFT                                 0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_ALL_DEBUG_DISABLE_BMSK                                 0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_V2_ALL_DEBUG_DISABLE_SHFT                                 0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_ADDR                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000158)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_OFFS                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000158)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_OEM_PRODUCT_ID_BMSK                             0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_OEM_PRODUCT_ID_SHFT                                   0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_OEM_HW_ID_BMSK                                      0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_V2_OEM_HW_ID_SHFT                                         0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_ADDR                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000015c)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_OFFS                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000015c)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_PERIPH_VID_BMSK                                 0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_PERIPH_VID_SHFT                                       0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_PERIPH_PID_BMSK                                     0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_V2_PERIPH_PID_SHFT                                        0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000160)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000160)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_VU_EN_BMSK                          0xfc000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_VU_EN_SHFT                                0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_NAV_EN_BMSK                                0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_NAV_EN_SHFT                                     0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_LDO_EN_BMSK                                0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_LDO_EN_SHFT                                     0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_ECS_EN_BMSK                                 0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_ECS_EN_SHFT                                     0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_WCDMA_EN_BMSK                         0x600000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_WCDMA_EN_SHFT                             0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_TDSCDMA_EN_BMSK                       0x180000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_TDSCDMA_EN_SHFT                           0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_EN_BMSK                            0x60000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_EN_SHFT                               0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_ABOVE_CAT2_EN_BMSK                 0x18000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_ABOVE_CAT2_EN_SHFT                     0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_ABOVE_CAT1_EN_BMSK                  0x6000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_ABOVE_CAT1_EN_SHFT                     0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_MIMO_EN_BMSK                       0x1800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_MIMO_EN_SHFT                          0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_EN_BMSK                             0x600
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_EN_SHFT                               0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_DC_EN_BMSK                          0x180
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_DC_EN_SHFT                            0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_GENRAN_EN_BMSK                            0x60
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_GENRAN_EN_SHFT                             0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_EN_BMSK                                   0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_EN_SHFT                                    0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_DO_EN_BMSK                                 0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_DO_EN_SHFT                                 0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_1X_EN_BMSK                                 0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_1X_EN_SHFT                                 0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000164)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000164)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK               0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                     0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_CM_FEAT_CONFIG_DISABLE_BMSK                    0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_CM_FEAT_CONFIG_DISABLE_SHFT                          0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_SPARE_BMSK                                     0x3ff00000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_SPARE_SHFT                                           0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_MSMC_MDSP_FW_EN_BMSK                              0xfff00
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_MSMC_MDSP_FW_EN_SHFT                                  0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_MSMC_SPARE_BMSK                                      0xc0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_MSMC_SPARE_SHFT                                       0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_MSMC_MODEM_VU_EN_BMSK                                0x3f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_V2_MSMC_MODEM_VU_EN_SHFT                                 0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000168)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000168)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_SPARE1_BMSK                                    0xe0000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_SPARE1_SHFT                                          0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_SECURE_CHANNEL_DISABLE_BMSK                    0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_SECURE_CHANNEL_DISABLE_SHFT                          0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_FUSE_SMT_PERM_ENABLE_BMSK                       0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_FUSE_SMT_PERM_ENABLE_SHFT                            0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_FUSE_RCP_BYPASS_ENABLE_BMSK                     0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_FUSE_RCP_BYPASS_ENABLE_SHFT                          0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_MSMC_SMMU_BYPASS_DISABLE_BMSK                   0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_MSMC_SMMU_BYPASS_DISABLE_SHFT                        0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_MSMC_GPIO_SLIMBUS_PD_DISABLE_BMSK               0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_MSMC_GPIO_SLIMBUS_PD_DISABLE_SHFT                    0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_FUSE_PCIE20_RC_DISABLE_BMSK                      0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_FUSE_PCIE20_RC_DISABLE_SHFT                          0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_MSMC_PCIE_DISABLE_BMSK                           0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_MSMC_PCIE_DISABLE_SHFT                               0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_NDINT_DISABLE_BMSK                               0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_NDINT_DISABLE_SHFT                                   0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_DEVICEEN_DISABLE_BMSK                     0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_DEVICEEN_DISABLE_SHFT                         0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_SPNIDEN_DISABLE_BMSK                       0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_SPNIDEN_DISABLE_SHFT                          0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_SPIDEN_DISABLE_BMSK                        0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_SPIDEN_DISABLE_SHFT                           0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_NIDEN_DISABLE_BMSK                         0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_NIDEN_DISABLE_SHFT                            0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_DBGEN_DISABLE_BMSK                         0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_DBGEN_DISABLE_SHFT                            0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_SPNIDEN_DISABLE_BMSK                       0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_SPNIDEN_DISABLE_SHFT                          0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_SPIDEN_DISABLE_BMSK                        0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_SPIDEN_DISABLE_SHFT                           0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_NIDEN_DISABLE_BMSK                         0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_NIDEN_DISABLE_SHFT                            0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_DBGEN_DISABLE_BMSK                         0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_DBGEN_DISABLE_SHFT                            0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE1_DISABLE_BMSK                              0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE1_DISABLE_SHFT                                0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE0_DISABLE_BMSK                              0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE0_DISABLE_SHFT                                0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE2_DISABLE_BMSK                              0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE2_DISABLE_SHFT                                0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_DAPEN_DISABLE_BMSK                           0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_DAPEN_DISABLE_SHFT                             0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_NIDEN_DISABLE_BMSK                            0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_NIDEN_DISABLE_SHFT                             0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_DBGEN_DISABLE_BMSK                            0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_DBGEN_DISABLE_SHFT                             0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE3_DISABLE_BMSK                               0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE3_DISABLE_SHFT                                0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_MSS_NIDEN_DISABLE_BMSK                            0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_MSS_NIDEN_DISABLE_SHFT                             0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_MSS_DBGEN_DISABLE_BMSK                             0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QC_MSS_DBGEN_DISABLE_SHFT                             0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QDI_SPMI_DISABLE_BMSK                                 0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_QDI_SPMI_DISABLE_SHFT                                 0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_SM_BIST_DISABLE_BMSK                                  0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_SM_BIST_DISABLE_SHFT                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_TIC_DISABLE_BMSK                                      0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_V2_TIC_DISABLE_SHFT                                      0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000016c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000016c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_SEC_TAP_ACCESS_DISABLE_BMSK                    0xfe000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_SEC_TAP_ACCESS_DISABLE_SHFT                          0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_TAP_CJI_CORE_SEL_DISABLE_BMSK                   0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_TAP_CJI_CORE_SEL_DISABLE_SHFT                        0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_TAP_INSTR_DISABLE_BMSK                           0xfff800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_TAP_INSTR_DISABLE_SHFT                                0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_SPARE1_BMSK                                         0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_SPARE1_SHFT                                           0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_MODEM_PBL_PATCH_VERSION_BMSK                        0x3e0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_MODEM_PBL_PATCH_VERSION_SHFT                          0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_APPS_PBL_PATCH_VERSION_BMSK                          0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_V2_APPS_PBL_PATCH_VERSION_SHFT                           0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000170)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000170)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK          0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_V2_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                 0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000174)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000174)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_STACKED_MEMORY_ID_BMSK                         0xf0000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_STACKED_MEMORY_ID_SHFT                               0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APSS_BOOT_TRIGGER_DISABLE_BMSK                  0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APSS_BOOT_TRIGGER_DISABLE_SHFT                       0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_MODEM_PBL_PLL_CTRL_BMSK                         0x7800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_MODEM_PBL_PLL_CTRL_SHFT                              0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APPS_PBL_PLL_CTRL_BMSK                           0x780000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APPS_PBL_PLL_CTRL_SHFT                               0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FSM_CFG_BMSK                            0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FSM_CFG_SHFT                               0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FSM_DELAY_BMSK                          0x30000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FSM_DELAY_SHFT                             0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_BMSK            0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_SHFT               0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_USB_SS_ENABLE_BMSK                                 0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_USB_SS_ENABLE_SHFT                                    0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APPS_PBL_BOOT_SPEED_BMSK                           0x3000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APPS_PBL_BOOT_SPEED_SHFT                              0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FROM_ROM_BMSK                             0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FROM_ROM_SHFT                               0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_MSA_ENA_BMSK                                        0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_MSA_ENA_SHFT                                          0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_FORCE_MSA_AUTH_EN_BMSK                              0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_FORCE_MSA_AUTH_EN_SHFT                                0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_PCIE_REFCLK_TERM_ENABLE_BMSK                        0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_PCIE_REFCLK_TERM_ENABLE_SHFT                          0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_MSS_HASH_INTEGRITY_CHECK_DISABLE_BMSK                0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_MSS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                 0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_MODEM_BOOT_FROM_ROM_BMSK                             0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_MODEM_BOOT_FROM_ROM_SHFT                              0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_BOOT_ROM_PATCH_DISABLE_BMSK                          0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_BOOT_ROM_PATCH_DISABLE_SHFT                           0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK               0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_V2_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_ADDR                                        (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000178)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_OFFS                                        (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000178)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_RMSK                                        0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_RSVD0_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_V2_RSVD0_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_ADDR                                        (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000017c)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_OFFS                                        (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000017c)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_RMSK                                        0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_RSVD0_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_V2_RSVD0_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_ADDR                                        (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000180)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_OFFS                                        (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000180)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_RMSK                                        0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_RSVD0_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_V2_RSVD0_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_ADDR                                        (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000184)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_OFFS                                        (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000184)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_RMSK                                        0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_RSVD0_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_V2_RSVD0_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_ADDR                                        (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000188)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_OFFS                                        (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000188)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_RMSK                                        0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_RSVD0_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_V2_RSVD0_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_ADDR                                        (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000018c)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_OFFS                                        (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000018c)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_RMSK                                        0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_RSVD0_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_V2_RSVD0_SHFT                                         0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n)                             (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000190 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_OFFS(n)                             (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000190 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_RMSK                                0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_MAXn                                         3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_KEY_DATA0_BMSK                      0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_KEY_DATA0_SHFT                             0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n)                             (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000194 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_OFFS(n)                             (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000194 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_RMSK                                0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_MAXn                                         3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_KEY_DATA1_BMSK                      0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_KEY_DATA1_SHFT                             0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n)                             (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000001b0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_OFFS(n)                             (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000001b0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_RMSK                                0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_MAXn                                         3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_KEY_DATA0_BMSK                      0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_KEY_DATA0_SHFT                             0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n)                             (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000001b4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_OFFS(n)                             (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000001b4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_RMSK                                0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_MAXn                                         3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_KEY_DATA1_BMSK                      0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_KEY_DATA1_SHFT                             0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000001d0)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000001d0)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_ADDR, HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_IN)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT4_BMSK                                0xff000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT4_SHFT                                      0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT3_BMSK                                  0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT3_SHFT                                      0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT2_BMSK                                    0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT2_SHFT                                       0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT1_BMSK                                      0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT1_SHFT                                       0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000001d4)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000001d4)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_ADDR, HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_IN)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_SPARE0_BMSK                                   0x80000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_SPARE0_SHFT                                         0x1f
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_FEC_VALUE_BMSK                                0x7f000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_FEC_VALUE_SHFT                                      0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT7_BMSK                                  0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT7_SHFT                                      0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT6_BMSK                                    0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT6_SHFT                                       0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT5_BMSK                                      0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT5_SHFT                                       0x0

#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000001d8)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000001d8)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_CALIB2_ROW0_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000001dc)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000001dc)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_CALIB2_ROW0_MSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_ADDR(n)                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000001e0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_OFFS(n)                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000001e0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_MAXn                                                        3
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_ADDR(n), HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_ADDR(n),val)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_ADDR(n),mask,val,HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_INI(n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_HASH_DATA0_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_LSB_V2_HASH_DATA0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_ADDR(n)                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000001e4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_OFFS(n)                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000001e4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_MAXn                                                        3
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_ADDR(n), HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_ADDR(n),val)
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_ADDR(n),mask,val,HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_INI(n))
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_HASH_DATA1_BMSK                                      0xffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROWn_MSB_V2_HASH_DATA1_SHFT                                           0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000200)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000200)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_ADDR, HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_IN)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_HASH_DATA0_BMSK                                    0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_LSB_V2_HASH_DATA0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000204)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000204)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_ADDR, HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_IN)
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_UNUSED_BMSK                                          0xffffff
#define HWIO_QFPROM_RAW_PK_HASH_ROW4_MSB_V2_UNUSED_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000208)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000208)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_TSENS2_POINT1_BMSK                                   0xf0000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_TSENS2_POINT1_SHFT                                         0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_TSENS1_POINT1_BMSK                                    0xf000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_TSENS1_POINT1_SHFT                                         0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_TSENS0_POINT1_BMSK                                     0xf00000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_TSENS0_POINT1_SHFT                                         0x14
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_TSENS_BASE1_BMSK                                        0xffc00
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_TSENS_BASE1_SHFT                                            0xa
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_TSENS_BASE0_BMSK                                          0x3ff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_V2_TSENS_BASE0_SHFT                                            0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000020c)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000020c)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_FEC_BMSK                                             0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_FEC_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_USB_PHY_TUNING_BMSK                                    0xf00000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_USB_PHY_TUNING_SHFT                                        0x14
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_SPARE_BMSK                                              0xf0000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_SPARE_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_Q6SS0_LDO_VREF_TRIM_BMSK                                 0xf800
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_Q6SS0_LDO_VREF_TRIM_SHFT                                    0xb
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_TSENS_CAL_SEL_BMSK                                        0x700
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_TSENS_CAL_SEL_SHFT                                          0x8
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_TSENS4_POINT1_BMSK                                         0xf0
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_TSENS4_POINT1_SHFT                                          0x4
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_TSENS3_POINT1_BMSK                                          0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_V2_TSENS3_POINT1_SHFT                                          0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000210)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000210)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_3_BMSK                 0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_3_SHFT                       0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_SVS2_BMSK                             0x7c000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_SVS2_SHFT                                   0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_2_BMSK                  0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_2_SHFT                       0x19
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_SVS_BMSK                               0x1f00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_SVS_SHFT                                    0x14
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_1_BMSK                    0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_1_SHFT                       0x13
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_NOM_BMSK                                 0x7c000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_NOM_SHFT                                     0xe
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_0_BMSK                     0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_0_SHFT                        0xd
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_TUR_BMSK                                  0x1f00
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_TUR_SHFT                                     0x8
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_BMSK                            0x80
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_SHFT                             0x7
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR2_TARG_VOLT_OFFSET_NOM_BMSK                             0x78
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR2_TARG_VOLT_OFFSET_NOM_SHFT                              0x3
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR2_TARG_VOLT_OFFSET_TUR_BMSK                              0x7
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_V2_CPR2_TARG_VOLT_OFFSET_TUR_SHFT                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000214)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000214)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_FEC_BMSK                                             0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_FEC_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_3_BMSK                   0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_3_SHFT                       0x17
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_SVS2_BMSK                               0x7c0000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_SVS2_SHFT                                   0x12
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_2_BMSK                    0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_2_SHFT                       0x11
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_SVS_BMSK                                 0x1f000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_SVS_SHFT                                     0xc
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_1_BMSK                      0x800
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_1_SHFT                        0xb
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_NOM_BMSK                                   0x7c0
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_NOM_SHFT                                     0x6
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_0_BMSK                       0x20
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_0_SHFT                        0x5
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_TUR_BMSK                                    0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_TUR_SHFT                                     0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000218)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000218)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_LDO_TARGET1_BMSK                                     0xf0000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_LDO_TARGET1_SHFT                                           0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_LDO_TARGET0_BMSK                                      0xfc00000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_LDO_TARGET0_SHFT                                           0x16
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_LDO_ENABLE_BMSK                                        0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_LDO_ENABLE_SHFT                                            0x15
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS2_BMSK                        0x1e0000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS2_SHFT                            0x11
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS_BMSK                          0x1c000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS_SHFT                              0xe
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR2_TARG_VOLT_NOM_BMSK                                  0x3e00
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR2_TARG_VOLT_NOM_SHFT                                     0x9
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS_BIT_0_BMSK                      0x100
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS_BIT_0_SHFT                        0x8
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR2_TARG_VOLT_TUR_BMSK                                    0xf8
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR2_TARG_VOLT_TUR_SHFT                                     0x3
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR_REV_BMSK                                                0x7
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_V2_CPR_REV_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000021c)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000021c)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_FEC_BMSK                                             0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_FEC_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_BMSK                        0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_SHFT                            0x16
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS_BMSK                         0x3c0000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS_SHFT                             0x12
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_NOM_BMSK                          0x3c000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_NOM_SHFT                              0xe
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_TUR_BMSK                           0x3c00
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_TUR_SHFT                              0xa
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_SPARE_BMSK                                                0x300
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_SPARE_SHFT                                                  0x8
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_VCM_BMSK                                      0xc0
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_VCM_SHFT                                       0x6
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_LDO_BMSK                                      0x30
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_LDO_SHFT                                       0x4
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_VREF_BMSK                                      0xc
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_VREF_SHFT                                      0x2
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_LDO_ENABLE_SVS2_BMSK                                        0x2
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_LDO_ENABLE_SVS2_SHFT                                        0x1
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_LDO_TARGET1_BMSK                                            0x1
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_V2_LDO_TARGET1_SHFT                                            0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000220)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000220)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ECS_BHS_SLOPE_BMSK                                   0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ECS_BHS_SLOPE_SHFT                                         0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ECS_BHS_INTERCEPT_BMSK                               0x7fe00000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ECS_BHS_INTERCEPT_SHFT                                     0x15
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ECS_LDO_SLOPE_BMSK                                     0x1ff800
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ECS_LDO_SLOPE_SHFT                                          0xb
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ECS_LDO_INTERCEPT_BMSK                                    0x7fe
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ECS_LDO_INTERCEPT_SHFT                                      0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ECS_ENABLE_BMSK                                             0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_V2_ECS_ENABLE_SHFT                                             0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000224)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000224)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_FEC_BMSK                                             0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_FEC_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_G_B1_BMSK                                              0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_G_B1_SHFT                                                  0x15
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_SAR_LDO_ERR0_BMSK                                      0x180000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_SAR_LDO_ERR0_SHFT                                          0x13
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_CPR2_TARG_VOLT_OFFSET_TUR_BIT_3_BMSK                    0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_CPR2_TARG_VOLT_OFFSET_TUR_BIT_3_SHFT                       0x12
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_BIT_3_BMSK                   0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_BIT_3_SHFT                      0x11
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_ECS_MISC_BMSK                                           0x1fe00
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_ECS_MISC_SHFT                                               0x9
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_ECS_BHS_SLOPE_BMSK                                        0x1ff
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_V2_ECS_BHS_SLOPE_SHFT                                          0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000228)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000228)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B1M2_BMSK                                         0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B1M2_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B1M1_BMSK                                         0x38000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B1M1_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B1M0_BMSK                                          0x7000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B1M0_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_SPARE0_BMSK                                            0xf80000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_SPARE0_SHFT                                                0x13
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_CLK_LDO_ERR0_BMSK                                       0x60000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_CLK_LDO_ERR0_SHFT                                          0x11
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_VREF_ERR_B0_BMSK                                        0x18000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_VREF_ERR_B0_SHFT                                            0xf
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B0M3_BMSK                                             0x7000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B0M3_SHFT                                                0xc
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B0M2_BMSK                                              0xe00
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B0M2_SHFT                                                0x9
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B0M1_BMSK                                              0x1c0
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B0M1_SHFT                                                0x6
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B0M0_BMSK                                               0x38
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_PH_B0M0_SHFT                                                0x3
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_G_B0_BMSK                                                   0x7
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_V2_G_B0_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000022c)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000022c)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_FEC_BMSK                                             0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_FEC_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B2M3_BMSK                                           0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B2M3_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B2M2_BMSK                                           0x380000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B2M2_SHFT                                               0x13
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B2M1_BMSK                                            0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B2M1_SHFT                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B2M0_BMSK                                             0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B2M0_SHFT                                                0xd
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_G_B2_BMSK                                                0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_G_B2_SHFT                                                   0xa
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_SAR_LDO_ERR1_BMSK                                         0x300
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_SAR_LDO_ERR1_SHFT                                           0x8
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_CLK_LDO_ERR1_BMSK                                          0xc0
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_CLK_LDO_ERR1_SHFT                                           0x6
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_VREF_ERR_B1_BMSK                                           0x30
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_VREF_ERR_B1_SHFT                                            0x4
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B1M3_BMSK                                                0xe
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B1M3_SHFT                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B1M2_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_V2_PH_B1M2_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000230)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000230)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B4M0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B4M0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_G_B4_BMSK                                            0x70000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_G_B4_SHFT                                                  0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_SAR_LDO_ERR3_BMSK                                     0xc000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_SAR_LDO_ERR3_SHFT                                          0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_CLK_LDO_ERR3_BMSK                                     0x3000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_CLK_LDO_ERR3_SHFT                                          0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_VREF_ERR_B3_BMSK                                       0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_VREF_ERR_B3_SHFT                                           0x16
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B3M3_BMSK                                           0x380000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B3M3_SHFT                                               0x13
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B3M2_BMSK                                            0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B3M2_SHFT                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B3M1_BMSK                                             0xe000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B3M1_SHFT                                                0xd
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B3M0_BMSK                                             0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B3M0_SHFT                                                0xa
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_G_B3_BMSK                                                 0x380
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_G_B3_SHFT                                                   0x7
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_SAR_LDO_ERR2_BMSK                                          0x60
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_SAR_LDO_ERR2_SHFT                                           0x5
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_CLK_LDO_ERR2_BMSK                                          0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_CLK_LDO_ERR2_SHFT                                           0x3
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_VREF_ERR_B2_BMSK                                            0x6
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_VREF_ERR_B2_SHFT                                            0x1
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B2M3_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_V2_PH_B2M3_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000234)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000234)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_SPARE0_BMSK                                          0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_SPARE0_SHFT                                                0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_FEC_VALUE_BMSK                                       0x7f000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_FEC_VALUE_SHFT                                             0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B5M1_0_BMSK                                         0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B5M1_0_SHFT                                             0x17
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B5M0_BMSK                                           0x700000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B5M0_SHFT                                               0x14
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_G_B5_BMSK                                               0xe0000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_G_B5_SHFT                                                  0x11
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_SAR_LDO_ERR4_BMSK                                       0x18000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_SAR_LDO_ERR4_SHFT                                           0xf
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_CLK_LDO_ERR4_BMSK                                        0x6000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_CLK_LDO_ERR4_SHFT                                           0xd
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_VREF_ERR_B4_BMSK                                         0x1800
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_VREF_ERR_B4_SHFT                                            0xb
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B4M3_BMSK                                              0x700
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B4M3_SHFT                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B4M2_BMSK                                               0xe0
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B4M2_SHFT                                                0x5
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B4M1_BMSK                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B4M1_SHFT                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B4M0_2_1_BMSK                                            0x3
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_V2_PH_B4M0_2_1_SHFT                                            0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000238)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000238)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_CLK_LDO_ERR6_BMSK                                    0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_CLK_LDO_ERR6_SHFT                                          0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_VREF_ERR_B6_BMSK                                     0x60000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_VREF_ERR_B6_SHFT                                           0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B6M3_BMSK                                         0x1c000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B6M3_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B6M2_BMSK                                          0x3800000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B6M2_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B6M1_BMSK                                           0x700000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B6M1_SHFT                                               0x14
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B6M0_BMSK                                            0xe0000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B6M0_SHFT                                               0x11
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_G_B6_BMSK                                               0x1c000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_G_B6_SHFT                                                   0xe
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_SAR_LDO_ERR5_BMSK                                        0x3000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_SAR_LDO_ERR5_SHFT                                           0xc
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_CLK_LDO_ERR5_BMSK                                         0xc00
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_CLK_LDO_ERR5_SHFT                                           0xa
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_VREF_ERR_B5_BMSK                                          0x300
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_VREF_ERR_B5_SHFT                                            0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B5M3_BMSK                                               0xe0
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B5M3_SHFT                                                0x5
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B5M2_BMSK                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B5M2_SHFT                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B5M1_BMSK                                                0x3
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_V2_PH_B5M1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000023c)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000023c)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_FEC_BMSK                                             0xff000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_FEC_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_BMSK                                0xff0000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_SHFT                                    0x10
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_BMSK                                  0xff00
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_SHFT                                     0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_AVG_ERROR_BMSK                          0x80
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_AVG_ERROR_SHFT                           0x7
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_AVG_ERROR_BMSK                          0x40
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_AVG_ERROR_SHFT                           0x6
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC_0_1_FUSEFLAG_BMSK                              0x20
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC_0_1_FUSEFLAG_SHFT                               0x5
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_OVERFLOW_BMSK                           0x10
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_OVERFLOW_SHFT                            0x4
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_OVERFLOW_BMSK                            0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_OVERFLOW_SHFT                            0x3
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_SAR_LDO_ERR6_BMSK                                           0x6
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_SAR_LDO_ERR6_SHFT                                           0x1
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_CLK_LDO_ERR6_BMSK                                           0x1
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_V2_CLK_LDO_ERR6_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000240)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000240)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_V2_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000244)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000244)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_V2_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000248)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000248)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_V2_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000024c)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000024c)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_V2_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000250)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000250)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_V2_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_ADDR                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000254)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_OFFS                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000254)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_SPARE0_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_V2_SPARE0_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000258)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000258)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000025c)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000025c)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000260)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000260)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000264)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000264)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_ADDR, HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_ADDR(n)                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000268 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_OFFS(n)                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000268 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_MAXn                                                    13
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_ADDR(n), HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_INI(n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_REDUN_DATA_BMSK                                 0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_V2_REDUN_DATA_SHFT                                        0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_ADDR(n)                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000026c + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_OFFS(n)                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000026c + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_MAXn                                                    13
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_ADDR(n), HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_INI(n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_SPARE0_BMSK                                     0x80000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_SPARE0_SHFT                                           0x1f
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_FEC_VALUE_BMSK                                  0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_FEC_VALUE_SHFT                                        0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_REDUN_DATA_BMSK                                   0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_V2_REDUN_DATA_SHFT                                        0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000002d8)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000002d8)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_MEM_ACCEL_COMPILER_BMSK                        0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_LSB_V2_MEM_ACCEL_COMPILER_SHFT                               0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000002dc)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000002dc)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_SPARE_BMSK                                     0x80000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_SPARE_SHFT                                           0x1f
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_FEC_VALUE_BMSK                                 0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_FEC_VALUE_SHFT                                       0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_SPARE0_BMSK                                      0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW14_MSB_V2_SPARE0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000002e0)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000002e0)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_MEM_ACCEL_CUSTOM_BMSK                          0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_LSB_V2_MEM_ACCEL_CUSTOM_SHFT                                 0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000002e4)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000002e4)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_SPARE_BMSK                                     0x80000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_SPARE_SHFT                                           0x1f
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_FEC_VALUE_BMSK                                 0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_FEC_VALUE_SHFT                                       0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_SPARE0_BMSK                                      0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW15_MSB_V2_SPARE0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000002e8)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000002e8)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_MEM_ACCEL_CUSTOM_BMSK                          0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_LSB_V2_MEM_ACCEL_CUSTOM_SHFT                                 0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000002ec)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000002ec)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_SPARE_BMSK                                     0x80000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_SPARE_SHFT                                           0x1f
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_FEC_VALUE_BMSK                                 0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_FEC_VALUE_SHFT                                       0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_SPARE0_BMSK                                      0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW16_MSB_V2_SPARE0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000002f0)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000002f0)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_MEM_ACCEL_COMPILER_BMSK                        0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_LSB_V2_MEM_ACCEL_COMPILER_SHFT                               0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000002f4)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000002f4)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_SPARE_BMSK                                     0x80000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_SPARE_SHFT                                           0x1f
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_FEC_VALUE_BMSK                                 0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_FEC_VALUE_SHFT                                       0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_SPARE0_BMSK                                      0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW17_MSB_V2_SPARE0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000002f8)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000002f8)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_MEM_ACCEL_CUSTOM_BMSK                          0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_LSB_V2_MEM_ACCEL_CUSTOM_SHFT                                 0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000002fc)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000002fc)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_ADDR, HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_IN)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_SPARE_BMSK                                     0x80000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_SPARE_SHFT                                           0x1f
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_FEC_VALUE_BMSK                                 0x7f000000
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_FEC_VALUE_SHFT                                       0x18
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_SPARE0_BMSK                                      0xffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROW18_MSB_V2_SPARE0_SHFT                                           0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_ADDR(n)                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000300 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_OFFS(n)                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000300 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_MAXn                                                     31
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_PATCH_DATA_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_V2_PATCH_DATA_SHFT                                         0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_ADDR(n)                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000304 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_OFFS(n)                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000304 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_MAXn                                                     31
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_SPARE0_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_SPARE0_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_FEC_VALUE_BMSK                                   0x7f000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_FEC_VALUE_SHFT                                         0x18
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_SPARE3_BMSK                                        0xfe0000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_SPARE3_SHFT                                            0x11
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_PATCH_ADDR_BMSK                                     0x1fffe
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_PATCH_ADDR_SHFT                                         0x1
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_PATCH_EN_BMSK                                           0x1
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_V2_PATCH_EN_SHFT                                           0x0

#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_ADDR                                                     (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000400)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_OFFS                                                     (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000400)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_V2_ADDR, HWIO_QFPROM_RAW_FEC_EN_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_LSB_V2_IN)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_RSVD0_BMSK                                               0xf8000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_RSVD0_SHFT                                                     0x1b
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION26_FEC_EN_BMSK                                      0x4000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION26_FEC_EN_SHFT                                           0x1a
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION25_FEC_EN_BMSK                                      0x2000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION25_FEC_EN_SHFT                                           0x19
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION24_FEC_EN_BMSK                                      0x1000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION24_FEC_EN_SHFT                                           0x18
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION23_FEC_EN_BMSK                                       0x800000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION23_FEC_EN_SHFT                                           0x17
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION22_FEC_EN_BMSK                                       0x400000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION22_FEC_EN_SHFT                                           0x16
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION21_FEC_EN_BMSK                                       0x200000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION21_FEC_EN_SHFT                                           0x15
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION20_FEC_EN_BMSK                                       0x100000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION20_FEC_EN_SHFT                                           0x14
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION19_FEC_EN_BMSK                                        0x80000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION19_FEC_EN_SHFT                                           0x13
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION18_FEC_EN_BMSK                                        0x40000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION18_FEC_EN_SHFT                                           0x12
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION17_FEC_EN_BMSK                                        0x20000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION17_FEC_EN_SHFT                                           0x11
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION16_FEC_EN_BMSK                                        0x10000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION16_FEC_EN_SHFT                                           0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION15_FEC_EN_BMSK                                         0x8000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION15_FEC_EN_SHFT                                            0xf
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION14_FEC_EN_BMSK                                         0x4000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION14_FEC_EN_SHFT                                            0xe
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION13_FEC_EN_BMSK                                         0x2000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION13_FEC_EN_SHFT                                            0xd
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION12_FEC_EN_BMSK                                         0x1000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION12_FEC_EN_SHFT                                            0xc
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION11_FEC_EN_BMSK                                          0x800
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION11_FEC_EN_SHFT                                            0xb
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION10_FEC_EN_BMSK                                          0x400
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION10_FEC_EN_SHFT                                            0xa
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION9_FEC_EN_BMSK                                           0x200
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION9_FEC_EN_SHFT                                             0x9
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION8_FEC_EN_BMSK                                           0x100
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION8_FEC_EN_SHFT                                             0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION7_FEC_EN_BMSK                                            0x80
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION7_FEC_EN_SHFT                                             0x7
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION6_FEC_EN_BMSK                                            0x40
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION6_FEC_EN_SHFT                                             0x6
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION5_FEC_EN_BMSK                                            0x20
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION5_FEC_EN_SHFT                                             0x5
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION4_FEC_EN_BMSK                                            0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION4_FEC_EN_SHFT                                             0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION3_FEC_EN_BMSK                                             0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION3_FEC_EN_SHFT                                             0x3
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION2_FEC_EN_BMSK                                             0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION2_FEC_EN_SHFT                                             0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION1_FEC_EN_BMSK                                             0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION1_FEC_EN_SHFT                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION0_FEC_EN_BMSK                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_V2_REGION0_FEC_EN_SHFT                                             0x0

#define HWIO_QFPROM_RAW_FEC_EN_MSB_V2_ADDR                                                     (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000404)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_V2_OFFS                                                     (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000404)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_V2_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_V2_ADDR, HWIO_QFPROM_RAW_FEC_EN_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_MSB_V2_IN)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_V2_FEC_EN_REDUNDANCY_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_V2_FEC_EN_REDUNDANCY_SHFT                                          0x0

#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000408)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000408)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000040c)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000040c)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_SPARE1_BMSK                                           0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG18_MSB_V2_SPARE1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000410)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000410)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000414)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000414)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_SPARE1_BMSK                                           0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_V2_SPARE1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000418)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000418)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000041c)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000041c)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_SPARE1_BMSK                                           0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG20_MSB_V2_SPARE1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000420)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000420)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000424)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000424)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_SPARE1_BMSK                                           0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG21_MSB_V2_SPARE1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000428)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000428)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG22_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000042c)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000042c)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_SPARE1_BMSK                                           0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG22_MSB_V2_SPARE1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000430)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000430)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG23_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000434)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000434)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_SPARE1_BMSK                                           0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG23_MSB_V2_SPARE1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000438)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000438)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG24_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000043c)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000043c)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_SPARE1_BMSK                                           0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG24_MSB_V2_SPARE1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000440)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000440)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG25_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000444)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000444)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_SPARE1_BMSK                                           0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG25_MSB_V2_SPARE1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000448)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000448)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG26_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000044c)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000044c)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_ADDR, HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_IN)
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_SPARE1_BMSK                                           0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG26_MSB_V2_SPARE1_SHFT                                                0x0

#define HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_ADDR(n)                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00000800 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_OFFS(n)                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00000800 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_MAXn                                                           39
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_ADDR(n), HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_RMSK)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_ACC_PRIVATE_BMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_V2_ACC_PRIVATE_SHFT                                              0x0

#define HWIO_ACC_IR_V2_ADDR                                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002000)
#define HWIO_ACC_IR_V2_OFFS                                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002000)
#define HWIO_ACC_IR_V2_RMSK                                                                          0x1f
#define HWIO_ACC_IR_V2_OUT(v)      \
        out_dword(HWIO_ACC_IR_V2_ADDR,v)
#define HWIO_ACC_IR_V2_INSTRUCTION_BMSK                                                              0x1f
#define HWIO_ACC_IR_V2_INSTRUCTION_SHFT                                                               0x0

#define HWIO_ACC_DR_V2_ADDR                                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002004)
#define HWIO_ACC_DR_V2_OFFS                                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002004)
#define HWIO_ACC_DR_V2_RMSK                                                                    0xffffffff
#define HWIO_ACC_DR_V2_IN          \
        in_dword_masked(HWIO_ACC_DR_V2_ADDR, HWIO_ACC_DR_V2_RMSK)
#define HWIO_ACC_DR_V2_INM(m)      \
        in_dword_masked(HWIO_ACC_DR_V2_ADDR, m)
#define HWIO_ACC_DR_V2_OUT(v)      \
        out_dword(HWIO_ACC_DR_V2_ADDR,v)
#define HWIO_ACC_DR_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_ACC_DR_V2_ADDR,m,v,HWIO_ACC_DR_V2_IN)
#define HWIO_ACC_DR_V2_DR_BMSK                                                                 0xffffffff
#define HWIO_ACC_DR_V2_DR_SHFT                                                                        0x0

#define HWIO_ACC_VERID_V2_ADDR                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002008)
#define HWIO_ACC_VERID_V2_OFFS                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002008)
#define HWIO_ACC_VERID_V2_RMSK                                                                     0xffff
#define HWIO_ACC_VERID_V2_IN          \
        in_dword_masked(HWIO_ACC_VERID_V2_ADDR, HWIO_ACC_VERID_V2_RMSK)
#define HWIO_ACC_VERID_V2_INM(m)      \
        in_dword_masked(HWIO_ACC_VERID_V2_ADDR, m)
#define HWIO_ACC_VERID_V2_FWVERID_BMSK                                                             0xff00
#define HWIO_ACC_VERID_V2_FWVERID_SHFT                                                                0x8
#define HWIO_ACC_VERID_V2_HWVERID_BMSK                                                               0xff
#define HWIO_ACC_VERID_V2_HWVERID_SHFT                                                                0x0

#define HWIO_ACC_FEATSETn_V2_ADDR(n)                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_V2_OFFS(n)                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_V2_RMSK                                                              0xffffffff
#define HWIO_ACC_FEATSETn_V2_MAXn                                                                       7
#define HWIO_ACC_FEATSETn_V2_INI(n)        \
        in_dword_masked(HWIO_ACC_FEATSETn_V2_ADDR(n), HWIO_ACC_FEATSETn_V2_RMSK)
#define HWIO_ACC_FEATSETn_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_ACC_FEATSETn_V2_ADDR(n), mask)
#define HWIO_ACC_FEATSETn_V2_FEAT_BMSK                                                         0xffffffff
#define HWIO_ACC_FEATSETn_V2_FEAT_SHFT                                                                0x0

#define HWIO_ACC_STATE_V2_ADDR                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002038)
#define HWIO_ACC_STATE_V2_OFFS                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002038)
#define HWIO_ACC_STATE_V2_RMSK                                                                        0x7
#define HWIO_ACC_STATE_V2_IN          \
        in_dword_masked(HWIO_ACC_STATE_V2_ADDR, HWIO_ACC_STATE_V2_RMSK)
#define HWIO_ACC_STATE_V2_INM(m)      \
        in_dword_masked(HWIO_ACC_STATE_V2_ADDR, m)
#define HWIO_ACC_STATE_V2_ACC_READY_BMSK                                                              0x4
#define HWIO_ACC_STATE_V2_ACC_READY_SHFT                                                              0x2
#define HWIO_ACC_STATE_V2_ACC_LOCKED_BMSK                                                             0x2
#define HWIO_ACC_STATE_V2_ACC_LOCKED_SHFT                                                             0x1
#define HWIO_ACC_STATE_V2_ACC_STOP_BMSK                                                               0x1
#define HWIO_ACC_STATE_V2_ACC_STOP_SHFT                                                               0x0

#define HWIO_QFPROM_BLOW_TIMER_V2_ADDR                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000203c)
#define HWIO_QFPROM_BLOW_TIMER_V2_OFFS                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000203c)
#define HWIO_QFPROM_BLOW_TIMER_V2_RMSK                                                              0xfff
#define HWIO_QFPROM_BLOW_TIMER_V2_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_V2_ADDR, HWIO_QFPROM_BLOW_TIMER_V2_RMSK)
#define HWIO_QFPROM_BLOW_TIMER_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_V2_ADDR, m)
#define HWIO_QFPROM_BLOW_TIMER_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_BLOW_TIMER_V2_ADDR,v)
#define HWIO_QFPROM_BLOW_TIMER_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BLOW_TIMER_V2_ADDR,m,v,HWIO_QFPROM_BLOW_TIMER_V2_IN)
#define HWIO_QFPROM_BLOW_TIMER_V2_BLOW_TIMER_BMSK                                                   0xfff
#define HWIO_QFPROM_BLOW_TIMER_V2_BLOW_TIMER_SHFT                                                     0x0

#define HWIO_QFPROM_TEST_CTRL_V2_ADDR                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002040)
#define HWIO_QFPROM_TEST_CTRL_V2_OFFS                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002040)
#define HWIO_QFPROM_TEST_CTRL_V2_RMSK                                                                 0xf
#define HWIO_QFPROM_TEST_CTRL_V2_IN          \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_V2_ADDR, HWIO_QFPROM_TEST_CTRL_V2_RMSK)
#define HWIO_QFPROM_TEST_CTRL_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_V2_ADDR, m)
#define HWIO_QFPROM_TEST_CTRL_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_TEST_CTRL_V2_ADDR,v)
#define HWIO_QFPROM_TEST_CTRL_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_TEST_CTRL_V2_ADDR,m,v,HWIO_QFPROM_TEST_CTRL_V2_IN)
#define HWIO_QFPROM_TEST_CTRL_V2_SEL_TST_ROM_BMSK                                                     0x8
#define HWIO_QFPROM_TEST_CTRL_V2_SEL_TST_ROM_SHFT                                                     0x3
#define HWIO_QFPROM_TEST_CTRL_V2_SEL_TST_WL_BMSK                                                      0x4
#define HWIO_QFPROM_TEST_CTRL_V2_SEL_TST_WL_SHFT                                                      0x2
#define HWIO_QFPROM_TEST_CTRL_V2_SEL_TST_BL_BMSK                                                      0x2
#define HWIO_QFPROM_TEST_CTRL_V2_SEL_TST_BL_SHFT                                                      0x1
#define HWIO_QFPROM_TEST_CTRL_V2_EN_FUSE_RES_MEAS_BMSK                                                0x1
#define HWIO_QFPROM_TEST_CTRL_V2_EN_FUSE_RES_MEAS_SHFT                                                0x0

#define HWIO_QFPROM_ACCEL_V2_ADDR                                                              (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002044)
#define HWIO_QFPROM_ACCEL_V2_OFFS                                                              (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002044)
#define HWIO_QFPROM_ACCEL_V2_RMSK                                                                   0xfff
#define HWIO_QFPROM_ACCEL_V2_IN          \
        in_dword_masked(HWIO_QFPROM_ACCEL_V2_ADDR, HWIO_QFPROM_ACCEL_V2_RMSK)
#define HWIO_QFPROM_ACCEL_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ACCEL_V2_ADDR, m)
#define HWIO_QFPROM_ACCEL_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_ACCEL_V2_ADDR,v)
#define HWIO_QFPROM_ACCEL_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_ACCEL_V2_ADDR,m,v,HWIO_QFPROM_ACCEL_V2_IN)
#define HWIO_QFPROM_ACCEL_V2_QFPROM_GATELAST_BMSK                                                   0x800
#define HWIO_QFPROM_ACCEL_V2_QFPROM_GATELAST_SHFT                                                     0xb
#define HWIO_QFPROM_ACCEL_V2_QFPROM_TRIPPT_SEL_BMSK                                                 0x700
#define HWIO_QFPROM_ACCEL_V2_QFPROM_TRIPPT_SEL_SHFT                                                   0x8
#define HWIO_QFPROM_ACCEL_V2_QFPROM_ACCEL_BMSK                                                       0xff
#define HWIO_QFPROM_ACCEL_V2_QFPROM_ACCEL_SHFT                                                        0x0

#define HWIO_QFPROM_BLOW_STATUS_V2_ADDR                                                        (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_V2_OFFS                                                        (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_V2_RMSK                                                               0x3
#define HWIO_QFPROM_BLOW_STATUS_V2_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_V2_ADDR, HWIO_QFPROM_BLOW_STATUS_V2_RMSK)
#define HWIO_QFPROM_BLOW_STATUS_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_V2_ADDR, m)
#define HWIO_QFPROM_BLOW_STATUS_V2_QFPROM_WR_ERR_BMSK                                                 0x2
#define HWIO_QFPROM_BLOW_STATUS_V2_QFPROM_WR_ERR_SHFT                                                 0x1
#define HWIO_QFPROM_BLOW_STATUS_V2_QFPROM_BUSY_BMSK                                                   0x1
#define HWIO_QFPROM_BLOW_STATUS_V2_QFPROM_BUSY_SHFT                                                   0x0

#define HWIO_QFPROM_ROM_ERROR_V2_ADDR                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000204c)
#define HWIO_QFPROM_ROM_ERROR_V2_OFFS                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000204c)
#define HWIO_QFPROM_ROM_ERROR_V2_RMSK                                                                 0x1
#define HWIO_QFPROM_ROM_ERROR_V2_IN          \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_V2_ADDR, HWIO_QFPROM_ROM_ERROR_V2_RMSK)
#define HWIO_QFPROM_ROM_ERROR_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_V2_ADDR, m)
#define HWIO_QFPROM_ROM_ERROR_V2_ERROR_BMSK                                                           0x1
#define HWIO_QFPROM_ROM_ERROR_V2_ERROR_SHFT                                                           0x0

#define HWIO_QFPROM_BIST_CTRL_V2_ADDR                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_V2_OFFS                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_V2_RMSK                                                                0x7f
#define HWIO_QFPROM_BIST_CTRL_V2_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_V2_ADDR, HWIO_QFPROM_BIST_CTRL_V2_RMSK)
#define HWIO_QFPROM_BIST_CTRL_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_V2_ADDR, m)
#define HWIO_QFPROM_BIST_CTRL_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_BIST_CTRL_V2_ADDR,v)
#define HWIO_QFPROM_BIST_CTRL_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BIST_CTRL_V2_ADDR,m,v,HWIO_QFPROM_BIST_CTRL_V2_IN)
#define HWIO_QFPROM_BIST_CTRL_V2_AUTH_REGION_BMSK                                                    0x7c
#define HWIO_QFPROM_BIST_CTRL_V2_AUTH_REGION_SHFT                                                     0x2
#define HWIO_QFPROM_BIST_CTRL_V2_SHA_ENABLE_BMSK                                                      0x2
#define HWIO_QFPROM_BIST_CTRL_V2_SHA_ENABLE_SHFT                                                      0x1
#define HWIO_QFPROM_BIST_CTRL_V2_START_BMSK                                                           0x1
#define HWIO_QFPROM_BIST_CTRL_V2_START_SHFT                                                           0x0

#define HWIO_QFPROM_BIST_ERROR_V2_ADDR                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_V2_OFFS                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_V2_RMSK                                                         0xffffffff
#define HWIO_QFPROM_BIST_ERROR_V2_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_V2_ADDR, HWIO_QFPROM_BIST_ERROR_V2_RMSK)
#define HWIO_QFPROM_BIST_ERROR_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_V2_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR_V2_ERROR_BMSK                                                   0xffffffff
#define HWIO_QFPROM_BIST_ERROR_V2_ERROR_SHFT                                                          0x0

#define HWIO_QFPROM_HASH_SIGNATUREn_V2_ADDR(n)                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_V2_OFFS(n)                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_V2_RMSK                                                    0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_V2_MAXn                                                             7
#define HWIO_QFPROM_HASH_SIGNATUREn_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_V2_ADDR(n), HWIO_QFPROM_HASH_SIGNATUREn_V2_RMSK)
#define HWIO_QFPROM_HASH_SIGNATUREn_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_V2_ADDR(n), mask)
#define HWIO_QFPROM_HASH_SIGNATUREn_V2_HASH_VALUE_BMSK                                         0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_V2_HASH_VALUE_SHFT                                                0x0

#define HWIO_HW_KEY_STATUS_V2_ADDR                                                             (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002080)
#define HWIO_HW_KEY_STATUS_V2_OFFS                                                             (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002080)
#define HWIO_HW_KEY_STATUS_V2_RMSK                                                                   0x7f
#define HWIO_HW_KEY_STATUS_V2_IN          \
        in_dword_masked(HWIO_HW_KEY_STATUS_V2_ADDR, HWIO_HW_KEY_STATUS_V2_RMSK)
#define HWIO_HW_KEY_STATUS_V2_INM(m)      \
        in_dword_masked(HWIO_HW_KEY_STATUS_V2_ADDR, m)
#define HWIO_HW_KEY_STATUS_V2_FUSE_SENSE_DONE_BMSK                                                   0x40
#define HWIO_HW_KEY_STATUS_V2_FUSE_SENSE_DONE_SHFT                                                    0x6
#define HWIO_HW_KEY_STATUS_V2_CRI_CM_BOOT_DONE_BMSK                                                  0x20
#define HWIO_HW_KEY_STATUS_V2_CRI_CM_BOOT_DONE_SHFT                                                   0x5
#define HWIO_HW_KEY_STATUS_V2_KDF_DONE_BMSK                                                          0x10
#define HWIO_HW_KEY_STATUS_V2_KDF_DONE_SHFT                                                           0x4
#define HWIO_HW_KEY_STATUS_V2_MSA_KEYS_BLOCKED_BMSK                                                   0x8
#define HWIO_HW_KEY_STATUS_V2_MSA_KEYS_BLOCKED_SHFT                                                   0x3
#define HWIO_HW_KEY_STATUS_V2_APPS_KEYS_BLOCKED_BMSK                                                  0x4
#define HWIO_HW_KEY_STATUS_V2_APPS_KEYS_BLOCKED_SHFT                                                  0x2
#define HWIO_HW_KEY_STATUS_V2_SEC_KEY_DERIVATION_KEY_BLOWN_BMSK                                       0x2
#define HWIO_HW_KEY_STATUS_V2_SEC_KEY_DERIVATION_KEY_BLOWN_SHFT                                       0x1
#define HWIO_HW_KEY_STATUS_V2_PRI_KEY_DERIVATION_KEY_BLOWN_BMSK                                       0x1
#define HWIO_HW_KEY_STATUS_V2_PRI_KEY_DERIVATION_KEY_BLOWN_SHFT                                       0x0

#define HWIO_RESET_JDR_STATUS_V2_ADDR                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002084)
#define HWIO_RESET_JDR_STATUS_V2_OFFS                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002084)
#define HWIO_RESET_JDR_STATUS_V2_RMSK                                                                 0x3
#define HWIO_RESET_JDR_STATUS_V2_IN          \
        in_dword_masked(HWIO_RESET_JDR_STATUS_V2_ADDR, HWIO_RESET_JDR_STATUS_V2_RMSK)
#define HWIO_RESET_JDR_STATUS_V2_INM(m)      \
        in_dword_masked(HWIO_RESET_JDR_STATUS_V2_ADDR, m)
#define HWIO_RESET_JDR_STATUS_V2_FORCE_RESET_BMSK                                                     0x2
#define HWIO_RESET_JDR_STATUS_V2_FORCE_RESET_SHFT                                                     0x1
#define HWIO_RESET_JDR_STATUS_V2_DISABLE_SYSTEM_RESET_BMSK                                            0x1
#define HWIO_RESET_JDR_STATUS_V2_DISABLE_SYSTEM_RESET_SHFT                                            0x0

#define HWIO_FEC_ESR_V2_ADDR                                                                   (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002090)
#define HWIO_FEC_ESR_V2_OFFS                                                                   (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002090)
#define HWIO_FEC_ESR_V2_RMSK                                                                       0x3fff
#define HWIO_FEC_ESR_V2_IN          \
        in_dword_masked(HWIO_FEC_ESR_V2_ADDR, HWIO_FEC_ESR_V2_RMSK)
#define HWIO_FEC_ESR_V2_INM(m)      \
        in_dword_masked(HWIO_FEC_ESR_V2_ADDR, m)
#define HWIO_FEC_ESR_V2_OUT(v)      \
        out_dword(HWIO_FEC_ESR_V2_ADDR,v)
#define HWIO_FEC_ESR_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FEC_ESR_V2_ADDR,m,v,HWIO_FEC_ESR_V2_IN)
#define HWIO_FEC_ESR_V2_CORR_SW_ACC_BMSK                                                           0x2000
#define HWIO_FEC_ESR_V2_CORR_SW_ACC_SHFT                                                              0xd
#define HWIO_FEC_ESR_V2_CORR_SECURE_CHANNEL_BMSK                                                    0x800
#define HWIO_FEC_ESR_V2_CORR_SECURE_CHANNEL_SHFT                                                      0xb
#define HWIO_FEC_ESR_V2_CORR_BOOT_ROM_BMSK                                                          0x400
#define HWIO_FEC_ESR_V2_CORR_BOOT_ROM_SHFT                                                            0xa
#define HWIO_FEC_ESR_V2_CORR_FUSE_SENSE_BMSK                                                        0x200
#define HWIO_FEC_ESR_V2_CORR_FUSE_SENSE_SHFT                                                          0x9
#define HWIO_FEC_ESR_V2_CORR_MULT_BMSK                                                              0x100
#define HWIO_FEC_ESR_V2_CORR_MULT_SHFT                                                                0x8
#define HWIO_FEC_ESR_V2_CORR_SEEN_BMSK                                                               0x80
#define HWIO_FEC_ESR_V2_CORR_SEEN_SHFT                                                                0x7
#define HWIO_FEC_ESR_V2_ERR_SW_ACC_BMSK                                                              0x40
#define HWIO_FEC_ESR_V2_ERR_SW_ACC_SHFT                                                               0x6
#define HWIO_FEC_ESR_V2_ERR_SECURE_CHANNEL_BMSK                                                      0x10
#define HWIO_FEC_ESR_V2_ERR_SECURE_CHANNEL_SHFT                                                       0x4
#define HWIO_FEC_ESR_V2_ERR_BOOT_ROM_BMSK                                                             0x8
#define HWIO_FEC_ESR_V2_ERR_BOOT_ROM_SHFT                                                             0x3
#define HWIO_FEC_ESR_V2_ERR_FUSE_SENSE_BMSK                                                           0x4
#define HWIO_FEC_ESR_V2_ERR_FUSE_SENSE_SHFT                                                           0x2
#define HWIO_FEC_ESR_V2_ERR_MULT_BMSK                                                                 0x2
#define HWIO_FEC_ESR_V2_ERR_MULT_SHFT                                                                 0x1
#define HWIO_FEC_ESR_V2_ERR_SEEN_BMSK                                                                 0x1
#define HWIO_FEC_ESR_V2_ERR_SEEN_SHFT                                                                 0x0

#define HWIO_FEC_EAR_V2_ADDR                                                                   (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002094)
#define HWIO_FEC_EAR_V2_OFFS                                                                   (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002094)
#define HWIO_FEC_EAR_V2_RMSK                                                                   0xffffffff
#define HWIO_FEC_EAR_V2_IN          \
        in_dword_masked(HWIO_FEC_EAR_V2_ADDR, HWIO_FEC_EAR_V2_RMSK)
#define HWIO_FEC_EAR_V2_INM(m)      \
        in_dword_masked(HWIO_FEC_EAR_V2_ADDR, m)
#define HWIO_FEC_EAR_V2_CORR_ADDR_BMSK                                                         0xffff0000
#define HWIO_FEC_EAR_V2_CORR_ADDR_SHFT                                                               0x10
#define HWIO_FEC_EAR_V2_ERR_ADDR_BMSK                                                              0xffff
#define HWIO_FEC_EAR_V2_ERR_ADDR_SHFT                                                                 0x0

#define HWIO_QFPROM0_MATCH_STATUS_V2_ADDR                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00002098)
#define HWIO_QFPROM0_MATCH_STATUS_V2_OFFS                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00002098)
#define HWIO_QFPROM0_MATCH_STATUS_V2_RMSK                                                      0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_V2_IN          \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_V2_ADDR, HWIO_QFPROM0_MATCH_STATUS_V2_RMSK)
#define HWIO_QFPROM0_MATCH_STATUS_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_V2_ADDR, m)
#define HWIO_QFPROM0_MATCH_STATUS_V2_FLAG_BMSK                                                 0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_V2_FLAG_SHFT                                                        0x0

#define HWIO_QFPROM1_MATCH_STATUS_V2_ADDR                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000209c)
#define HWIO_QFPROM1_MATCH_STATUS_V2_OFFS                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000209c)
#define HWIO_QFPROM1_MATCH_STATUS_V2_RMSK                                                      0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_V2_IN          \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_V2_ADDR, HWIO_QFPROM1_MATCH_STATUS_V2_RMSK)
#define HWIO_QFPROM1_MATCH_STATUS_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_V2_ADDR, m)
#define HWIO_QFPROM1_MATCH_STATUS_V2_FLAG_BMSK                                                 0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_V2_FLAG_SHFT                                                        0x0

#define HWIO_FEAT_PROV_OUTn_V2_ADDR(n)                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000020a0 + 0x4 * (n))
#define HWIO_FEAT_PROV_OUTn_V2_OFFS(n)                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000020a0 + 0x4 * (n))
#define HWIO_FEAT_PROV_OUTn_V2_RMSK                                                            0xffffffff
#define HWIO_FEAT_PROV_OUTn_V2_MAXn                                                                     3
#define HWIO_FEAT_PROV_OUTn_V2_INI(n)        \
        in_dword_masked(HWIO_FEAT_PROV_OUTn_V2_ADDR(n), HWIO_FEAT_PROV_OUTn_V2_RMSK)
#define HWIO_FEAT_PROV_OUTn_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_FEAT_PROV_OUTn_V2_ADDR(n), mask)
#define HWIO_FEAT_PROV_OUTn_V2_FEAT_PROV_OUT_VALUE_BMSK                                        0xffffffff
#define HWIO_FEAT_PROV_OUTn_V2_FEAT_PROV_OUT_VALUE_SHFT                                               0x0

#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_ADDR(n)                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000020b0 + 0x4 * (n))
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_OFFS(n)                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000020b0 + 0x4 * (n))
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_RMSK                                              0xffffffff
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_MAXn                                                       3
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_INI(n)        \
        in_dword_masked(HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_ADDR(n), HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_RMSK)
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_ADDR(n), mask)
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_SEC_CTRL_MISC_CONFIG_STATUS_VALUE_BMSK            0xffffffff
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_V2_SEC_CTRL_MISC_CONFIG_STATUS_VALUE_SHFT                   0x0

#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_ADDR(n)                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004000 + 0x4 * (n))
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_OFFS(n)                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004000 + 0x4 * (n))
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_MAXn                                                       71
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_ADDR(n), HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_RMSK)
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_CRI_CM_PRIVATE_BMSK                                0xffffffff
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_V2_CRI_CM_PRIVATE_SHFT                                       0x0

#define HWIO_QFPROM_CORR_JTAG_ID_V2_ADDR                                                       (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004120)
#define HWIO_QFPROM_CORR_JTAG_ID_V2_OFFS                                                       (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004120)
#define HWIO_QFPROM_CORR_JTAG_ID_V2_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_JTAG_ID_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_V2_ADDR, HWIO_QFPROM_CORR_JTAG_ID_V2_RMSK)
#define HWIO_QFPROM_CORR_JTAG_ID_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_V2_ADDR, m)
#define HWIO_QFPROM_CORR_JTAG_ID_V2_IDDQ_CX_ON_BMSK                                            0xc0000000
#define HWIO_QFPROM_CORR_JTAG_ID_V2_IDDQ_CX_ON_SHFT                                                  0x1e
#define HWIO_QFPROM_CORR_JTAG_ID_V2_SPARE_BMSK                                                 0x20000000
#define HWIO_QFPROM_CORR_JTAG_ID_V2_SPARE_SHFT                                                       0x1d
#define HWIO_QFPROM_CORR_JTAG_ID_V2_MACCHIATO_EN_BMSK                                          0x10000000
#define HWIO_QFPROM_CORR_JTAG_ID_V2_MACCHIATO_EN_SHFT                                                0x1c
#define HWIO_QFPROM_CORR_JTAG_ID_V2_FEATURE_ID_BMSK                                             0xff00000
#define HWIO_QFPROM_CORR_JTAG_ID_V2_FEATURE_ID_SHFT                                                  0x14
#define HWIO_QFPROM_CORR_JTAG_ID_V2_JTAG_ID_BMSK                                                  0xfffff
#define HWIO_QFPROM_CORR_JTAG_ID_V2_JTAG_ID_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_PTE1_V2_ADDR                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004124)
#define HWIO_QFPROM_CORR_PTE1_V2_OFFS                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004124)
#define HWIO_QFPROM_CORR_PTE1_V2_RMSK                                                          0xffffffff
#define HWIO_QFPROM_CORR_PTE1_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_V2_ADDR, HWIO_QFPROM_CORR_PTE1_V2_RMSK)
#define HWIO_QFPROM_CORR_PTE1_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_V2_ADDR, m)
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_BIN_BMSK                                                 0xe0000000
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_BIN_SHFT                                                       0x1d
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_MULTIPLIER_BMSK                                          0x1c000000
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_MULTIPLIER_SHFT                                                0x1a
#define HWIO_QFPROM_CORR_PTE1_V2_PROCESS_NODE_ID_BMSK                                           0x2000000
#define HWIO_QFPROM_CORR_PTE1_V2_PROCESS_NODE_ID_SHFT                                                0x19
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_MX_OFF_BMSK                                               0x1f80000
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_MX_OFF_SHFT                                                    0x13
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_CX_OFF_BMSK                                                 0x7f000
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_CX_OFF_SHFT                                                     0xc
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_MX_ON_BMSK                                                    0xfe0
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_MX_ON_SHFT                                                      0x5
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_CX_ON_BMSK                                                     0x1f
#define HWIO_QFPROM_CORR_PTE1_V2_IDDQ_CX_ON_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_SERIAL_NUM_V2_ADDR                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004128)
#define HWIO_QFPROM_CORR_SERIAL_NUM_V2_OFFS                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004128)
#define HWIO_QFPROM_CORR_SERIAL_NUM_V2_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_V2_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_V2_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_V2_SERIAL_NUM_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_V2_SERIAL_NUM_SHFT                                                0x0

#define HWIO_QFPROM_CORR_PTE2_V2_ADDR                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000412c)
#define HWIO_QFPROM_CORR_PTE2_V2_OFFS                                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000412c)
#define HWIO_QFPROM_CORR_PTE2_V2_RMSK                                                          0xffffffff
#define HWIO_QFPROM_CORR_PTE2_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_V2_ADDR, HWIO_QFPROM_CORR_PTE2_V2_RMSK)
#define HWIO_QFPROM_CORR_PTE2_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_V2_ADDR, m)
#define HWIO_QFPROM_CORR_PTE2_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_PTE2_V2_ADDR,v)
#define HWIO_QFPROM_CORR_PTE2_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_PTE2_V2_ADDR,m,v,HWIO_QFPROM_CORR_PTE2_V2_IN)
#define HWIO_QFPROM_CORR_PTE2_V2_MX_RET_BIN_BMSK                                               0xe0000000
#define HWIO_QFPROM_CORR_PTE2_V2_MX_RET_BIN_SHFT                                                     0x1d
#define HWIO_QFPROM_CORR_PTE2_V2_IDDQ_SPARE_2_BMSK                                             0x10000000
#define HWIO_QFPROM_CORR_PTE2_V2_IDDQ_SPARE_2_SHFT                                                   0x1c
#define HWIO_QFPROM_CORR_PTE2_V2_IDDQ_RC_BMSK                                                   0xc000000
#define HWIO_QFPROM_CORR_PTE2_V2_IDDQ_RC_SHFT                                                        0x1a
#define HWIO_QFPROM_CORR_PTE2_V2_IDDQ_MSS_OFF_BMSK                                              0x3f80000
#define HWIO_QFPROM_CORR_PTE2_V2_IDDQ_MSS_OFF_SHFT                                                   0x13
#define HWIO_QFPROM_CORR_PTE2_V2_IDDQ_MSS_ON_BMSK                                                 0x7f800
#define HWIO_QFPROM_CORR_PTE2_V2_IDDQ_MSS_ON_SHFT                                                     0xb
#define HWIO_QFPROM_CORR_PTE2_V2_FOUNDRY_ID_BMSK                                                    0x700
#define HWIO_QFPROM_CORR_PTE2_V2_FOUNDRY_ID_SHFT                                                      0x8
#define HWIO_QFPROM_CORR_PTE2_V2_LOGIC_RETENTION_BMSK                                                0xe0
#define HWIO_QFPROM_CORR_PTE2_V2_LOGIC_RETENTION_SHFT                                                 0x5
#define HWIO_QFPROM_CORR_PTE2_V2_SPEED_BIN_BMSK                                                      0x1c
#define HWIO_QFPROM_CORR_PTE2_V2_SPEED_BIN_SHFT                                                       0x2
#define HWIO_QFPROM_CORR_PTE2_V2_IDDQ_SPARE_BMSK                                                      0x3
#define HWIO_QFPROM_CORR_PTE2_V2_IDDQ_SPARE_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004130)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004130)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_RSVD0_BMSK                                          0xf8000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_RSVD0_SHFT                                                0x1b
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE26_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE26_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE25_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE25_SHFT                                              0x19
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE24_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE24_SHFT                                              0x18
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE23_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE23_SHFT                                              0x17
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE22_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE22_SHFT                                              0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE21_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE21_SHFT                                              0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE20_BMSK                                          0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE20_SHFT                                              0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE19_BMSK                                           0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE19_SHFT                                              0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE18_BMSK                                           0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SPARE18_SHFT                                              0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_FEC_EN_BMSK                                            0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_FEC_EN_SHFT                                               0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_BOOT_ROM_PATCH_BMSK                                    0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_BOOT_ROM_PATCH_SHFT                                       0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_MEM_CONFIG_BMSK                                         0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_MEM_CONFIG_SHFT                                            0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_CALIB_BMSK                                              0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_CALIB_SHFT                                                 0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_PK_HASH_BMSK                                            0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_PK_HASH_SHFT                                               0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_CALIB2_BMSK                                             0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_CALIB2_SHFT                                                0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_OEM_SEC_BOOT_BMSK                                        0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_OEM_SEC_BOOT_SHFT                                          0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SEC_KEY_DERIVATION_KEY_BMSK                              0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_SEC_KEY_DERIVATION_KEY_SHFT                                0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_PRI_KEY_DERIVATION_KEY_BMSK                              0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_PRI_KEY_DERIVATION_KEY_SHFT                                0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_CM_FEAT_CONFIG_BMSK                                      0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_CM_FEAT_CONFIG_SHFT                                        0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_FEAT_CONFIG_BMSK                                          0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_FEAT_CONFIG_SHFT                                           0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_OEM_CONFIG_BMSK                                           0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_OEM_CONFIG_SHFT                                            0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_3_BMSK                                      0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_3_SHFT                                       0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_2_BMSK                                      0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_2_SHFT                                       0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_1_BMSK                                       0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_ANTI_ROLLBACK_1_SHFT                                       0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_RD_WR_PERM_BMSK                                            0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_RD_WR_PERM_SHFT                                            0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_PTE_BMSK                                                   0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_PTE_SHFT                                                   0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_CRI_CM_PRIVATE_BMSK                                        0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_V2_CRI_CM_PRIVATE_SHFT                                        0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004134)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004134)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_RSVD0_BMSK                                          0xf8000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_RSVD0_SHFT                                                0x1b
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE26_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE26_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE25_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE25_SHFT                                              0x19
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE24_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE24_SHFT                                              0x18
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE23_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE23_SHFT                                              0x17
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE22_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE22_SHFT                                              0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE21_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE21_SHFT                                              0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE20_BMSK                                          0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE20_SHFT                                              0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE19_BMSK                                           0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE19_SHFT                                              0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE18_BMSK                                           0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SPARE18_SHFT                                              0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_FEC_EN_BMSK                                            0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_FEC_EN_SHFT                                               0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_BOOT_ROM_PATCH_BMSK                                    0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_BOOT_ROM_PATCH_SHFT                                       0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_MEM_CONFIG_BMSK                                         0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_MEM_CONFIG_SHFT                                            0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_CALIB_BMSK                                              0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_CALIB_SHFT                                                 0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_PK_HASH_BMSK                                            0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_PK_HASH_SHFT                                               0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_CALIB2_BMSK                                             0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_CALIB2_SHFT                                                0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_OEM_SEC_BOOT_BMSK                                        0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_OEM_SEC_BOOT_SHFT                                          0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SEC_KEY_DERIVATION_KEY_BMSK                              0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_SEC_KEY_DERIVATION_KEY_SHFT                                0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_PRI_KEY_DERIVATION_KEY_BMSK                              0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_PRI_KEY_DERIVATION_KEY_SHFT                                0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_CM_FEAT_CONFIG_BMSK                                      0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_CM_FEAT_CONFIG_SHFT                                        0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_FEAT_CONFIG_BMSK                                          0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_FEAT_CONFIG_SHFT                                           0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_OEM_CONFIG_BMSK                                           0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_OEM_CONFIG_SHFT                                            0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_3_BMSK                                      0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_3_SHFT                                       0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_2_BMSK                                      0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_2_SHFT                                       0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_1_BMSK                                       0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_ANTI_ROLLBACK_1_SHFT                                       0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_RD_WR_PERM_BMSK                                            0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_RD_WR_PERM_SHFT                                            0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_PTE_BMSK                                                   0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_PTE_SHFT                                                   0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_CRI_CM_PRIVATE_BMSK                                        0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_V2_CRI_CM_PRIVATE_SHFT                                        0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004138)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004138)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_PIL_SUBSYSTEM0_BMSK                            0xfc000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_PIL_SUBSYSTEM0_SHFT                                  0x1a
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_TZ_BMSK                                         0x3fff000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_TZ_SHFT                                               0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_SBL1_BMSK                                           0xffe
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_SBL1_SHFT                                             0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_RPMB_KEY_PROVISIONED_BMSK                             0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_V2_RPMB_KEY_PROVISIONED_SHFT                             0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000413c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000413c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_APPSBL0_BMSK                                   0xfffc0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_APPSBL0_SHFT                                         0x12
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_PIL_SUBSYSTEM1_BMSK                               0x3ffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_V2_PIL_SUBSYSTEM1_SHFT                                   0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004140)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004140)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_V2_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_V2_APPSBL1_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_V2_APPSBL1_SHFT                                          0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004144)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004144)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_ROOT_CERT_PK_HASH_INDEX_BMSK                   0xff000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_ROOT_CERT_PK_HASH_INDEX_SHFT                         0x18
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_HYPERVISOR_BMSK                                  0xfff000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_HYPERVISOR_SHFT                                       0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_RPM_BMSK                                            0xff0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_RPM_SHFT                                              0x4
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_APPSBL2_BMSK                                          0xf
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_V2_APPSBL2_SHFT                                          0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004148)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004148)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_MSS_BMSK                                       0xffff0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_MSS_SHFT                                             0x10
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_MBA_BMSK                                           0xffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_V2_MBA_SHFT                                              0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000414c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000414c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_SPARE0_BMSK                                    0xffffff00
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_SPARE0_SHFT                                           0x8
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                   0xff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_V2_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                    0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004150)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004150)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_DISABLE_CRASH_DUMP_OU_FIELD_BMSK               0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_DISABLE_CRASH_DUMP_OU_FIELD_SHFT                     0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_DEBUG_POLICY_DISABLE_BMSK                      0x40000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_DEBUG_POLICY_DISABLE_SHFT                            0x1e
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_RSVD0_BMSK                                     0x30000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_RSVD0_SHFT                                           0x1c
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG26_SECURE_BMSK                         0x8000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG26_SECURE_SHFT                              0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG25_SECURE_BMSK                         0x4000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG25_SECURE_SHFT                              0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG24_SECURE_BMSK                         0x2000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG24_SECURE_SHFT                              0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG23_SECURE_BMSK                         0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG23_SECURE_SHFT                              0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG22_SECURE_BMSK                          0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG22_SECURE_SHFT                              0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_PBL_LOG_DISABLE_BMSK                             0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_PBL_LOG_DISABLE_SHFT                                 0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_ROOT_CERT_TOTAL_NUM_BMSK                         0x3c0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_ROOT_CERT_TOTAL_NUM_SHFT                             0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG21_SECURE_BMSK                           0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG21_SECURE_SHFT                              0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG20_SECURE_BMSK                           0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG20_SECURE_SHFT                              0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG19_SECURE_BMSK                            0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG19_SECURE_SHFT                               0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG18_SECURE_BMSK                            0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_REG18_SECURE_SHFT                               0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_RSVD1_BMSK                                         0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_RSVD1_SHFT                                            0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_WDOG_EN_BMSK                                       0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_WDOG_EN_SHFT                                          0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPDM_SECURE_MODE_BMSK                               0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPDM_SECURE_MODE_SHFT                                 0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_ALT_SD_PORT_FOR_BOOT_BMSK                           0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_ALT_SD_PORT_FOR_BOOT_SHFT                             0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                  0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                    0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SDC_EMMC_MODE1P2_EN_BMSK                            0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SDC_EMMC_MODE1P2_EN_SHFT                              0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_FAST_BOOT_BMSK                                       0xe0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_FAST_BOOT_SHFT                                        0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SDCC_MCLK_BOOT_FREQ_BMSK                             0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SDCC_MCLK_BOOT_FREQ_SHFT                              0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_FORCE_DLOAD_DISABLE_BMSK                              0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_FORCE_DLOAD_DISABLE_SHFT                              0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_BMSK                                            0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_SPARE_SHFT                                            0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_ENUM_TIMEOUT_BMSK                                     0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_ENUM_TIMEOUT_SHFT                                     0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_E_DLOAD_DISABLE_BMSK                                  0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_V2_E_DLOAD_DISABLE_SHFT                                  0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004154)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004154)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_SPARE1_BMSK                                    0xff800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_SPARE1_SHFT                                          0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_ANTI_ROLLBACK_FEATURE_EN_BMSK                    0x780000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_ANTI_ROLLBACK_FEATURE_EN_SHFT                        0x13
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_DAP_DEVICEEN_DISABLE_BMSK                         0x40000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_DAP_DEVICEEN_DISABLE_SHFT                            0x12
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_DAP_SPNIDEN_DISABLE_BMSK                          0x20000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_DAP_SPNIDEN_DISABLE_SHFT                             0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_DAP_SPIDEN_DISABLE_BMSK                           0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_DAP_SPIDEN_DISABLE_SHFT                              0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_DAP_NIDEN_DISABLE_BMSK                             0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_DAP_NIDEN_DISABLE_SHFT                                0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_DAP_DBGEN_DISABLE_BMSK                             0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_DAP_DBGEN_DISABLE_SHFT                                0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_APPS_SPNIDEN_DISABLE_BMSK                          0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_APPS_SPNIDEN_DISABLE_SHFT                             0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_APPS_SPIDEN_DISABLE_BMSK                           0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_APPS_SPIDEN_DISABLE_SHFT                              0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_APPS_NIDEN_DISABLE_BMSK                             0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_APPS_NIDEN_DISABLE_SHFT                               0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_APPS_DBGEN_DISABLE_BMSK                             0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_APPS_DBGEN_DISABLE_SHFT                               0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_SPARE1_DISABLE_BMSK                                 0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_SPARE1_DISABLE_SHFT                                   0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_SPARE0_DISABLE_BMSK                                 0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_SPARE0_DISABLE_SHFT                                   0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_SPARE2_DISABLE_BMSK                                  0x80
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_SPARE2_DISABLE_SHFT                                   0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_RPM_DAPEN_DISABLE_BMSK                               0x40
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_RPM_DAPEN_DISABLE_SHFT                                0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_RPM_NIDEN_DISABLE_BMSK                               0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_RPM_NIDEN_DISABLE_SHFT                                0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_RPM_DBGEN_DISABLE_BMSK                               0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_RPM_DBGEN_DISABLE_SHFT                                0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_SPARE3_DISABLE_BMSK                                   0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_SPARE3_DISABLE_SHFT                                   0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_MSS_NIDEN_DISABLE_BMSK                                0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_MSS_NIDEN_DISABLE_SHFT                                0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_MSS_DBGEN_DISABLE_BMSK                                0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_MSS_DBGEN_DISABLE_SHFT                                0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_ALL_DEBUG_DISABLE_BMSK                                0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_V2_ALL_DEBUG_DISABLE_SHFT                                0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004158)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004158)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_OEM_PRODUCT_ID_BMSK                            0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_OEM_PRODUCT_ID_SHFT                                  0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_OEM_HW_ID_BMSK                                     0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_V2_OEM_HW_ID_SHFT                                        0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_ADDR                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000415c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_OFFS                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000415c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_PERIPH_VID_BMSK                                0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_PERIPH_VID_SHFT                                      0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_PERIPH_PID_BMSK                                    0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_V2_PERIPH_PID_SHFT                                       0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004160)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004160)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_VU_EN_BMSK                         0xfc000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_VU_EN_SHFT                               0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_NAV_EN_BMSK                               0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_NAV_EN_SHFT                                    0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_LDO_EN_BMSK                               0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_LDO_EN_SHFT                                    0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_ECS_EN_BMSK                                0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_ECS_EN_SHFT                                    0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_WCDMA_EN_BMSK                        0x600000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_WCDMA_EN_SHFT                            0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_TDSCDMA_EN_BMSK                      0x180000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_TDSCDMA_EN_SHFT                          0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_EN_BMSK                           0x60000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_EN_SHFT                              0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_ABOVE_CAT2_EN_BMSK                0x18000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_ABOVE_CAT2_EN_SHFT                    0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_ABOVE_CAT1_EN_BMSK                 0x6000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_LTE_ABOVE_CAT1_EN_SHFT                    0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_MIMO_EN_BMSK                      0x1800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_MIMO_EN_SHFT                         0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_EN_BMSK                            0x600
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_EN_SHFT                              0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_DC_EN_BMSK                         0x180
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_HSPA_DC_EN_SHFT                           0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_GENRAN_EN_BMSK                           0x60
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_GENRAN_EN_SHFT                            0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_EN_BMSK                                  0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_EN_SHFT                                   0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_DO_EN_BMSK                                0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_DO_EN_SHFT                                0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_1X_EN_BMSK                                0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_V2_MSMC_MODEM_1X_EN_SHFT                                0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004164)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004164)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK              0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                    0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_CM_FEAT_CONFIG_DISABLE_BMSK                   0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_CM_FEAT_CONFIG_DISABLE_SHFT                         0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_SPARE_BMSK                                    0x3ff00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_SPARE_SHFT                                          0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_MSMC_MDSP_FW_EN_BMSK                             0xfff00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_MSMC_MDSP_FW_EN_SHFT                                 0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_MSMC_SPARE_BMSK                                     0xc0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_MSMC_SPARE_SHFT                                      0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_MSMC_MODEM_VU_EN_BMSK                               0x3f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_V2_MSMC_MODEM_VU_EN_SHFT                                0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004168)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004168)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_SPARE1_BMSK                                   0xe0000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_SPARE1_SHFT                                         0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_SECURE_CHANNEL_DISABLE_BMSK                   0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_SECURE_CHANNEL_DISABLE_SHFT                         0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_FUSE_SMT_PERM_ENABLE_BMSK                      0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_FUSE_SMT_PERM_ENABLE_SHFT                           0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_FUSE_RCP_BYPASS_ENABLE_BMSK                    0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_FUSE_RCP_BYPASS_ENABLE_SHFT                         0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_MSMC_SMMU_BYPASS_DISABLE_BMSK                  0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_MSMC_SMMU_BYPASS_DISABLE_SHFT                       0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_MSMC_GPIO_SLIMBUS_PD_DISABLE_BMSK              0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_MSMC_GPIO_SLIMBUS_PD_DISABLE_SHFT                   0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_FUSE_PCIE20_RC_DISABLE_BMSK                     0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_FUSE_PCIE20_RC_DISABLE_SHFT                         0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_MSMC_PCIE_DISABLE_BMSK                          0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_MSMC_PCIE_DISABLE_SHFT                              0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_NDINT_DISABLE_BMSK                              0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_NDINT_DISABLE_SHFT                                  0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_DEVICEEN_DISABLE_BMSK                    0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_DEVICEEN_DISABLE_SHFT                        0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_SPNIDEN_DISABLE_BMSK                      0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_SPNIDEN_DISABLE_SHFT                         0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_SPIDEN_DISABLE_BMSK                       0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_SPIDEN_DISABLE_SHFT                          0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_NIDEN_DISABLE_BMSK                        0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_NIDEN_DISABLE_SHFT                           0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_DBGEN_DISABLE_BMSK                        0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_DAP_DBGEN_DISABLE_SHFT                           0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_SPNIDEN_DISABLE_BMSK                      0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_SPNIDEN_DISABLE_SHFT                         0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_SPIDEN_DISABLE_BMSK                       0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_SPIDEN_DISABLE_SHFT                          0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_NIDEN_DISABLE_BMSK                        0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_NIDEN_DISABLE_SHFT                           0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_DBGEN_DISABLE_BMSK                        0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_APPS_DBGEN_DISABLE_SHFT                           0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE1_DISABLE_BMSK                             0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE1_DISABLE_SHFT                               0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE0_DISABLE_BMSK                             0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE0_DISABLE_SHFT                               0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE2_DISABLE_BMSK                             0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE2_DISABLE_SHFT                               0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_DAPEN_DISABLE_BMSK                          0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_DAPEN_DISABLE_SHFT                            0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_NIDEN_DISABLE_BMSK                           0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_NIDEN_DISABLE_SHFT                            0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_DBGEN_DISABLE_BMSK                           0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_RPM_DBGEN_DISABLE_SHFT                            0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE3_DISABLE_BMSK                              0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_SPARE3_DISABLE_SHFT                               0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_MSS_NIDEN_DISABLE_BMSK                           0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_MSS_NIDEN_DISABLE_SHFT                            0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_MSS_DBGEN_DISABLE_BMSK                            0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QC_MSS_DBGEN_DISABLE_SHFT                            0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QDI_SPMI_DISABLE_BMSK                                0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_QDI_SPMI_DISABLE_SHFT                                0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_SM_BIST_DISABLE_BMSK                                 0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_SM_BIST_DISABLE_SHFT                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_TIC_DISABLE_BMSK                                     0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_V2_TIC_DISABLE_SHFT                                     0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000416c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000416c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_SEC_TAP_ACCESS_DISABLE_BMSK                   0xfe000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_SEC_TAP_ACCESS_DISABLE_SHFT                         0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_TAP_CJI_CORE_SEL_DISABLE_BMSK                  0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_TAP_CJI_CORE_SEL_DISABLE_SHFT                       0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_TAP_INSTR_DISABLE_BMSK                          0xfff800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_TAP_INSTR_DISABLE_SHFT                               0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_SPARE1_BMSK                                        0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_SPARE1_SHFT                                          0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_MODEM_PBL_PATCH_VERSION_BMSK                       0x3e0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_MODEM_PBL_PATCH_VERSION_SHFT                         0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_APPS_PBL_PATCH_VERSION_BMSK                         0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_V2_APPS_PBL_PATCH_VERSION_SHFT                          0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004170)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004170)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_V2_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_V2_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK         0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_V2_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004174)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004174)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_STACKED_MEMORY_ID_BMSK                        0xf0000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_STACKED_MEMORY_ID_SHFT                              0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APSS_BOOT_TRIGGER_DISABLE_BMSK                 0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APSS_BOOT_TRIGGER_DISABLE_SHFT                      0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_MODEM_PBL_PLL_CTRL_BMSK                        0x7800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_MODEM_PBL_PLL_CTRL_SHFT                             0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APPS_PBL_PLL_CTRL_BMSK                          0x780000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APPS_PBL_PLL_CTRL_SHFT                              0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FSM_CFG_BMSK                           0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FSM_CFG_SHFT                              0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FSM_DELAY_BMSK                         0x30000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FSM_DELAY_SHFT                            0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_BMSK           0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_SHFT              0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_USB_SS_ENABLE_BMSK                                0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_USB_SS_ENABLE_SHFT                                   0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APPS_PBL_BOOT_SPEED_BMSK                          0x3000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APPS_PBL_BOOT_SPEED_SHFT                             0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FROM_ROM_BMSK                            0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_APPS_BOOT_FROM_ROM_SHFT                              0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_MSA_ENA_BMSK                                       0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_MSA_ENA_SHFT                                         0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_FORCE_MSA_AUTH_EN_BMSK                             0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_FORCE_MSA_AUTH_EN_SHFT                               0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_PCIE_REFCLK_TERM_ENABLE_BMSK                       0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_PCIE_REFCLK_TERM_ENABLE_SHFT                         0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_MSS_HASH_INTEGRITY_CHECK_DISABLE_BMSK               0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_MSS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_MODEM_BOOT_FROM_ROM_BMSK                            0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_MODEM_BOOT_FROM_ROM_SHFT                             0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_BOOT_ROM_PATCH_DISABLE_BMSK                         0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_BOOT_ROM_PATCH_DISABLE_SHFT                          0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK              0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_V2_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT               0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_V2_ADDR                                       (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004178)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_V2_OFFS                                       (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004178)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_V2_RMSK                                       0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_V2_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_V2_RSVD0_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_V2_RSVD0_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_V2_ADDR                                       (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000417c)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_V2_OFFS                                       (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000417c)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_V2_RMSK                                       0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_V2_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_V2_RSVD0_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_V2_RSVD0_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_V2_ADDR                                       (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004180)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_V2_OFFS                                       (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004180)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_V2_RMSK                                       0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_V2_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_V2_RSVD0_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_V2_RSVD0_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_V2_ADDR                                       (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004184)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_V2_OFFS                                       (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004184)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_V2_RMSK                                       0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_V2_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_V2_RSVD0_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_V2_RSVD0_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_V2_ADDR                                       (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004188)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_V2_OFFS                                       (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004188)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_V2_RMSK                                       0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_V2_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_V2_RSVD0_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_V2_RSVD0_SHFT                                        0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_V2_ADDR                                       (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000418c)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_V2_OFFS                                       (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000418c)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_V2_RMSK                                       0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_V2_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_V2_RSVD0_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_V2_RSVD0_SHFT                                        0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n)                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004190 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_OFFS(n)                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004190 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_RMSK                               0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_MAXn                                        3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_KEY_DATA0_BMSK                     0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_V2_KEY_DATA0_SHFT                            0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n)                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004194 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_OFFS(n)                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004194 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_RMSK                               0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_MAXn                                        3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_KEY_DATA1_BMSK                     0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_V2_KEY_DATA1_SHFT                            0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n)                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000041b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_OFFS(n)                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000041b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_RMSK                               0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_MAXn                                        3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_KEY_DATA0_BMSK                     0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_V2_KEY_DATA0_SHFT                            0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n)                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000041b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_OFFS(n)                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000041b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_RMSK                               0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_MAXn                                        3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_KEY_DATA1_BMSK                     0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_V2_KEY_DATA1_SHFT                            0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_ADDR                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000041d0)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_OFFS                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000041d0)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_RMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_ADDR, HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT4_BMSK                               0xff000000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT4_SHFT                                     0x18
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT3_BMSK                                 0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT3_SHFT                                     0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT2_BMSK                                   0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT2_SHFT                                      0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT1_BMSK                                     0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_V2_SEC_BOOT1_SHFT                                      0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_ADDR                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000041d4)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_OFFS                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000041d4)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_RMSK                                           0xffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_ADDR, HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT7_BMSK                                 0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT7_SHFT                                     0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT6_BMSK                                   0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT6_SHFT                                      0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT5_BMSK                                     0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_MSB_V2_SEC_BOOT5_SHFT                                      0x0

#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000041d8)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000041d8)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_CALIB2_ROW0_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000041dc)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000041dc)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_CALIB2_ROW0_MSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_ADDR(n)                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000041e0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_OFFS(n)                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000041e0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_MAXn                                                       3
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_ADDR(n), HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_HASH_DATA0_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_LSB_V2_HASH_DATA0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_ADDR(n)                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000041e4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_OFFS(n)                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000041e4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_MAXn                                                       3
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_ADDR(n), HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_SPARE0_BMSK                                       0x80000000
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_SPARE0_SHFT                                             0x1f
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_FEC_VALUE_BMSK                                    0x7f000000
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_FEC_VALUE_SHFT                                          0x18
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_HASH_DATA1_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROWn_MSB_V2_HASH_DATA1_SHFT                                          0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_V2_ADDR                                              (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004200)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_V2_OFFS                                              (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004200)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_V2_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_V2_ADDR, HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_V2_HASH_DATA0_BMSK                                   0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_LSB_V2_HASH_DATA0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_ADDR                                              (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004204)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_OFFS                                              (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004204)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_ADDR, HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_SPARE0_BMSK                                       0x80000000
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_SPARE0_SHFT                                             0x1f
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_FEC_VALUE_BMSK                                    0x7f000000
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_FEC_VALUE_SHFT                                          0x18
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_UNUSED_BMSK                                         0xffffff
#define HWIO_QFPROM_CORR_PK_HASH_ROW4_MSB_V2_UNUSED_SHFT                                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004208)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004208)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_TSENS2_POINT1_BMSK                                  0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_TSENS2_POINT1_SHFT                                        0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_TSENS1_POINT1_BMSK                                   0xf000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_TSENS1_POINT1_SHFT                                        0x18
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_TSENS0_POINT1_BMSK                                    0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_TSENS0_POINT1_SHFT                                        0x14
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_TSENS_BASE1_BMSK                                       0xffc00
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_TSENS_BASE1_SHFT                                           0xa
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_TSENS_BASE0_BMSK                                         0x3ff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_V2_TSENS_BASE0_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000420c)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000420c)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_FEC_BMSK                                            0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_FEC_SHFT                                                  0x18
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_USB_PHY_TUNING_BMSK                                   0xf00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_USB_PHY_TUNING_SHFT                                       0x14
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_SPARE_BMSK                                             0xf0000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_SPARE_SHFT                                                0x10
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_Q6SS0_LDO_VREF_TRIM_BMSK                                0xf800
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_Q6SS0_LDO_VREF_TRIM_SHFT                                   0xb
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_TSENS_CAL_SEL_BMSK                                       0x700
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_TSENS_CAL_SEL_SHFT                                         0x8
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_TSENS4_POINT1_BMSK                                        0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_TSENS4_POINT1_SHFT                                         0x4
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_TSENS3_POINT1_BMSK                                         0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_V2_TSENS3_POINT1_SHFT                                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004210)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004210)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_3_BMSK                0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_3_SHFT                      0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_SVS2_BMSK                            0x7c000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_SVS2_SHFT                                  0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_2_BMSK                 0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_2_SHFT                      0x19
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_SVS_BMSK                              0x1f00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_SVS_SHFT                                   0x14
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_1_BMSK                   0x80000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_1_SHFT                      0x13
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_NOM_BMSK                                0x7c000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_NOM_SHFT                                    0xe
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_0_BMSK                    0x2000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_OFFSET_TUR_BIT_0_SHFT                       0xd
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_TUR_BMSK                                 0x1f00
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR0_TARG_VOLT_TUR_SHFT                                    0x8
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_BMSK                           0x80
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_SHFT                            0x7
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR2_TARG_VOLT_OFFSET_NOM_BMSK                            0x78
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR2_TARG_VOLT_OFFSET_NOM_SHFT                             0x3
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR2_TARG_VOLT_OFFSET_TUR_BMSK                             0x7
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_V2_CPR2_TARG_VOLT_OFFSET_TUR_SHFT                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004214)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004214)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_FEC_BMSK                                            0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_FEC_SHFT                                                  0x18
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_3_BMSK                  0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_3_SHFT                      0x17
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_SVS2_BMSK                              0x7c0000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_SVS2_SHFT                                  0x12
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_2_BMSK                   0x20000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_2_SHFT                      0x11
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_SVS_BMSK                                0x1f000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_SVS_SHFT                                    0xc
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_1_BMSK                     0x800
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_1_SHFT                       0xb
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_NOM_BMSK                                  0x7c0
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_NOM_SHFT                                    0x6
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_0_BMSK                      0x20
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR0_TARG_VOLT_OFFSET_NOM_BIT_0_SHFT                       0x5
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_TUR_BMSK                                   0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_V2_CPR1_TARG_VOLT_TUR_SHFT                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004218)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004218)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_LDO_TARGET1_BMSK                                    0xf0000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_LDO_TARGET1_SHFT                                          0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_LDO_TARGET0_BMSK                                     0xfc00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_LDO_TARGET0_SHFT                                          0x16
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_LDO_ENABLE_BMSK                                       0x200000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_LDO_ENABLE_SHFT                                           0x15
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS2_BMSK                       0x1e0000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS2_SHFT                           0x11
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS_BMSK                         0x1c000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS_SHFT                             0xe
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR2_TARG_VOLT_NOM_BMSK                                 0x3e00
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR2_TARG_VOLT_NOM_SHFT                                    0x9
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS_BIT_3_BMSK                     0x100
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR0_TARG_VOLT_OFFSET_SVS_BIT_3_SHFT                       0x8
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR2_TARG_VOLT_TUR_BMSK                                   0xf8
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR2_TARG_VOLT_TUR_SHFT                                    0x3
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR_REV_BMSK                                               0x7
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_V2_CPR_REV_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000421c)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000421c)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_FEC_BMSK                                            0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_FEC_SHFT                                                  0x18
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_BMSK                       0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_SHFT                           0x16
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS_BMSK                        0x3c0000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS_SHFT                            0x12
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_NOM_BMSK                         0x3c000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_NOM_SHFT                             0xe
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_TUR_BMSK                          0x3c00
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_CPR1_TARG_VOLT_OFFSET_TUR_SHFT                             0xa
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_SPARE_BMSK                                               0x300
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_SPARE_SHFT                                                 0x8
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_VCM_BMSK                                     0xc0
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_VCM_SHFT                                      0x6
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_LDO_BMSK                                     0x30
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_LDO_SHFT                                      0x4
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_VREF_BMSK                                     0xc
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_GNSS_ADC_CH0_VREF_SHFT                                     0x2
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_LDO_ENABLE_SVS2_BMSK                                       0x2
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_LDO_ENABLE_SVS2_SHFT                                       0x1
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_LDO_TARGET1_BMSK                                           0x1
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_V2_LDO_TARGET1_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004220)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004220)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ECS_BHS_SLOPE_BMSK                                  0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ECS_BHS_SLOPE_SHFT                                        0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ECS_BHS_INTERCEPT_BMSK                              0x7fe00000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ECS_BHS_INTERCEPT_SHFT                                    0x15
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ECS_LDO_SLOPE_BMSK                                    0x1ff800
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ECS_LDO_SLOPE_SHFT                                         0xb
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ECS_LDO_INTERCEPT_BMSK                                   0x7fe
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ECS_LDO_INTERCEPT_SHFT                                     0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ECS_ENABLE_BMSK                                            0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_V2_ECS_ENABLE_SHFT                                            0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004224)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004224)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_FEC_BMSK                                            0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_FEC_SHFT                                                  0x18
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_G_B1_BMSK                                             0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_G_B1_SHFT                                                 0x15
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_SAR_LDO_ERR0_BMSK                                     0x180000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_SAR_LDO_ERR0_SHFT                                         0x13
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_CPR2_TARG_VOLT_OFFSET_TUR_BIT_3_BMSK                   0x40000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_CPR2_TARG_VOLT_OFFSET_TUR_BIT_3_SHFT                      0x12
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_BIT_3_BMSK                  0x20000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_CPR1_TARG_VOLT_OFFSET_SVS2_BIT_3_SHFT                     0x11
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_ECS_MISC_BMSK                                          0x1fe00
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_ECS_MISC_SHFT                                              0x9
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_ECS_BHS_SLOPE_BMSK                                       0x1ff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_V2_ECS_BHS_SLOPE_SHFT                                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004228)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004228)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B1M2_BMSK                                        0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B1M2_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B1M1_BMSK                                        0x38000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B1M1_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B1M0_BMSK                                         0x7000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B1M0_SHFT                                              0x18
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_SPARE0_BMSK                                           0xf80000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_SPARE0_SHFT                                               0x13
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_CLK_LDO_ERR0_BMSK                                      0x60000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_CLK_LDO_ERR0_SHFT                                         0x11
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_VREF_ERR_B0_BMSK                                       0x18000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_VREF_ERR_B0_SHFT                                           0xf
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B0M3_BMSK                                            0x7000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B0M3_SHFT                                               0xc
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B0M2_BMSK                                             0xe00
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B0M2_SHFT                                               0x9
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B0M1_BMSK                                             0x1c0
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B0M1_SHFT                                               0x6
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B0M0_BMSK                                              0x38
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_PH_B0M0_SHFT                                               0x3
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_G_B0_BMSK                                                  0x7
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_V2_G_B0_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000422c)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000422c)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_FEC_BMSK                                            0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_FEC_SHFT                                                  0x18
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B2M3_BMSK                                          0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B2M3_SHFT                                              0x16
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B2M2_BMSK                                          0x380000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B2M2_SHFT                                              0x13
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B2M1_BMSK                                           0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B2M1_SHFT                                              0x10
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B2M0_BMSK                                            0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B2M0_SHFT                                               0xd
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_G_B2_BMSK                                               0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_G_B2_SHFT                                                  0xa
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_SAR_LDO_ERR1_BMSK                                        0x300
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_SAR_LDO_ERR1_SHFT                                          0x8
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_CLK_LDO_ERR1_BMSK                                         0xc0
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_CLK_LDO_ERR1_SHFT                                          0x6
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_VREF_ERR_B1_BMSK                                          0x30
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_VREF_ERR_B1_SHFT                                           0x4
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B1M3_BMSK                                               0xe
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B1M3_SHFT                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B1M2_BMSK                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_V2_PH_B1M2_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004230)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004230)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B4M0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B4M0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_G_B4_BMSK                                           0x70000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_G_B4_SHFT                                                 0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_SAR_LDO_ERR3_BMSK                                    0xc000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_SAR_LDO_ERR3_SHFT                                         0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_CLK_LDO_ERR3_BMSK                                    0x3000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_CLK_LDO_ERR3_SHFT                                         0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_VREF_ERR_B3_BMSK                                      0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_VREF_ERR_B3_SHFT                                          0x16
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B3M3_BMSK                                          0x380000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B3M3_SHFT                                              0x13
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B3M2_BMSK                                           0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B3M2_SHFT                                              0x10
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B3M1_BMSK                                            0xe000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B3M1_SHFT                                               0xd
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B3M0_BMSK                                            0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B3M0_SHFT                                               0xa
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_G_B3_BMSK                                                0x380
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_G_B3_SHFT                                                  0x7
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_SAR_LDO_ERR2_BMSK                                         0x60
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_SAR_LDO_ERR2_SHFT                                          0x5
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_CLK_LDO_ERR2_BMSK                                         0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_CLK_LDO_ERR2_SHFT                                          0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_VREF_ERR_B2_BMSK                                           0x6
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_VREF_ERR_B2_SHFT                                           0x1
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B2M3_BMSK                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_V2_PH_B2M3_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004234)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004234)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_FEC_VALUE_BMSK                                      0x7f000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_FEC_VALUE_SHFT                                            0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B5M1_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B5M1_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B5M0_BMSK                                          0x700000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B5M0_SHFT                                              0x14
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_G_B5_BMSK                                              0xe0000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_G_B5_SHFT                                                 0x11
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_SAR_LDO_ERR4_BMSK                                      0x18000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_SAR_LDO_ERR4_SHFT                                          0xf
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_CLK_LDO_ERR4_BMSK                                       0x6000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_CLK_LDO_ERR4_SHFT                                          0xd
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_VREF_ERR_B4_BMSK                                        0x1800
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_VREF_ERR_B4_SHFT                                           0xb
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B4M3_BMSK                                             0x700
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B4M3_SHFT                                               0x8
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B4M2_BMSK                                              0xe0
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B4M2_SHFT                                               0x5
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B4M1_BMSK                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B4M1_SHFT                                               0x2
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B4M0_BMSK                                               0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_V2_PH_B4M0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004238)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004238)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_CLK_LDO_ERR6_BMSK                                   0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_CLK_LDO_ERR6_SHFT                                         0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_VREF_ERR_B6_BMSK                                    0x60000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_VREF_ERR_B6_SHFT                                          0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B6M3_BMSK                                        0x1c000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B6M3_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B6M2_BMSK                                         0x3800000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B6M2_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B6M1_BMSK                                          0x700000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B6M1_SHFT                                              0x14
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B6M0_BMSK                                           0xe0000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B6M0_SHFT                                              0x11
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_G_B6_BMSK                                              0x1c000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_G_B6_SHFT                                                  0xe
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_SAR_LDO_ERR5_BMSK                                       0x3000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_SAR_LDO_ERR5_SHFT                                          0xc
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_CLK_LDO_ERR5_BMSK                                        0xc00
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_CLK_LDO_ERR5_SHFT                                          0xa
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_VREF_ERR_B5_BMSK                                         0x300
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_VREF_ERR_B5_SHFT                                           0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B5M3_BMSK                                              0xe0
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B5M3_SHFT                                               0x5
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B5M2_BMSK                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B5M2_SHFT                                               0x2
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B5M1_BMSK                                               0x3
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_V2_PH_B5M1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000423c)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000423c)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_FEC_BMSK                                            0xff000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_FEC_SHFT                                                  0x18
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_BMSK                               0xff0000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_SHFT                                   0x10
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_BMSK                                 0xff00
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_SHFT                                    0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_AVG_ERROR_BMSK                         0x80
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_AVG_ERROR_SHFT                          0x7
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_AVG_ERROR_BMSK                         0x40
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_AVG_ERROR_SHFT                          0x6
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC_0_1_FUSEFLAG_BMSK                             0x20
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC_0_1_FUSEFLAG_SHFT                              0x5
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_OVERFLOW_BMSK                          0x10
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC1_CALIB_OVERFLOW_SHFT                           0x4
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_OVERFLOW_BMSK                           0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_MODEM_TXDAC0_CALIB_OVERFLOW_SHFT                           0x3
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_SAR_LDO_ERR6_BMSK                                          0x6
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_SAR_LDO_ERR6_SHFT                                          0x1
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_CLK_LDO_ERR6_BMSK                                          0x1
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_V2_CLK_LDO_ERR6_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004240)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004240)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW7_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004244)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004244)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW7_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004248)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004248)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW8_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000424c)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000424c)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW8_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004250)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004250)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW9_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_V2_ADDR                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004254)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_V2_OFFS                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004254)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_V2_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW9_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_V2_SPARE0_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_V2_SPARE0_SHFT                                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004258)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004258)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW10_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000425c)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000425c)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW10_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004260)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004260)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW11_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004264)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004264)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_V2_ADDR, HWIO_QFPROM_CORR_CALIB_ROW11_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_ADDR(n)                                        (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004268 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_OFFS(n)                                        (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004268 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_MAXn                                                   13
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_REDUN_DATA_BMSK                                0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_V2_REDUN_DATA_SHFT                                       0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_ADDR(n)                                        (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000426c + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_OFFS(n)                                        (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000426c + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_RMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_MAXn                                                   13
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_SPARE0_BMSK                                    0x80000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_SPARE0_SHFT                                          0x1f
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_FEC_VALUE_BMSK                                 0x7f000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_FEC_VALUE_SHFT                                       0x18
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_REDUN_DATA_BMSK                                  0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_V2_REDUN_DATA_SHFT                                       0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_LSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000042d8)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_LSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000042d8)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_LSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_LSB_V2_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_LSB_V2_MEM_ACCEL_COMPILER_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_LSB_V2_MEM_ACCEL_COMPILER_SHFT                              0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000042dc)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000042dc)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_SPARE_BMSK                                    0x80000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_SPARE_SHFT                                          0x1f
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_FEC_VALUE_BMSK                                0x7f000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_FEC_VALUE_SHFT                                      0x18
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_SPARE0_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW14_MSB_V2_SPARE0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_LSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000042e0)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_LSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000042e0)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_LSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_LSB_V2_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_LSB_V2_MEM_ACCEL_CUSTOM_BMSK                         0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_LSB_V2_MEM_ACCEL_CUSTOM_SHFT                                0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000042e4)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000042e4)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_SPARE_BMSK                                    0x80000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_SPARE_SHFT                                          0x1f
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_FEC_VALUE_BMSK                                0x7f000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_FEC_VALUE_SHFT                                      0x18
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_SPARE0_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW15_MSB_V2_SPARE0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000042e8)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000042e8)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_V2_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_V2_MEM_ACCEL_CUSTOM_BMSK                         0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_LSB_V2_MEM_ACCEL_CUSTOM_SHFT                                0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000042ec)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000042ec)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_SPARE_BMSK                                    0x80000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_SPARE_SHFT                                          0x1f
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_FEC_VALUE_BMSK                                0x7f000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_FEC_VALUE_SHFT                                      0x18
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_SPARE0_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW16_MSB_V2_SPARE0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000042f0)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000042f0)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_V2_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_V2_MEM_ACCEL_COMPILER_BMSK                       0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_LSB_V2_MEM_ACCEL_COMPILER_SHFT                              0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000042f4)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000042f4)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_SPARE_BMSK                                    0x80000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_SPARE_SHFT                                          0x1f
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_FEC_VALUE_BMSK                                0x7f000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_FEC_VALUE_SHFT                                      0x18
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_SPARE0_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW17_MSB_V2_SPARE0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000042f8)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000042f8)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_V2_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_V2_MEM_ACCEL_CUSTOM_BMSK                         0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_LSB_V2_MEM_ACCEL_CUSTOM_SHFT                                0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000042fc)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000042fc)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_ADDR, HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_SPARE_BMSK                                    0x80000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_SPARE_SHFT                                          0x1f
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_FEC_VALUE_BMSK                                0x7f000000
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_FEC_VALUE_SHFT                                      0x18
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_SPARE0_BMSK                                     0xffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROW18_MSB_V2_SPARE0_SHFT                                          0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_ADDR(n)                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004300 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_OFFS(n)                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004300 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_MAXn                                                    31
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_PATCH_DATA_BMSK                                 0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_V2_PATCH_DATA_SHFT                                        0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_ADDR(n)                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004304 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_OFFS(n)                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004304 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_MAXn                                                    31
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_SPARE0_BMSK                                     0x80000000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_SPARE0_SHFT                                           0x1f
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_FEC_VALUE_BMSK                                  0x7f000000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_FEC_VALUE_SHFT                                        0x18
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_SPARE3_BMSK                                       0xfe0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_SPARE3_SHFT                                           0x11
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_PATCH_ADDR_BMSK                                    0x1fffe
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_PATCH_ADDR_SHFT                                        0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_PATCH_EN_BMSK                                          0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_V2_PATCH_EN_SHFT                                          0x0

#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_ADDR                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004400)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_OFFS                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004400)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_V2_ADDR, HWIO_QFPROM_CORR_FEC_EN_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_RSVD0_BMSK                                              0xf8000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_RSVD0_SHFT                                                    0x1b
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION26_FEC_EN_BMSK                                     0x4000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION26_FEC_EN_SHFT                                          0x1a
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION25_FEC_EN_BMSK                                     0x2000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION25_FEC_EN_SHFT                                          0x19
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION24_FEC_EN_BMSK                                     0x1000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION24_FEC_EN_SHFT                                          0x18
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION23_FEC_EN_BMSK                                      0x800000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION23_FEC_EN_SHFT                                          0x17
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION22_FEC_EN_BMSK                                      0x400000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION22_FEC_EN_SHFT                                          0x16
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION21_FEC_EN_BMSK                                      0x200000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION21_FEC_EN_SHFT                                          0x15
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION20_FEC_EN_BMSK                                      0x100000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION20_FEC_EN_SHFT                                          0x14
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION19_FEC_EN_BMSK                                       0x80000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION19_FEC_EN_SHFT                                          0x13
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION18_FEC_EN_BMSK                                       0x40000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION18_FEC_EN_SHFT                                          0x12
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION17_FEC_EN_BMSK                                       0x20000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION17_FEC_EN_SHFT                                          0x11
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION16_FEC_EN_BMSK                                       0x10000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION16_FEC_EN_SHFT                                          0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION15_FEC_EN_BMSK                                        0x8000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION15_FEC_EN_SHFT                                           0xf
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION14_FEC_EN_BMSK                                        0x4000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION14_FEC_EN_SHFT                                           0xe
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION13_FEC_EN_BMSK                                        0x2000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION13_FEC_EN_SHFT                                           0xd
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION12_FEC_EN_BMSK                                        0x1000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION12_FEC_EN_SHFT                                           0xc
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION11_FEC_EN_BMSK                                         0x800
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION11_FEC_EN_SHFT                                           0xb
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION10_FEC_EN_BMSK                                         0x400
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION10_FEC_EN_SHFT                                           0xa
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION9_FEC_EN_BMSK                                          0x200
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION9_FEC_EN_SHFT                                            0x9
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION8_FEC_EN_BMSK                                          0x100
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION8_FEC_EN_SHFT                                            0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION7_FEC_EN_BMSK                                           0x80
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION7_FEC_EN_SHFT                                            0x7
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION6_FEC_EN_BMSK                                           0x40
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION6_FEC_EN_SHFT                                            0x6
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION5_FEC_EN_BMSK                                           0x20
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION5_FEC_EN_SHFT                                            0x5
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION4_FEC_EN_BMSK                                           0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION4_FEC_EN_SHFT                                            0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION3_FEC_EN_BMSK                                            0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION3_FEC_EN_SHFT                                            0x3
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION2_FEC_EN_BMSK                                            0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION2_FEC_EN_SHFT                                            0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION1_FEC_EN_BMSK                                            0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION1_FEC_EN_SHFT                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION0_FEC_EN_BMSK                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_V2_REGION0_FEC_EN_SHFT                                            0x0

#define HWIO_QFPROM_CORR_FEC_EN_MSB_V2_ADDR                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004404)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_V2_OFFS                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004404)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_V2_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_V2_ADDR, HWIO_QFPROM_CORR_FEC_EN_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_V2_FEC_EN_REDUNDANCY_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_V2_FEC_EN_REDUNDANCY_SHFT                                         0x0

#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004408)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004408)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_LSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG18_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000440c)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000440c)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_IN)
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_SPARE1_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG18_MSB_V2_SPARE1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004410)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004410)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004414)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004414)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_IN)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_SPARE1_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_V2_SPARE1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004418)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004418)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_LSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG20_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000441c)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000441c)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_ADDR,v)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_ADDR,m,v,HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_IN)
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_SPARE1_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG20_MSB_V2_SPARE1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004420)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004420)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_LSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG21_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004424)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004424)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_SPARE1_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG21_MSB_V2_SPARE1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004428)
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004428)
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG22_LSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG22_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG22_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG22_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000442c)
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000442c)
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_SPARE1_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG22_MSB_V2_SPARE1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004430)
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004430)
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG23_LSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG23_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG23_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG23_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004434)
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004434)
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_SPARE1_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG23_MSB_V2_SPARE1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004438)
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004438)
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG24_LSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG24_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG24_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG24_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000443c)
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000443c)
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_SPARE1_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG24_MSB_V2_SPARE1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004440)
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004440)
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG25_LSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG25_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG25_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG25_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004444)
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004444)
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_SPARE1_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG25_MSB_V2_SPARE1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004448)
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004448)
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG26_LSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG26_LSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG26_LSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_V2_SPARE0_BMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG26_LSB_V2_SPARE0_SHFT                                               0x0

#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_ADDR                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000444c)
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_OFFS                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000444c)
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_ADDR, HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_SPARE1_BMSK                                          0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG26_MSB_V2_SPARE1_SHFT                                               0x0

#define HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_ADDR(n)                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00004800 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_OFFS(n)                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00004800 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_MAXn                                                          39
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_ADDR(n), HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_RMSK)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_ACC_PRIVATE_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_V2_ACC_PRIVATE_SHFT                                             0x0

#define HWIO_SEC_CTRL_HW_VERSION_V2_ADDR                                                       (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_V2_OFFS                                                       (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_V2_RMSK                                                       0xffffffff
#define HWIO_SEC_CTRL_HW_VERSION_V2_IN          \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_V2_ADDR, HWIO_SEC_CTRL_HW_VERSION_V2_RMSK)
#define HWIO_SEC_CTRL_HW_VERSION_V2_INM(m)      \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_V2_ADDR, m)
#define HWIO_SEC_CTRL_HW_VERSION_V2_MAJOR_BMSK                                                 0xf0000000
#define HWIO_SEC_CTRL_HW_VERSION_V2_MAJOR_SHFT                                                       0x1c
#define HWIO_SEC_CTRL_HW_VERSION_V2_MINOR_BMSK                                                  0xfff0000
#define HWIO_SEC_CTRL_HW_VERSION_V2_MINOR_SHFT                                                       0x10
#define HWIO_SEC_CTRL_HW_VERSION_V2_STEP_BMSK                                                      0xffff
#define HWIO_SEC_CTRL_HW_VERSION_V2_STEP_SHFT                                                         0x0

#define HWIO_FEATURE_CONFIG0_V2_ADDR                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006004)
#define HWIO_FEATURE_CONFIG0_V2_OFFS                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006004)
#define HWIO_FEATURE_CONFIG0_V2_RMSK                                                           0xffffffff
#define HWIO_FEATURE_CONFIG0_V2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG0_V2_ADDR, HWIO_FEATURE_CONFIG0_V2_RMSK)
#define HWIO_FEATURE_CONFIG0_V2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG0_V2_ADDR, m)
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_VU_EN_BMSK                                          0xfc000000
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_VU_EN_SHFT                                                0x1a
#define HWIO_FEATURE_CONFIG0_V2_MSMC_NAV_EN_BMSK                                                0x2000000
#define HWIO_FEATURE_CONFIG0_V2_MSMC_NAV_EN_SHFT                                                     0x19
#define HWIO_FEATURE_CONFIG0_V2_MSMC_LDO_EN_BMSK                                                0x1000000
#define HWIO_FEATURE_CONFIG0_V2_MSMC_LDO_EN_SHFT                                                     0x18
#define HWIO_FEATURE_CONFIG0_V2_MSMC_ECS_EN_BMSK                                                 0x800000
#define HWIO_FEATURE_CONFIG0_V2_MSMC_ECS_EN_SHFT                                                     0x17
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_WCDMA_EN_BMSK                                         0x600000
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_WCDMA_EN_SHFT                                             0x15
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_TDSCDMA_EN_BMSK                                       0x180000
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_TDSCDMA_EN_SHFT                                           0x13
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_LTE_EN_BMSK                                            0x60000
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_LTE_EN_SHFT                                               0x11
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_LTE_ABOVE_CAT2_EN_BMSK                                 0x18000
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_LTE_ABOVE_CAT2_EN_SHFT                                     0xf
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_LTE_ABOVE_CAT1_EN_BMSK                                  0x6000
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_LTE_ABOVE_CAT1_EN_SHFT                                     0xd
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_HSPA_MIMO_EN_BMSK                                       0x1800
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_HSPA_MIMO_EN_SHFT                                          0xb
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_HSPA_EN_BMSK                                             0x600
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_HSPA_EN_SHFT                                               0x9
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_HSPA_DC_EN_BMSK                                          0x180
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_HSPA_DC_EN_SHFT                                            0x7
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_GENRAN_EN_BMSK                                            0x60
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_GENRAN_EN_SHFT                                             0x5
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_EN_BMSK                                                   0x10
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_EN_SHFT                                                    0x4
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_DO_EN_BMSK                                                 0xc
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_DO_EN_SHFT                                                 0x2
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_1X_EN_BMSK                                                 0x3
#define HWIO_FEATURE_CONFIG0_V2_MSMC_MODEM_1X_EN_SHFT                                                 0x0

#define HWIO_FEATURE_CONFIG1_V2_ADDR                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006008)
#define HWIO_FEATURE_CONFIG1_V2_OFFS                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006008)
#define HWIO_FEATURE_CONFIG1_V2_RMSK                                                           0xffffffff
#define HWIO_FEATURE_CONFIG1_V2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG1_V2_ADDR, HWIO_FEATURE_CONFIG1_V2_RMSK)
#define HWIO_FEATURE_CONFIG1_V2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_V2_ADDR, m)
#define HWIO_FEATURE_CONFIG1_V2_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                               0x80000000
#define HWIO_FEATURE_CONFIG1_V2_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                     0x1f
#define HWIO_FEATURE_CONFIG1_V2_CM_FEAT_CONFIG_DISABLE_BMSK                                    0x40000000
#define HWIO_FEATURE_CONFIG1_V2_CM_FEAT_CONFIG_DISABLE_SHFT                                          0x1e
#define HWIO_FEATURE_CONFIG1_V2_SPARE_BMSK                                                     0x3ff00000
#define HWIO_FEATURE_CONFIG1_V2_SPARE_SHFT                                                           0x14
#define HWIO_FEATURE_CONFIG1_V2_MSMC_MDSP_FW_EN_BMSK                                              0xfff00
#define HWIO_FEATURE_CONFIG1_V2_MSMC_MDSP_FW_EN_SHFT                                                  0x8
#define HWIO_FEATURE_CONFIG1_V2_MSMC_SPARE_BMSK                                                      0xc0
#define HWIO_FEATURE_CONFIG1_V2_MSMC_SPARE_SHFT                                                       0x6
#define HWIO_FEATURE_CONFIG1_V2_MSMC_MODEM_VU_EN_BMSK                                                0x3f
#define HWIO_FEATURE_CONFIG1_V2_MSMC_MODEM_VU_EN_SHFT                                                 0x0

#define HWIO_FEATURE_CONFIG2_V2_ADDR                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_V2_OFFS                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_V2_RMSK                                                           0xffffffff
#define HWIO_FEATURE_CONFIG2_V2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG2_V2_ADDR, HWIO_FEATURE_CONFIG2_V2_RMSK)
#define HWIO_FEATURE_CONFIG2_V2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_V2_ADDR, m)
#define HWIO_FEATURE_CONFIG2_V2_SPARE1_BMSK                                                    0xe0000000
#define HWIO_FEATURE_CONFIG2_V2_SPARE1_SHFT                                                          0x1d
#define HWIO_FEATURE_CONFIG2_V2_SECURE_CHANNEL_DISABLE_BMSK                                    0x10000000
#define HWIO_FEATURE_CONFIG2_V2_SECURE_CHANNEL_DISABLE_SHFT                                          0x1c
#define HWIO_FEATURE_CONFIG2_V2_FUSE_SMT_PERM_ENABLE_BMSK                                       0x8000000
#define HWIO_FEATURE_CONFIG2_V2_FUSE_SMT_PERM_ENABLE_SHFT                                            0x1b
#define HWIO_FEATURE_CONFIG2_V2_FUSE_RCP_BYPASS_ENABLE_BMSK                                     0x4000000
#define HWIO_FEATURE_CONFIG2_V2_FUSE_RCP_BYPASS_ENABLE_SHFT                                          0x1a
#define HWIO_FEATURE_CONFIG2_V2_MSMC_SMMU_BYPASS_DISABLE_BMSK                                   0x2000000
#define HWIO_FEATURE_CONFIG2_V2_MSMC_SMMU_BYPASS_DISABLE_SHFT                                        0x19
#define HWIO_FEATURE_CONFIG2_V2_MSMC_GPIO_SLIMBUS_PD_DISABLE_BMSK                               0x1000000
#define HWIO_FEATURE_CONFIG2_V2_MSMC_GPIO_SLIMBUS_PD_DISABLE_SHFT                                    0x18
#define HWIO_FEATURE_CONFIG2_V2_FUSE_PCIE20_RC_DISABLE_BMSK                                      0x800000
#define HWIO_FEATURE_CONFIG2_V2_FUSE_PCIE20_RC_DISABLE_SHFT                                          0x17
#define HWIO_FEATURE_CONFIG2_V2_MSMC_PCIE_DISABLE_BMSK                                           0x400000
#define HWIO_FEATURE_CONFIG2_V2_MSMC_PCIE_DISABLE_SHFT                                               0x16
#define HWIO_FEATURE_CONFIG2_V2_NDINT_DISABLE_BMSK                                               0x200000
#define HWIO_FEATURE_CONFIG2_V2_NDINT_DISABLE_SHFT                                                   0x15
#define HWIO_FEATURE_CONFIG2_V2_QC_DAP_DEVICEEN_DISABLE_BMSK                                     0x100000
#define HWIO_FEATURE_CONFIG2_V2_QC_DAP_DEVICEEN_DISABLE_SHFT                                         0x14
#define HWIO_FEATURE_CONFIG2_V2_QC_DAP_SPNIDEN_DISABLE_BMSK                                       0x80000
#define HWIO_FEATURE_CONFIG2_V2_QC_DAP_SPNIDEN_DISABLE_SHFT                                          0x13
#define HWIO_FEATURE_CONFIG2_V2_QC_DAP_SPIDEN_DISABLE_BMSK                                        0x40000
#define HWIO_FEATURE_CONFIG2_V2_QC_DAP_SPIDEN_DISABLE_SHFT                                           0x12
#define HWIO_FEATURE_CONFIG2_V2_QC_DAP_NIDEN_DISABLE_BMSK                                         0x20000
#define HWIO_FEATURE_CONFIG2_V2_QC_DAP_NIDEN_DISABLE_SHFT                                            0x11
#define HWIO_FEATURE_CONFIG2_V2_QC_DAP_DBGEN_DISABLE_BMSK                                         0x10000
#define HWIO_FEATURE_CONFIG2_V2_QC_DAP_DBGEN_DISABLE_SHFT                                            0x10
#define HWIO_FEATURE_CONFIG2_V2_QC_APPS_SPNIDEN_DISABLE_BMSK                                       0x8000
#define HWIO_FEATURE_CONFIG2_V2_QC_APPS_SPNIDEN_DISABLE_SHFT                                          0xf
#define HWIO_FEATURE_CONFIG2_V2_QC_APPS_SPIDEN_DISABLE_BMSK                                        0x4000
#define HWIO_FEATURE_CONFIG2_V2_QC_APPS_SPIDEN_DISABLE_SHFT                                           0xe
#define HWIO_FEATURE_CONFIG2_V2_QC_APPS_NIDEN_DISABLE_BMSK                                         0x2000
#define HWIO_FEATURE_CONFIG2_V2_QC_APPS_NIDEN_DISABLE_SHFT                                            0xd
#define HWIO_FEATURE_CONFIG2_V2_QC_APPS_DBGEN_DISABLE_BMSK                                         0x1000
#define HWIO_FEATURE_CONFIG2_V2_QC_APPS_DBGEN_DISABLE_SHFT                                            0xc
#define HWIO_FEATURE_CONFIG2_V2_QC_SPARE1_DISABLE_BMSK                                              0x800
#define HWIO_FEATURE_CONFIG2_V2_QC_SPARE1_DISABLE_SHFT                                                0xb
#define HWIO_FEATURE_CONFIG2_V2_QC_SPARE0_DISABLE_BMSK                                              0x400
#define HWIO_FEATURE_CONFIG2_V2_QC_SPARE0_DISABLE_SHFT                                                0xa
#define HWIO_FEATURE_CONFIG2_V2_QC_SPARE2_DISABLE_BMSK                                              0x200
#define HWIO_FEATURE_CONFIG2_V2_QC_SPARE2_DISABLE_SHFT                                                0x9
#define HWIO_FEATURE_CONFIG2_V2_QC_RPM_DAPEN_DISABLE_BMSK                                           0x100
#define HWIO_FEATURE_CONFIG2_V2_QC_RPM_DAPEN_DISABLE_SHFT                                             0x8
#define HWIO_FEATURE_CONFIG2_V2_QC_RPM_NIDEN_DISABLE_BMSK                                            0x80
#define HWIO_FEATURE_CONFIG2_V2_QC_RPM_NIDEN_DISABLE_SHFT                                             0x7
#define HWIO_FEATURE_CONFIG2_V2_QC_RPM_DBGEN_DISABLE_BMSK                                            0x40
#define HWIO_FEATURE_CONFIG2_V2_QC_RPM_DBGEN_DISABLE_SHFT                                             0x6
#define HWIO_FEATURE_CONFIG2_V2_QC_SPARE3_DISABLE_BMSK                                               0x20
#define HWIO_FEATURE_CONFIG2_V2_QC_SPARE3_DISABLE_SHFT                                                0x5
#define HWIO_FEATURE_CONFIG2_V2_QC_MSS_NIDEN_DISABLE_BMSK                                            0x10
#define HWIO_FEATURE_CONFIG2_V2_QC_MSS_NIDEN_DISABLE_SHFT                                             0x4
#define HWIO_FEATURE_CONFIG2_V2_QC_MSS_DBGEN_DISABLE_BMSK                                             0x8
#define HWIO_FEATURE_CONFIG2_V2_QC_MSS_DBGEN_DISABLE_SHFT                                             0x3
#define HWIO_FEATURE_CONFIG2_V2_QDI_SPMI_DISABLE_BMSK                                                 0x4
#define HWIO_FEATURE_CONFIG2_V2_QDI_SPMI_DISABLE_SHFT                                                 0x2
#define HWIO_FEATURE_CONFIG2_V2_SM_BIST_DISABLE_BMSK                                                  0x2
#define HWIO_FEATURE_CONFIG2_V2_SM_BIST_DISABLE_SHFT                                                  0x1
#define HWIO_FEATURE_CONFIG2_V2_TIC_DISABLE_BMSK                                                      0x1
#define HWIO_FEATURE_CONFIG2_V2_TIC_DISABLE_SHFT                                                      0x0

#define HWIO_FEATURE_CONFIG3_V2_ADDR                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006010)
#define HWIO_FEATURE_CONFIG3_V2_OFFS                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006010)
#define HWIO_FEATURE_CONFIG3_V2_RMSK                                                           0xffffffff
#define HWIO_FEATURE_CONFIG3_V2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG3_V2_ADDR, HWIO_FEATURE_CONFIG3_V2_RMSK)
#define HWIO_FEATURE_CONFIG3_V2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG3_V2_ADDR, m)
#define HWIO_FEATURE_CONFIG3_V2_SEC_TAP_ACCESS_DISABLE_BMSK                                    0xfe000000
#define HWIO_FEATURE_CONFIG3_V2_SEC_TAP_ACCESS_DISABLE_SHFT                                          0x19
#define HWIO_FEATURE_CONFIG3_V2_TAP_CJI_CORE_SEL_DISABLE_BMSK                                   0x1000000
#define HWIO_FEATURE_CONFIG3_V2_TAP_CJI_CORE_SEL_DISABLE_SHFT                                        0x18
#define HWIO_FEATURE_CONFIG3_V2_TAP_INSTR_DISABLE_BMSK                                           0xfff800
#define HWIO_FEATURE_CONFIG3_V2_TAP_INSTR_DISABLE_SHFT                                                0xb
#define HWIO_FEATURE_CONFIG3_V2_SPARE1_BMSK                                                         0x400
#define HWIO_FEATURE_CONFIG3_V2_SPARE1_SHFT                                                           0xa
#define HWIO_FEATURE_CONFIG3_V2_MODEM_PBL_PATCH_VERSION_BMSK                                        0x3e0
#define HWIO_FEATURE_CONFIG3_V2_MODEM_PBL_PATCH_VERSION_SHFT                                          0x5
#define HWIO_FEATURE_CONFIG3_V2_APPS_PBL_PATCH_VERSION_BMSK                                          0x1f
#define HWIO_FEATURE_CONFIG3_V2_APPS_PBL_PATCH_VERSION_SHFT                                           0x0

#define HWIO_FEATURE_CONFIG4_V2_ADDR                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006014)
#define HWIO_FEATURE_CONFIG4_V2_OFFS                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006014)
#define HWIO_FEATURE_CONFIG4_V2_RMSK                                                           0xffffffff
#define HWIO_FEATURE_CONFIG4_V2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG4_V2_ADDR, HWIO_FEATURE_CONFIG4_V2_RMSK)
#define HWIO_FEATURE_CONFIG4_V2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG4_V2_ADDR, m)
#define HWIO_FEATURE_CONFIG4_V2_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK                          0xffffffff
#define HWIO_FEATURE_CONFIG4_V2_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                                 0x0

#define HWIO_FEATURE_CONFIG5_V2_ADDR                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006018)
#define HWIO_FEATURE_CONFIG5_V2_OFFS                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006018)
#define HWIO_FEATURE_CONFIG5_V2_RMSK                                                           0xffffffff
#define HWIO_FEATURE_CONFIG5_V2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG5_V2_ADDR, HWIO_FEATURE_CONFIG5_V2_RMSK)
#define HWIO_FEATURE_CONFIG5_V2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG5_V2_ADDR, m)
#define HWIO_FEATURE_CONFIG5_V2_STACKED_MEMORY_ID_BMSK                                         0xf0000000
#define HWIO_FEATURE_CONFIG5_V2_STACKED_MEMORY_ID_SHFT                                               0x1c
#define HWIO_FEATURE_CONFIG5_V2_APSS_BOOT_TRIGGER_DISABLE_BMSK                                  0x8000000
#define HWIO_FEATURE_CONFIG5_V2_APSS_BOOT_TRIGGER_DISABLE_SHFT                                       0x1b
#define HWIO_FEATURE_CONFIG5_V2_MODEM_PBL_PLL_CTRL_BMSK                                         0x7800000
#define HWIO_FEATURE_CONFIG5_V2_MODEM_PBL_PLL_CTRL_SHFT                                              0x17
#define HWIO_FEATURE_CONFIG5_V2_APPS_PBL_PLL_CTRL_BMSK                                           0x780000
#define HWIO_FEATURE_CONFIG5_V2_APPS_PBL_PLL_CTRL_SHFT                                               0x13
#define HWIO_FEATURE_CONFIG5_V2_APPS_BOOT_FSM_CFG_BMSK                                            0x40000
#define HWIO_FEATURE_CONFIG5_V2_APPS_BOOT_FSM_CFG_SHFT                                               0x12
#define HWIO_FEATURE_CONFIG5_V2_APPS_BOOT_FSM_DELAY_BMSK                                          0x30000
#define HWIO_FEATURE_CONFIG5_V2_APPS_BOOT_FSM_DELAY_SHFT                                             0x10
#define HWIO_FEATURE_CONFIG5_V2_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_BMSK                            0x8000
#define HWIO_FEATURE_CONFIG5_V2_BOOT_ROM_PATCH_SEL_EN_USB_PHY_TUNE_SHFT                               0xf
#define HWIO_FEATURE_CONFIG5_V2_USB_SS_ENABLE_BMSK                                                 0x4000
#define HWIO_FEATURE_CONFIG5_V2_USB_SS_ENABLE_SHFT                                                    0xe
#define HWIO_FEATURE_CONFIG5_V2_APPS_PBL_BOOT_SPEED_BMSK                                           0x3000
#define HWIO_FEATURE_CONFIG5_V2_APPS_PBL_BOOT_SPEED_SHFT                                              0xc
#define HWIO_FEATURE_CONFIG5_V2_APPS_BOOT_FROM_ROM_BMSK                                             0x800
#define HWIO_FEATURE_CONFIG5_V2_APPS_BOOT_FROM_ROM_SHFT                                               0xb
#define HWIO_FEATURE_CONFIG5_V2_MSA_ENA_BMSK                                                        0x400
#define HWIO_FEATURE_CONFIG5_V2_MSA_ENA_SHFT                                                          0xa
#define HWIO_FEATURE_CONFIG5_V2_FORCE_MSA_AUTH_EN_BMSK                                              0x200
#define HWIO_FEATURE_CONFIG5_V2_FORCE_MSA_AUTH_EN_SHFT                                                0x9
#define HWIO_FEATURE_CONFIG5_V2_PCIE_REFCLK_TERM_ENABLE_BMSK                                        0x100
#define HWIO_FEATURE_CONFIG5_V2_PCIE_REFCLK_TERM_ENABLE_SHFT                                          0x8
#define HWIO_FEATURE_CONFIG5_V2_MSS_HASH_INTEGRITY_CHECK_DISABLE_BMSK                                0x80
#define HWIO_FEATURE_CONFIG5_V2_MSS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                                 0x7
#define HWIO_FEATURE_CONFIG5_V2_MODEM_BOOT_FROM_ROM_BMSK                                             0x40
#define HWIO_FEATURE_CONFIG5_V2_MODEM_BOOT_FROM_ROM_SHFT                                              0x6
#define HWIO_FEATURE_CONFIG5_V2_BOOT_ROM_PATCH_DISABLE_BMSK                                          0x20
#define HWIO_FEATURE_CONFIG5_V2_BOOT_ROM_PATCH_DISABLE_SHFT                                           0x5
#define HWIO_FEATURE_CONFIG5_V2_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                               0x1f
#define HWIO_FEATURE_CONFIG5_V2_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                                0x0

#define HWIO_OEM_CONFIG0_V2_ADDR                                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000601c)
#define HWIO_OEM_CONFIG0_V2_OFFS                                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000601c)
#define HWIO_OEM_CONFIG0_V2_RMSK                                                               0xffffffff
#define HWIO_OEM_CONFIG0_V2_IN          \
        in_dword_masked(HWIO_OEM_CONFIG0_V2_ADDR, HWIO_OEM_CONFIG0_V2_RMSK)
#define HWIO_OEM_CONFIG0_V2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG0_V2_ADDR, m)
#define HWIO_OEM_CONFIG0_V2_DISABLE_CRASH_DUMP_OU_FIELD_BMSK                                   0x80000000
#define HWIO_OEM_CONFIG0_V2_DISABLE_CRASH_DUMP_OU_FIELD_SHFT                                         0x1f
#define HWIO_OEM_CONFIG0_V2_DEBUG_POLICY_DISABLE_BMSK                                          0x40000000
#define HWIO_OEM_CONFIG0_V2_DEBUG_POLICY_DISABLE_SHFT                                                0x1e
#define HWIO_OEM_CONFIG0_V2_RSVD0_BMSK                                                         0x30000000
#define HWIO_OEM_CONFIG0_V2_RSVD0_SHFT                                                               0x1c
#define HWIO_OEM_CONFIG0_V2_SPARE_REG26_SECURE_BMSK                                             0x8000000
#define HWIO_OEM_CONFIG0_V2_SPARE_REG26_SECURE_SHFT                                                  0x1b
#define HWIO_OEM_CONFIG0_V2_SPARE_REG25_SECURE_BMSK                                             0x4000000
#define HWIO_OEM_CONFIG0_V2_SPARE_REG25_SECURE_SHFT                                                  0x1a
#define HWIO_OEM_CONFIG0_V2_SPARE_REG24_SECURE_BMSK                                             0x2000000
#define HWIO_OEM_CONFIG0_V2_SPARE_REG24_SECURE_SHFT                                                  0x19
#define HWIO_OEM_CONFIG0_V2_SPARE_REG23_SECURE_BMSK                                             0x1000000
#define HWIO_OEM_CONFIG0_V2_SPARE_REG23_SECURE_SHFT                                                  0x18
#define HWIO_OEM_CONFIG0_V2_SPARE_REG22_SECURE_BMSK                                              0x800000
#define HWIO_OEM_CONFIG0_V2_SPARE_REG22_SECURE_SHFT                                                  0x17
#define HWIO_OEM_CONFIG0_V2_PBL_LOG_DISABLE_BMSK                                                 0x400000
#define HWIO_OEM_CONFIG0_V2_PBL_LOG_DISABLE_SHFT                                                     0x16
#define HWIO_OEM_CONFIG0_V2_ROOT_CERT_TOTAL_NUM_BMSK                                             0x3c0000
#define HWIO_OEM_CONFIG0_V2_ROOT_CERT_TOTAL_NUM_SHFT                                                 0x12
#define HWIO_OEM_CONFIG0_V2_SPARE_REG21_SECURE_BMSK                                               0x20000
#define HWIO_OEM_CONFIG0_V2_SPARE_REG21_SECURE_SHFT                                                  0x11
#define HWIO_OEM_CONFIG0_V2_SPARE_REG20_SECURE_BMSK                                               0x10000
#define HWIO_OEM_CONFIG0_V2_SPARE_REG20_SECURE_SHFT                                                  0x10
#define HWIO_OEM_CONFIG0_V2_SPARE_REG19_SECURE_BMSK                                                0x8000
#define HWIO_OEM_CONFIG0_V2_SPARE_REG19_SECURE_SHFT                                                   0xf
#define HWIO_OEM_CONFIG0_V2_SPARE_REG18_SECURE_BMSK                                                0x4000
#define HWIO_OEM_CONFIG0_V2_SPARE_REG18_SECURE_SHFT                                                   0xe
#define HWIO_OEM_CONFIG0_V2_RSVD1_BMSK                                                             0x2000
#define HWIO_OEM_CONFIG0_V2_RSVD1_SHFT                                                                0xd
#define HWIO_OEM_CONFIG0_V2_WDOG_EN_BMSK                                                           0x1000
#define HWIO_OEM_CONFIG0_V2_WDOG_EN_SHFT                                                              0xc
#define HWIO_OEM_CONFIG0_V2_SPDM_SECURE_MODE_BMSK                                                   0x800
#define HWIO_OEM_CONFIG0_V2_SPDM_SECURE_MODE_SHFT                                                     0xb
#define HWIO_OEM_CONFIG0_V2_ALT_SD_PORT_FOR_BOOT_BMSK                                               0x400
#define HWIO_OEM_CONFIG0_V2_ALT_SD_PORT_FOR_BOOT_SHFT                                                 0xa
#define HWIO_OEM_CONFIG0_V2_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                      0x200
#define HWIO_OEM_CONFIG0_V2_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                        0x9
#define HWIO_OEM_CONFIG0_V2_SDC_EMMC_MODE1P2_EN_BMSK                                                0x100
#define HWIO_OEM_CONFIG0_V2_SDC_EMMC_MODE1P2_EN_SHFT                                                  0x8
#define HWIO_OEM_CONFIG0_V2_FAST_BOOT_BMSK                                                           0xe0
#define HWIO_OEM_CONFIG0_V2_FAST_BOOT_SHFT                                                            0x5
#define HWIO_OEM_CONFIG0_V2_SDCC_MCLK_BOOT_FREQ_BMSK                                                 0x10
#define HWIO_OEM_CONFIG0_V2_SDCC_MCLK_BOOT_FREQ_SHFT                                                  0x4
#define HWIO_OEM_CONFIG0_V2_FORCE_DLOAD_DISABLE_BMSK                                                  0x8
#define HWIO_OEM_CONFIG0_V2_FORCE_DLOAD_DISABLE_SHFT                                                  0x3
#define HWIO_OEM_CONFIG0_V2_SPARE_BMSK                                                                0x4
#define HWIO_OEM_CONFIG0_V2_SPARE_SHFT                                                                0x2
#define HWIO_OEM_CONFIG0_V2_ENUM_TIMEOUT_BMSK                                                         0x2
#define HWIO_OEM_CONFIG0_V2_ENUM_TIMEOUT_SHFT                                                         0x1
#define HWIO_OEM_CONFIG0_V2_E_DLOAD_DISABLE_BMSK                                                      0x1
#define HWIO_OEM_CONFIG0_V2_E_DLOAD_DISABLE_SHFT                                                      0x0

#define HWIO_OEM_CONFIG1_V2_ADDR                                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006020)
#define HWIO_OEM_CONFIG1_V2_OFFS                                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006020)
#define HWIO_OEM_CONFIG1_V2_RMSK                                                               0xffffffff
#define HWIO_OEM_CONFIG1_V2_IN          \
        in_dword_masked(HWIO_OEM_CONFIG1_V2_ADDR, HWIO_OEM_CONFIG1_V2_RMSK)
#define HWIO_OEM_CONFIG1_V2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG1_V2_ADDR, m)
#define HWIO_OEM_CONFIG1_V2_SPARE1_BMSK                                                        0xff800000
#define HWIO_OEM_CONFIG1_V2_SPARE1_SHFT                                                              0x17
#define HWIO_OEM_CONFIG1_V2_ANTI_ROLLBACK_FEATURE_EN_BMSK                                        0x780000
#define HWIO_OEM_CONFIG1_V2_ANTI_ROLLBACK_FEATURE_EN_SHFT                                            0x13
#define HWIO_OEM_CONFIG1_V2_DAP_DEVICEEN_DISABLE_BMSK                                             0x40000
#define HWIO_OEM_CONFIG1_V2_DAP_DEVICEEN_DISABLE_SHFT                                                0x12
#define HWIO_OEM_CONFIG1_V2_DAP_SPNIDEN_DISABLE_BMSK                                              0x20000
#define HWIO_OEM_CONFIG1_V2_DAP_SPNIDEN_DISABLE_SHFT                                                 0x11
#define HWIO_OEM_CONFIG1_V2_DAP_SPIDEN_DISABLE_BMSK                                               0x10000
#define HWIO_OEM_CONFIG1_V2_DAP_SPIDEN_DISABLE_SHFT                                                  0x10
#define HWIO_OEM_CONFIG1_V2_DAP_NIDEN_DISABLE_BMSK                                                 0x8000
#define HWIO_OEM_CONFIG1_V2_DAP_NIDEN_DISABLE_SHFT                                                    0xf
#define HWIO_OEM_CONFIG1_V2_DAP_DBGEN_DISABLE_BMSK                                                 0x4000
#define HWIO_OEM_CONFIG1_V2_DAP_DBGEN_DISABLE_SHFT                                                    0xe
#define HWIO_OEM_CONFIG1_V2_APPS_SPNIDEN_DISABLE_BMSK                                              0x2000
#define HWIO_OEM_CONFIG1_V2_APPS_SPNIDEN_DISABLE_SHFT                                                 0xd
#define HWIO_OEM_CONFIG1_V2_APPS_SPIDEN_DISABLE_BMSK                                               0x1000
#define HWIO_OEM_CONFIG1_V2_APPS_SPIDEN_DISABLE_SHFT                                                  0xc
#define HWIO_OEM_CONFIG1_V2_APPS_NIDEN_DISABLE_BMSK                                                 0x800
#define HWIO_OEM_CONFIG1_V2_APPS_NIDEN_DISABLE_SHFT                                                   0xb
#define HWIO_OEM_CONFIG1_V2_APPS_DBGEN_DISABLE_BMSK                                                 0x400
#define HWIO_OEM_CONFIG1_V2_APPS_DBGEN_DISABLE_SHFT                                                   0xa
#define HWIO_OEM_CONFIG1_V2_SPARE1_DISABLE_BMSK                                                     0x200
#define HWIO_OEM_CONFIG1_V2_SPARE1_DISABLE_SHFT                                                       0x9
#define HWIO_OEM_CONFIG1_V2_SPARE0_DISABLE_BMSK                                                     0x100
#define HWIO_OEM_CONFIG1_V2_SPARE0_DISABLE_SHFT                                                       0x8
#define HWIO_OEM_CONFIG1_V2_SPARE2_DISABLE_BMSK                                                      0x80
#define HWIO_OEM_CONFIG1_V2_SPARE2_DISABLE_SHFT                                                       0x7
#define HWIO_OEM_CONFIG1_V2_RPM_DAPEN_DISABLE_BMSK                                                   0x40
#define HWIO_OEM_CONFIG1_V2_RPM_DAPEN_DISABLE_SHFT                                                    0x6
#define HWIO_OEM_CONFIG1_V2_RPM_NIDEN_DISABLE_BMSK                                                   0x20
#define HWIO_OEM_CONFIG1_V2_RPM_NIDEN_DISABLE_SHFT                                                    0x5
#define HWIO_OEM_CONFIG1_V2_RPM_DBGEN_DISABLE_BMSK                                                   0x10
#define HWIO_OEM_CONFIG1_V2_RPM_DBGEN_DISABLE_SHFT                                                    0x4
#define HWIO_OEM_CONFIG1_V2_SPARE3_DISABLE_BMSK                                                       0x8
#define HWIO_OEM_CONFIG1_V2_SPARE3_DISABLE_SHFT                                                       0x3
#define HWIO_OEM_CONFIG1_V2_MSS_NIDEN_DISABLE_BMSK                                                    0x4
#define HWIO_OEM_CONFIG1_V2_MSS_NIDEN_DISABLE_SHFT                                                    0x2
#define HWIO_OEM_CONFIG1_V2_MSS_DBGEN_DISABLE_BMSK                                                    0x2
#define HWIO_OEM_CONFIG1_V2_MSS_DBGEN_DISABLE_SHFT                                                    0x1
#define HWIO_OEM_CONFIG1_V2_ALL_DEBUG_DISABLE_BMSK                                                    0x1
#define HWIO_OEM_CONFIG1_V2_ALL_DEBUG_DISABLE_SHFT                                                    0x0

#define HWIO_OEM_CONFIG2_V2_ADDR                                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006024)
#define HWIO_OEM_CONFIG2_V2_OFFS                                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006024)
#define HWIO_OEM_CONFIG2_V2_RMSK                                                               0xffffffff
#define HWIO_OEM_CONFIG2_V2_IN          \
        in_dword_masked(HWIO_OEM_CONFIG2_V2_ADDR, HWIO_OEM_CONFIG2_V2_RMSK)
#define HWIO_OEM_CONFIG2_V2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG2_V2_ADDR, m)
#define HWIO_OEM_CONFIG2_V2_OEM_PRODUCT_ID_BMSK                                                0xffff0000
#define HWIO_OEM_CONFIG2_V2_OEM_PRODUCT_ID_SHFT                                                      0x10
#define HWIO_OEM_CONFIG2_V2_OEM_HW_ID_BMSK                                                         0xffff
#define HWIO_OEM_CONFIG2_V2_OEM_HW_ID_SHFT                                                            0x0

#define HWIO_OEM_CONFIG3_V2_ADDR                                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006028)
#define HWIO_OEM_CONFIG3_V2_OFFS                                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006028)
#define HWIO_OEM_CONFIG3_V2_RMSK                                                               0xffffffff
#define HWIO_OEM_CONFIG3_V2_IN          \
        in_dword_masked(HWIO_OEM_CONFIG3_V2_ADDR, HWIO_OEM_CONFIG3_V2_RMSK)
#define HWIO_OEM_CONFIG3_V2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG3_V2_ADDR, m)
#define HWIO_OEM_CONFIG3_V2_PERIPH_VID_BMSK                                                    0xffff0000
#define HWIO_OEM_CONFIG3_V2_PERIPH_VID_SHFT                                                          0x10
#define HWIO_OEM_CONFIG3_V2_PERIPH_PID_BMSK                                                        0xffff
#define HWIO_OEM_CONFIG3_V2_PERIPH_PID_SHFT                                                           0x0

#define HWIO_BOOT_CONFIG_V2_ADDR                                                               (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000602c)
#define HWIO_BOOT_CONFIG_V2_OFFS                                                               (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000602c)
#define HWIO_BOOT_CONFIG_V2_RMSK                                                                    0x3ff
#define HWIO_BOOT_CONFIG_V2_IN          \
        in_dword_masked(HWIO_BOOT_CONFIG_V2_ADDR, HWIO_BOOT_CONFIG_V2_RMSK)
#define HWIO_BOOT_CONFIG_V2_INM(m)      \
        in_dword_masked(HWIO_BOOT_CONFIG_V2_ADDR, m)
#define HWIO_BOOT_CONFIG_V2_USB_SS_ENABLE_BMSK                                                      0x200
#define HWIO_BOOT_CONFIG_V2_USB_SS_ENABLE_SHFT                                                        0x9
#define HWIO_BOOT_CONFIG_V2_FORCE_MSA_AUTH_EN_BMSK                                                  0x100
#define HWIO_BOOT_CONFIG_V2_FORCE_MSA_AUTH_EN_SHFT                                                    0x8
#define HWIO_BOOT_CONFIG_V2_APPS_PBL_BOOT_SPEED_BMSK                                                 0xc0
#define HWIO_BOOT_CONFIG_V2_APPS_PBL_BOOT_SPEED_SHFT                                                  0x6
#define HWIO_BOOT_CONFIG_V2_APPS_BOOT_FROM_ROM_BMSK                                                  0x20
#define HWIO_BOOT_CONFIG_V2_APPS_BOOT_FROM_ROM_SHFT                                                   0x5
#define HWIO_BOOT_CONFIG_V2_RSVD_BMSK                                                                0x10
#define HWIO_BOOT_CONFIG_V2_RSVD_SHFT                                                                 0x4
#define HWIO_BOOT_CONFIG_V2_FAST_BOOT_BMSK                                                            0xe
#define HWIO_BOOT_CONFIG_V2_FAST_BOOT_SHFT                                                            0x1
#define HWIO_BOOT_CONFIG_V2_WDOG_EN_BMSK                                                              0x1
#define HWIO_BOOT_CONFIG_V2_WDOG_EN_SHFT                                                              0x0

#define HWIO_SECURE_BOOTn_V2_ADDR(n)                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006030 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_V2_OFFS(n)                                                           (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006030 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_V2_RMSK                                                                   0x1ff
#define HWIO_SECURE_BOOTn_V2_MAXn                                                                       7
#define HWIO_SECURE_BOOTn_V2_INI(n)        \
        in_dword_masked(HWIO_SECURE_BOOTn_V2_ADDR(n), HWIO_SECURE_BOOTn_V2_RMSK)
#define HWIO_SECURE_BOOTn_V2_INMI(n,mask)    \
        in_dword_masked(HWIO_SECURE_BOOTn_V2_ADDR(n), mask)
#define HWIO_SECURE_BOOTn_V2_FUSE_SRC_BMSK                                                          0x100
#define HWIO_SECURE_BOOTn_V2_FUSE_SRC_SHFT                                                            0x8
#define HWIO_SECURE_BOOTn_V2_RSVD_7_BMSK                                                             0x80
#define HWIO_SECURE_BOOTn_V2_RSVD_7_SHFT                                                              0x7
#define HWIO_SECURE_BOOTn_V2_USE_SERIAL_NUM_BMSK                                                     0x40
#define HWIO_SECURE_BOOTn_V2_USE_SERIAL_NUM_SHFT                                                      0x6
#define HWIO_SECURE_BOOTn_V2_AUTH_EN_BMSK                                                            0x20
#define HWIO_SECURE_BOOTn_V2_AUTH_EN_SHFT                                                             0x5
#define HWIO_SECURE_BOOTn_V2_PK_HASH_IN_FUSE_BMSK                                                    0x10
#define HWIO_SECURE_BOOTn_V2_PK_HASH_IN_FUSE_SHFT                                                     0x4
#define HWIO_SECURE_BOOTn_V2_ROM_PK_HASH_INDEX_BMSK                                                   0xf
#define HWIO_SECURE_BOOTn_V2_ROM_PK_HASH_INDEX_SHFT                                                   0x0

#define HWIO_OVERRIDE_0_V2_ADDR                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006058)
#define HWIO_OVERRIDE_0_V2_OFFS                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006058)
#define HWIO_OVERRIDE_0_V2_RMSK                                                                0x1fffffff
#define HWIO_OVERRIDE_0_V2_IN          \
        in_dword_masked(HWIO_OVERRIDE_0_V2_ADDR, HWIO_OVERRIDE_0_V2_RMSK)
#define HWIO_OVERRIDE_0_V2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_0_V2_ADDR, m)
#define HWIO_OVERRIDE_0_V2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_0_V2_ADDR,v)
#define HWIO_OVERRIDE_0_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_0_V2_ADDR,m,v,HWIO_OVERRIDE_0_V2_IN)
#define HWIO_OVERRIDE_0_V2_TX_DISABLE_BMSK                                                     0x10000000
#define HWIO_OVERRIDE_0_V2_TX_DISABLE_SHFT                                                           0x1c
#define HWIO_OVERRIDE_0_V2_RSVD_27_2_BMSK                                                       0xffffffc
#define HWIO_OVERRIDE_0_V2_RSVD_27_2_SHFT                                                             0x2
#define HWIO_OVERRIDE_0_V2_SDC_EMMC_MODE1P2_EN_BMSK                                                   0x2
#define HWIO_OVERRIDE_0_V2_SDC_EMMC_MODE1P2_EN_SHFT                                                   0x1
#define HWIO_OVERRIDE_0_V2_SDC_EMMC_MODE1P2_OVERRIDE_BMSK                                             0x1
#define HWIO_OVERRIDE_0_V2_SDC_EMMC_MODE1P2_OVERRIDE_SHFT                                             0x0

#define HWIO_OVERRIDE_1_V2_ADDR                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000605c)
#define HWIO_OVERRIDE_1_V2_OFFS                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000605c)
#define HWIO_OVERRIDE_1_V2_RMSK                                                                0xffffffff
#define HWIO_OVERRIDE_1_V2_IN          \
        in_dword_masked(HWIO_OVERRIDE_1_V2_ADDR, HWIO_OVERRIDE_1_V2_RMSK)
#define HWIO_OVERRIDE_1_V2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_1_V2_ADDR, m)
#define HWIO_OVERRIDE_1_V2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_1_V2_ADDR,v)
#define HWIO_OVERRIDE_1_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_1_V2_ADDR,m,v,HWIO_OVERRIDE_1_V2_IN)
#define HWIO_OVERRIDE_1_V2_OVERRIDE_1_BMSK                                                     0xffffffff
#define HWIO_OVERRIDE_1_V2_OVERRIDE_1_SHFT                                                            0x0

#define HWIO_OVERRIDE_2_V2_ADDR                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006060)
#define HWIO_OVERRIDE_2_V2_OFFS                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006060)
#define HWIO_OVERRIDE_2_V2_RMSK                                                                 0x1fffc00
#define HWIO_OVERRIDE_2_V2_IN          \
        in_dword_masked(HWIO_OVERRIDE_2_V2_ADDR, HWIO_OVERRIDE_2_V2_RMSK)
#define HWIO_OVERRIDE_2_V2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_2_V2_ADDR, m)
#define HWIO_OVERRIDE_2_V2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_2_V2_ADDR,v)
#define HWIO_OVERRIDE_2_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_2_V2_ADDR,m,v,HWIO_OVERRIDE_2_V2_IN)
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE4_DISABLE_BMSK                                            0x1000000
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE4_DISABLE_SHFT                                                 0x18
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE2_DISABLE_BMSK                                             0x800000
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE2_DISABLE_SHFT                                                 0x17
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE3_DISABLE_BMSK                                             0x400000
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE3_DISABLE_SHFT                                                 0x16
#define HWIO_OVERRIDE_2_V2_OVRID_DAP_DEVICEEN_DISABLE_BMSK                                       0x200000
#define HWIO_OVERRIDE_2_V2_OVRID_DAP_DEVICEEN_DISABLE_SHFT                                           0x15
#define HWIO_OVERRIDE_2_V2_OVRID_DAP_NIDEN_DISABLE_BMSK                                          0x100000
#define HWIO_OVERRIDE_2_V2_OVRID_DAP_NIDEN_DISABLE_SHFT                                              0x14
#define HWIO_OVERRIDE_2_V2_OVRID_DAP_DBGEN_DISABLE_BMSK                                           0x80000
#define HWIO_OVERRIDE_2_V2_OVRID_DAP_DBGEN_DISABLE_SHFT                                              0x13
#define HWIO_OVERRIDE_2_V2_OVRID_APPS_NIDEN_DISABLE_BMSK                                          0x40000
#define HWIO_OVERRIDE_2_V2_OVRID_APPS_NIDEN_DISABLE_SHFT                                             0x12
#define HWIO_OVERRIDE_2_V2_OVRID_APPS_DBGEN_DISABLE_BMSK                                          0x20000
#define HWIO_OVERRIDE_2_V2_OVRID_APPS_DBGEN_DISABLE_SHFT                                             0x11
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE1_DISABLE_BMSK                                              0x10000
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE1_DISABLE_SHFT                                                 0x10
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE0_DISABLE_BMSK                                               0x8000
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE0_DISABLE_SHFT                                                  0xf
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE5_DISABLE_BMSK                                               0x4000
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE5_DISABLE_SHFT                                                  0xe
#define HWIO_OVERRIDE_2_V2_OVRID_RPM_DAPEN_DISABLE_BMSK                                            0x2000
#define HWIO_OVERRIDE_2_V2_OVRID_RPM_DAPEN_DISABLE_SHFT                                               0xd
#define HWIO_OVERRIDE_2_V2_OVRID_RPM_NIDEN_DISABLE_BMSK                                            0x1000
#define HWIO_OVERRIDE_2_V2_OVRID_RPM_NIDEN_DISABLE_SHFT                                               0xc
#define HWIO_OVERRIDE_2_V2_OVRID_RPM_DBGEN_DISABLE_BMSK                                             0x800
#define HWIO_OVERRIDE_2_V2_OVRID_RPM_DBGEN_DISABLE_SHFT                                               0xb
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE6_DISABLE_BMSK                                                0x400
#define HWIO_OVERRIDE_2_V2_OVRID_SPARE6_DISABLE_SHFT                                                  0xa

#define HWIO_OVERRIDE_3_V2_ADDR                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006064)
#define HWIO_OVERRIDE_3_V2_OFFS                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006064)
#define HWIO_OVERRIDE_3_V2_RMSK                                                                      0x1f
#define HWIO_OVERRIDE_3_V2_IN          \
        in_dword_masked(HWIO_OVERRIDE_3_V2_ADDR, HWIO_OVERRIDE_3_V2_RMSK)
#define HWIO_OVERRIDE_3_V2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_3_V2_ADDR, m)
#define HWIO_OVERRIDE_3_V2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_3_V2_ADDR,v)
#define HWIO_OVERRIDE_3_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_3_V2_ADDR,m,v,HWIO_OVERRIDE_3_V2_IN)
#define HWIO_OVERRIDE_3_V2_OVRID_DAP_SPNIDEN_DISABLE_BMSK                                            0x10
#define HWIO_OVERRIDE_3_V2_OVRID_DAP_SPNIDEN_DISABLE_SHFT                                             0x4
#define HWIO_OVERRIDE_3_V2_OVRID_DAP_SPIDEN_DISABLE_BMSK                                              0x8
#define HWIO_OVERRIDE_3_V2_OVRID_DAP_SPIDEN_DISABLE_SHFT                                              0x3
#define HWIO_OVERRIDE_3_V2_OVRID_APPS_SPNIDEN_DISABLE_BMSK                                            0x4
#define HWIO_OVERRIDE_3_V2_OVRID_APPS_SPNIDEN_DISABLE_SHFT                                            0x2
#define HWIO_OVERRIDE_3_V2_OVRID_APPS_SPIDEN_DISABLE_BMSK                                             0x2
#define HWIO_OVERRIDE_3_V2_OVRID_APPS_SPIDEN_DISABLE_SHFT                                             0x1
#define HWIO_OVERRIDE_3_V2_OVRID_SPDM_SECURE_MODE_BMSK                                                0x1
#define HWIO_OVERRIDE_3_V2_OVRID_SPDM_SECURE_MODE_SHFT                                                0x0

#define HWIO_OVERRIDE_4_V2_ADDR                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006068)
#define HWIO_OVERRIDE_4_V2_OFFS                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006068)
#define HWIO_OVERRIDE_4_V2_RMSK                                                                       0x3
#define HWIO_OVERRIDE_4_V2_IN          \
        in_dword_masked(HWIO_OVERRIDE_4_V2_ADDR, HWIO_OVERRIDE_4_V2_RMSK)
#define HWIO_OVERRIDE_4_V2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_4_V2_ADDR, m)
#define HWIO_OVERRIDE_4_V2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_4_V2_ADDR,v)
#define HWIO_OVERRIDE_4_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_4_V2_ADDR,m,v,HWIO_OVERRIDE_4_V2_IN)
#define HWIO_OVERRIDE_4_V2_OVRID_MSS_NIDEN_DISABLE_BMSK                                               0x2
#define HWIO_OVERRIDE_4_V2_OVRID_MSS_NIDEN_DISABLE_SHFT                                               0x1
#define HWIO_OVERRIDE_4_V2_OVRID_MSS_DBGEN_DISABLE_BMSK                                               0x1
#define HWIO_OVERRIDE_4_V2_OVRID_MSS_DBGEN_DISABLE_SHFT                                               0x0

#define HWIO_CAPT_SEC_GPIO_V2_ADDR                                                             (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000606c)
#define HWIO_CAPT_SEC_GPIO_V2_OFFS                                                             (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000606c)
#define HWIO_CAPT_SEC_GPIO_V2_RMSK                                                                0x1ffff
#define HWIO_CAPT_SEC_GPIO_V2_IN          \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_V2_ADDR, HWIO_CAPT_SEC_GPIO_V2_RMSK)
#define HWIO_CAPT_SEC_GPIO_V2_INM(m)      \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_V2_ADDR, m)
#define HWIO_CAPT_SEC_GPIO_V2_OUT(v)      \
        out_dword(HWIO_CAPT_SEC_GPIO_V2_ADDR,v)
#define HWIO_CAPT_SEC_GPIO_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CAPT_SEC_GPIO_V2_ADDR,m,v,HWIO_CAPT_SEC_GPIO_V2_IN)
#define HWIO_CAPT_SEC_GPIO_V2_SDC_EMMC_MODE1P2_EN_BMSK                                            0x10000
#define HWIO_CAPT_SEC_GPIO_V2_SDC_EMMC_MODE1P2_EN_SHFT                                               0x10
#define HWIO_CAPT_SEC_GPIO_V2_FORCE_USB_BOOT_GPIO_BMSK                                             0x8000
#define HWIO_CAPT_SEC_GPIO_V2_FORCE_USB_BOOT_GPIO_SHFT                                                0xf
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_FORCE_MSA_AUTH_EN_BMSK                              0x4000
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_FORCE_MSA_AUTH_EN_SHFT                                 0xe
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_AP_AUTH_EN_BMSK                                     0x2000
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_AP_AUTH_EN_SHFT                                        0xd
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_AP_PK_HASH_IN_FUSE_BMSK                             0x1000
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_AP_PK_HASH_IN_FUSE_SHFT                                0xc
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_MSA_AUTH_EN_BMSK                                     0x800
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_MSA_AUTH_EN_SHFT                                       0xb
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_MSA_PK_HASH_IN_FUSE_BMSK                             0x400
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_MSA_PK_HASH_IN_FUSE_SHFT                               0xa
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_ALL_USE_SERIAL_NUM_BMSK                              0x200
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_ALL_USE_SERIAL_NUM_SHFT                                0x9
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_PK_HASH_INDEX_SRC_BMSK                               0x100
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_PK_HASH_INDEX_SRC_SHFT                                 0x8
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_APPS_PBL_BOOT_SPEED_BMSK                              0xc0
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_APPS_PBL_BOOT_SPEED_SHFT                               0x6
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_APPS_BOOT_FROM_ROM_BMSK                               0x20
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_APPS_BOOT_FROM_ROM_SHFT                                0x5
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_USB_SS_ENABLE_BMSK                                    0x10
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_USB_SS_ENABLE_SHFT                                     0x4
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_FAST_BOOT_BMSK                                         0xe
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_FAST_BOOT_SHFT                                         0x1
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_WDOG_DISABLE_BMSK                                      0x1
#define HWIO_CAPT_SEC_GPIO_V2_BOOT_CONFIG_GPIO_WDOG_DISABLE_SHFT                                      0x0

#define HWIO_APP_PROC_CFG_V2_ADDR                                                              (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006070)
#define HWIO_APP_PROC_CFG_V2_OFFS                                                              (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006070)
#define HWIO_APP_PROC_CFG_V2_RMSK                                                               0x1ffffff
#define HWIO_APP_PROC_CFG_V2_IN          \
        in_dword_masked(HWIO_APP_PROC_CFG_V2_ADDR, HWIO_APP_PROC_CFG_V2_RMSK)
#define HWIO_APP_PROC_CFG_V2_INM(m)      \
        in_dword_masked(HWIO_APP_PROC_CFG_V2_ADDR, m)
#define HWIO_APP_PROC_CFG_V2_OUT(v)      \
        out_dword(HWIO_APP_PROC_CFG_V2_ADDR,v)
#define HWIO_APP_PROC_CFG_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APP_PROC_CFG_V2_ADDR,m,v,HWIO_APP_PROC_CFG_V2_IN)
#define HWIO_APP_PROC_CFG_V2_SPARE0_BMSK                                                        0x1000000
#define HWIO_APP_PROC_CFG_V2_SPARE0_SHFT                                                             0x18
#define HWIO_APP_PROC_CFG_V2_SPARE1_BMSK                                                         0x800000
#define HWIO_APP_PROC_CFG_V2_SPARE1_SHFT                                                             0x17
#define HWIO_APP_PROC_CFG_V2_RPM_DBG_NIDEN_BMSK                                                  0x400000
#define HWIO_APP_PROC_CFG_V2_RPM_DBG_NIDEN_SHFT                                                      0x16
#define HWIO_APP_PROC_CFG_V2_DAP_DBG_NIDEN_BMSK                                                  0x200000
#define HWIO_APP_PROC_CFG_V2_DAP_DBG_NIDEN_SHFT                                                      0x15
#define HWIO_APP_PROC_CFG_V2_DAP_DBG_SPNIDEN_BMSK                                                0x100000
#define HWIO_APP_PROC_CFG_V2_DAP_DBG_SPNIDEN_SHFT                                                    0x14
#define HWIO_APP_PROC_CFG_V2_APPS_CFG_MISC3_BMSK                                                  0xf0000
#define HWIO_APP_PROC_CFG_V2_APPS_CFG_MISC3_SHFT                                                     0x10
#define HWIO_APP_PROC_CFG_V2_APPS_CFG_MISC2_BMSK                                                   0xf000
#define HWIO_APP_PROC_CFG_V2_APPS_CFG_MISC2_SHFT                                                      0xc
#define HWIO_APP_PROC_CFG_V2_APPS_CFG_MISC1_BMSK                                                    0xf00
#define HWIO_APP_PROC_CFG_V2_APPS_CFG_MISC1_SHFT                                                      0x8
#define HWIO_APP_PROC_CFG_V2_APPS_CFG_MISC0_BMSK                                                     0xf0
#define HWIO_APP_PROC_CFG_V2_APPS_CFG_MISC0_SHFT                                                      0x4
#define HWIO_APP_PROC_CFG_V2_APPS_DBG_NIDEN_BMSK                                                      0x8
#define HWIO_APP_PROC_CFG_V2_APPS_DBG_NIDEN_SHFT                                                      0x3
#define HWIO_APP_PROC_CFG_V2_APPS_DBG_SPNIDEN_BMSK                                                    0x4
#define HWIO_APP_PROC_CFG_V2_APPS_DBG_SPNIDEN_SHFT                                                    0x2
#define HWIO_APP_PROC_CFG_V2_APPS_CP15_DISABLE_BMSK                                                   0x2
#define HWIO_APP_PROC_CFG_V2_APPS_CP15_DISABLE_SHFT                                                   0x1
#define HWIO_APP_PROC_CFG_V2_APPS_CFG_NMFI_BMSK                                                       0x1
#define HWIO_APP_PROC_CFG_V2_APPS_CFG_NMFI_SHFT                                                       0x0

#define HWIO_MSS_PROC_CFG_V2_ADDR                                                              (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006074)
#define HWIO_MSS_PROC_CFG_V2_OFFS                                                              (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006074)
#define HWIO_MSS_PROC_CFG_V2_RMSK                                                                     0x1
#define HWIO_MSS_PROC_CFG_V2_IN          \
        in_dword_masked(HWIO_MSS_PROC_CFG_V2_ADDR, HWIO_MSS_PROC_CFG_V2_RMSK)
#define HWIO_MSS_PROC_CFG_V2_INM(m)      \
        in_dword_masked(HWIO_MSS_PROC_CFG_V2_ADDR, m)
#define HWIO_MSS_PROC_CFG_V2_OUT(v)      \
        out_dword(HWIO_MSS_PROC_CFG_V2_ADDR,v)
#define HWIO_MSS_PROC_CFG_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_PROC_CFG_V2_ADDR,m,v,HWIO_MSS_PROC_CFG_V2_IN)
#define HWIO_MSS_PROC_CFG_V2_MSS_DBG_NIDEN_BMSK                                                       0x1
#define HWIO_MSS_PROC_CFG_V2_MSS_DBG_NIDEN_SHFT                                                       0x0

#define HWIO_QFPROM_CLK_CTL_V2_ADDR                                                            (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006078)
#define HWIO_QFPROM_CLK_CTL_V2_OFFS                                                            (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006078)
#define HWIO_QFPROM_CLK_CTL_V2_RMSK                                                                   0x1
#define HWIO_QFPROM_CLK_CTL_V2_IN          \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_V2_ADDR, HWIO_QFPROM_CLK_CTL_V2_RMSK)
#define HWIO_QFPROM_CLK_CTL_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_V2_ADDR, m)
#define HWIO_QFPROM_CLK_CTL_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_CLK_CTL_V2_ADDR,v)
#define HWIO_QFPROM_CLK_CTL_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CLK_CTL_V2_ADDR,m,v,HWIO_QFPROM_CLK_CTL_V2_IN)
#define HWIO_QFPROM_CLK_CTL_V2_CLK_HALT_BMSK                                                          0x1
#define HWIO_QFPROM_CLK_CTL_V2_CLK_HALT_SHFT                                                          0x0

#define HWIO_JTAG_ID_V2_ADDR                                                                   (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000607c)
#define HWIO_JTAG_ID_V2_OFFS                                                                   (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000607c)
#define HWIO_JTAG_ID_V2_RMSK                                                                   0xffffffff
#define HWIO_JTAG_ID_V2_IN          \
        in_dword_masked(HWIO_JTAG_ID_V2_ADDR, HWIO_JTAG_ID_V2_RMSK)
#define HWIO_JTAG_ID_V2_INM(m)      \
        in_dword_masked(HWIO_JTAG_ID_V2_ADDR, m)
#define HWIO_JTAG_ID_V2_JTAG_ID_BMSK                                                           0xffffffff
#define HWIO_JTAG_ID_V2_JTAG_ID_SHFT                                                                  0x0

#define HWIO_OEM_ID_V2_ADDR                                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006080)
#define HWIO_OEM_ID_V2_OFFS                                                                    (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006080)
#define HWIO_OEM_ID_V2_RMSK                                                                    0xffffffff
#define HWIO_OEM_ID_V2_IN          \
        in_dword_masked(HWIO_OEM_ID_V2_ADDR, HWIO_OEM_ID_V2_RMSK)
#define HWIO_OEM_ID_V2_INM(m)      \
        in_dword_masked(HWIO_OEM_ID_V2_ADDR, m)
#define HWIO_OEM_ID_V2_OEM_ID_BMSK                                                             0xffff0000
#define HWIO_OEM_ID_V2_OEM_ID_SHFT                                                                   0x10
#define HWIO_OEM_ID_V2_OEM_PRODUCT_ID_BMSK                                                         0xffff
#define HWIO_OEM_ID_V2_OEM_PRODUCT_ID_SHFT                                                            0x0

#define HWIO_TEST_BUS_SEL_V2_ADDR                                                              (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006084)
#define HWIO_TEST_BUS_SEL_V2_OFFS                                                              (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006084)
#define HWIO_TEST_BUS_SEL_V2_RMSK                                                                     0xf
#define HWIO_TEST_BUS_SEL_V2_IN          \
        in_dword_masked(HWIO_TEST_BUS_SEL_V2_ADDR, HWIO_TEST_BUS_SEL_V2_RMSK)
#define HWIO_TEST_BUS_SEL_V2_INM(m)      \
        in_dword_masked(HWIO_TEST_BUS_SEL_V2_ADDR, m)
#define HWIO_TEST_BUS_SEL_V2_OUT(v)      \
        out_dword(HWIO_TEST_BUS_SEL_V2_ADDR,v)
#define HWIO_TEST_BUS_SEL_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TEST_BUS_SEL_V2_ADDR,m,v,HWIO_TEST_BUS_SEL_V2_IN)
#define HWIO_TEST_BUS_SEL_V2_TEST_EN_BMSK                                                             0x8
#define HWIO_TEST_BUS_SEL_V2_TEST_EN_SHFT                                                             0x3
#define HWIO_TEST_BUS_SEL_V2_TEST_SELECT_BMSK                                                         0x7
#define HWIO_TEST_BUS_SEL_V2_TEST_SELECT_SHFT                                                         0x0

#define HWIO_SPDM_DYN_SECURE_MODE_V2_ADDR                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006088)
#define HWIO_SPDM_DYN_SECURE_MODE_V2_OFFS                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006088)
#define HWIO_SPDM_DYN_SECURE_MODE_V2_RMSK                                                             0x1
#define HWIO_SPDM_DYN_SECURE_MODE_V2_IN          \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_V2_ADDR, HWIO_SPDM_DYN_SECURE_MODE_V2_RMSK)
#define HWIO_SPDM_DYN_SECURE_MODE_V2_INM(m)      \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_V2_ADDR, m)
#define HWIO_SPDM_DYN_SECURE_MODE_V2_OUT(v)      \
        out_dword(HWIO_SPDM_DYN_SECURE_MODE_V2_ADDR,v)
#define HWIO_SPDM_DYN_SECURE_MODE_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPDM_DYN_SECURE_MODE_V2_ADDR,m,v,HWIO_SPDM_DYN_SECURE_MODE_V2_IN)
#define HWIO_SPDM_DYN_SECURE_MODE_V2_SECURE_MODE_BMSK                                                 0x1
#define HWIO_SPDM_DYN_SECURE_MODE_V2_SECURE_MODE_SHFT                                                 0x0

#define HWIO_OVERRIDE_5_V2_ADDR                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x0000608c)
#define HWIO_OVERRIDE_5_V2_OFFS                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x0000608c)
#define HWIO_OVERRIDE_5_V2_RMSK                                                                0xffffffff
#define HWIO_OVERRIDE_5_V2_IN          \
        in_dword_masked(HWIO_OVERRIDE_5_V2_ADDR, HWIO_OVERRIDE_5_V2_RMSK)
#define HWIO_OVERRIDE_5_V2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_5_V2_ADDR, m)
#define HWIO_OVERRIDE_5_V2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_5_V2_ADDR,v)
#define HWIO_OVERRIDE_5_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_5_V2_ADDR,m,v,HWIO_OVERRIDE_5_V2_IN)
#define HWIO_OVERRIDE_5_V2_RSVD_31_0_BMSK                                                      0xffffffff
#define HWIO_OVERRIDE_5_V2_RSVD_31_0_SHFT                                                             0x0

#define HWIO_OVERRIDE_6_V2_ADDR                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006090)
#define HWIO_OVERRIDE_6_V2_OFFS                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006090)
#define HWIO_OVERRIDE_6_V2_RMSK                                                                0xffffffff
#define HWIO_OVERRIDE_6_V2_IN          \
        in_dword_masked(HWIO_OVERRIDE_6_V2_ADDR, HWIO_OVERRIDE_6_V2_RMSK)
#define HWIO_OVERRIDE_6_V2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_6_V2_ADDR, m)
#define HWIO_OVERRIDE_6_V2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_6_V2_ADDR,v)
#define HWIO_OVERRIDE_6_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_6_V2_ADDR,m,v,HWIO_OVERRIDE_6_V2_IN)
#define HWIO_OVERRIDE_6_V2_RSVD_31_0_BMSK                                                      0xffffffff
#define HWIO_OVERRIDE_6_V2_RSVD_31_0_SHFT                                                             0x0

#define HWIO_PTE0_V2_ADDR                                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060a0)
#define HWIO_PTE0_V2_OFFS                                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060a0)
#define HWIO_PTE0_V2_RMSK                                                                      0xffffffff
#define HWIO_PTE0_V2_IN          \
        in_dword_masked(HWIO_PTE0_V2_ADDR, HWIO_PTE0_V2_RMSK)
#define HWIO_PTE0_V2_INM(m)      \
        in_dword_masked(HWIO_PTE0_V2_ADDR, m)
#define HWIO_PTE0_V2_IDDQ_CX_ON_BMSK                                                           0xc0000000
#define HWIO_PTE0_V2_IDDQ_CX_ON_SHFT                                                                 0x1e
#define HWIO_PTE0_V2_SPARE_BMSK                                                                0x20000000
#define HWIO_PTE0_V2_SPARE_SHFT                                                                      0x1d
#define HWIO_PTE0_V2_MACCHIATO_EN_BMSK                                                         0x10000000
#define HWIO_PTE0_V2_MACCHIATO_EN_SHFT                                                               0x1c
#define HWIO_PTE0_V2_FEATURE_ID_BMSK                                                            0xff00000
#define HWIO_PTE0_V2_FEATURE_ID_SHFT                                                                 0x14
#define HWIO_PTE0_V2_JTAG_ID_BMSK                                                                 0xfffff
#define HWIO_PTE0_V2_JTAG_ID_SHFT                                                                     0x0

#define HWIO_PTE1_V2_ADDR                                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060a4)
#define HWIO_PTE1_V2_OFFS                                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060a4)
#define HWIO_PTE1_V2_RMSK                                                                      0xffffffff
#define HWIO_PTE1_V2_IN          \
        in_dword_masked(HWIO_PTE1_V2_ADDR, HWIO_PTE1_V2_RMSK)
#define HWIO_PTE1_V2_INM(m)      \
        in_dword_masked(HWIO_PTE1_V2_ADDR, m)
#define HWIO_PTE1_V2_IDDQ_BINNING_BMSK                                                         0xfc000000
#define HWIO_PTE1_V2_IDDQ_BINNING_SHFT                                                               0x1a
#define HWIO_PTE1_V2_PROCESS_NODE_ID_BMSK                                                       0x2000000
#define HWIO_PTE1_V2_PROCESS_NODE_ID_SHFT                                                            0x19
#define HWIO_PTE1_V2_PTE_DATA1_BMSK                                                             0x1ffffff
#define HWIO_PTE1_V2_PTE_DATA1_SHFT                                                                   0x0

#define HWIO_SERIAL_NUM_V2_ADDR                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060a8)
#define HWIO_SERIAL_NUM_V2_OFFS                                                                (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060a8)
#define HWIO_SERIAL_NUM_V2_RMSK                                                                0xffffffff
#define HWIO_SERIAL_NUM_V2_IN          \
        in_dword_masked(HWIO_SERIAL_NUM_V2_ADDR, HWIO_SERIAL_NUM_V2_RMSK)
#define HWIO_SERIAL_NUM_V2_INM(m)      \
        in_dword_masked(HWIO_SERIAL_NUM_V2_ADDR, m)
#define HWIO_SERIAL_NUM_V2_SERIAL_NUM_BMSK                                                     0xffffffff
#define HWIO_SERIAL_NUM_V2_SERIAL_NUM_SHFT                                                            0x0

#define HWIO_PTE2_V2_ADDR                                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060ac)
#define HWIO_PTE2_V2_OFFS                                                                      (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060ac)
#define HWIO_PTE2_V2_RMSK                                                                      0xffffffff
#define HWIO_PTE2_V2_IN          \
        in_dword_masked(HWIO_PTE2_V2_ADDR, HWIO_PTE2_V2_RMSK)
#define HWIO_PTE2_V2_INM(m)      \
        in_dword_masked(HWIO_PTE2_V2_ADDR, m)
#define HWIO_PTE2_V2_OUT(v)      \
        out_dword(HWIO_PTE2_V2_ADDR,v)
#define HWIO_PTE2_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PTE2_V2_ADDR,m,v,HWIO_PTE2_V2_IN)
#define HWIO_PTE2_V2_MX_RET_BIN_BMSK                                                           0xe0000000
#define HWIO_PTE2_V2_MX_RET_BIN_SHFT                                                                 0x1d
#define HWIO_PTE2_V2_IDDQ_SPARE_2_BMSK                                                         0x10000000
#define HWIO_PTE2_V2_IDDQ_SPARE_2_SHFT                                                               0x1c
#define HWIO_PTE2_V2_IDDQ_RC_BMSK                                                               0xc000000
#define HWIO_PTE2_V2_IDDQ_RC_SHFT                                                                    0x1a
#define HWIO_PTE2_V2_IDDQ_MSS_OFF_BMSK                                                          0x3f80000
#define HWIO_PTE2_V2_IDDQ_MSS_OFF_SHFT                                                               0x13
#define HWIO_PTE2_V2_IDDQ_MSS_ON_BMSK                                                             0x7f800
#define HWIO_PTE2_V2_IDDQ_MSS_ON_SHFT                                                                 0xb
#define HWIO_PTE2_V2_FOUNDRY_ID_BMSK                                                                0x700
#define HWIO_PTE2_V2_FOUNDRY_ID_SHFT                                                                  0x8
#define HWIO_PTE2_V2_LOGIC_RETENTION_BMSK                                                            0xe0
#define HWIO_PTE2_V2_LOGIC_RETENTION_SHFT                                                             0x5
#define HWIO_PTE2_V2_SPEED_BIN_BMSK                                                                  0x1c
#define HWIO_PTE2_V2_SPEED_BIN_SHFT                                                                   0x2
#define HWIO_PTE2_V2_IDDQ_SPARE_BMSK                                                                  0x3
#define HWIO_PTE2_V2_IDDQ_SPARE_SHFT                                                                  0x0

#define HWIO_ANTI_ROLLBACK_1_0_V2_ADDR                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060b0)
#define HWIO_ANTI_ROLLBACK_1_0_V2_OFFS                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060b0)
#define HWIO_ANTI_ROLLBACK_1_0_V2_RMSK                                                         0xffffffff
#define HWIO_ANTI_ROLLBACK_1_0_V2_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_0_V2_ADDR, HWIO_ANTI_ROLLBACK_1_0_V2_RMSK)
#define HWIO_ANTI_ROLLBACK_1_0_V2_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_0_V2_ADDR, m)
#define HWIO_ANTI_ROLLBACK_1_0_V2_PIL_SUBSYSTEM0_BMSK                                          0xfc000000
#define HWIO_ANTI_ROLLBACK_1_0_V2_PIL_SUBSYSTEM0_SHFT                                                0x1a
#define HWIO_ANTI_ROLLBACK_1_0_V2_TZ_BMSK                                                       0x3fff000
#define HWIO_ANTI_ROLLBACK_1_0_V2_TZ_SHFT                                                             0xc
#define HWIO_ANTI_ROLLBACK_1_0_V2_SBL1_BMSK                                                         0xffe
#define HWIO_ANTI_ROLLBACK_1_0_V2_SBL1_SHFT                                                           0x1
#define HWIO_ANTI_ROLLBACK_1_0_V2_RPMB_KEY_PROVISIONED_BMSK                                           0x1
#define HWIO_ANTI_ROLLBACK_1_0_V2_RPMB_KEY_PROVISIONED_SHFT                                           0x0

#define HWIO_ANTI_ROLLBACK_1_1_V2_ADDR                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060b4)
#define HWIO_ANTI_ROLLBACK_1_1_V2_OFFS                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060b4)
#define HWIO_ANTI_ROLLBACK_1_1_V2_RMSK                                                         0xffffffff
#define HWIO_ANTI_ROLLBACK_1_1_V2_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_1_V2_ADDR, HWIO_ANTI_ROLLBACK_1_1_V2_RMSK)
#define HWIO_ANTI_ROLLBACK_1_1_V2_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_1_V2_ADDR, m)
#define HWIO_ANTI_ROLLBACK_1_1_V2_APPSBL0_BMSK                                                 0xfffc0000
#define HWIO_ANTI_ROLLBACK_1_1_V2_APPSBL0_SHFT                                                       0x12
#define HWIO_ANTI_ROLLBACK_1_1_V2_PIL_SUBSYSTEM1_BMSK                                             0x3ffff
#define HWIO_ANTI_ROLLBACK_1_1_V2_PIL_SUBSYSTEM1_SHFT                                                 0x0

#define HWIO_ANTI_ROLLBACK_2_0_V2_ADDR                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060b8)
#define HWIO_ANTI_ROLLBACK_2_0_V2_OFFS                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060b8)
#define HWIO_ANTI_ROLLBACK_2_0_V2_RMSK                                                         0xffffffff
#define HWIO_ANTI_ROLLBACK_2_0_V2_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_0_V2_ADDR, HWIO_ANTI_ROLLBACK_2_0_V2_RMSK)
#define HWIO_ANTI_ROLLBACK_2_0_V2_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_0_V2_ADDR, m)
#define HWIO_ANTI_ROLLBACK_2_0_V2_APPSBL1_BMSK                                                 0xffffffff
#define HWIO_ANTI_ROLLBACK_2_0_V2_APPSBL1_SHFT                                                        0x0

#define HWIO_ANTI_ROLLBACK_2_1_V2_ADDR                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060bc)
#define HWIO_ANTI_ROLLBACK_2_1_V2_OFFS                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060bc)
#define HWIO_ANTI_ROLLBACK_2_1_V2_RMSK                                                         0xffffffff
#define HWIO_ANTI_ROLLBACK_2_1_V2_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_1_V2_ADDR, HWIO_ANTI_ROLLBACK_2_1_V2_RMSK)
#define HWIO_ANTI_ROLLBACK_2_1_V2_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_1_V2_ADDR, m)
#define HWIO_ANTI_ROLLBACK_2_1_V2_ROOT_CERT_PK_HASH_INDEX_BMSK                                 0xff000000
#define HWIO_ANTI_ROLLBACK_2_1_V2_ROOT_CERT_PK_HASH_INDEX_SHFT                                       0x18
#define HWIO_ANTI_ROLLBACK_2_1_V2_HYPERVISOR_BMSK                                                0xfff000
#define HWIO_ANTI_ROLLBACK_2_1_V2_HYPERVISOR_SHFT                                                     0xc
#define HWIO_ANTI_ROLLBACK_2_1_V2_RPM_BMSK                                                          0xff0
#define HWIO_ANTI_ROLLBACK_2_1_V2_RPM_SHFT                                                            0x4
#define HWIO_ANTI_ROLLBACK_2_1_V2_APPSBL2_BMSK                                                        0xf
#define HWIO_ANTI_ROLLBACK_2_1_V2_APPSBL2_SHFT                                                        0x0

#define HWIO_ANTI_ROLLBACK_3_0_V2_ADDR                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060c0)
#define HWIO_ANTI_ROLLBACK_3_0_V2_OFFS                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060c0)
#define HWIO_ANTI_ROLLBACK_3_0_V2_RMSK                                                         0xffffffff
#define HWIO_ANTI_ROLLBACK_3_0_V2_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_0_V2_ADDR, HWIO_ANTI_ROLLBACK_3_0_V2_RMSK)
#define HWIO_ANTI_ROLLBACK_3_0_V2_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_0_V2_ADDR, m)
#define HWIO_ANTI_ROLLBACK_3_0_V2_MSS_BMSK                                                     0xffff0000
#define HWIO_ANTI_ROLLBACK_3_0_V2_MSS_SHFT                                                           0x10
#define HWIO_ANTI_ROLLBACK_3_0_V2_MBA_BMSK                                                         0xffff
#define HWIO_ANTI_ROLLBACK_3_0_V2_MBA_SHFT                                                            0x0

#define HWIO_ANTI_ROLLBACK_3_1_V2_ADDR                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060c4)
#define HWIO_ANTI_ROLLBACK_3_1_V2_OFFS                                                         (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060c4)
#define HWIO_ANTI_ROLLBACK_3_1_V2_RMSK                                                         0xffffffff
#define HWIO_ANTI_ROLLBACK_3_1_V2_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_1_V2_ADDR, HWIO_ANTI_ROLLBACK_3_1_V2_RMSK)
#define HWIO_ANTI_ROLLBACK_3_1_V2_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_1_V2_ADDR, m)
#define HWIO_ANTI_ROLLBACK_3_1_V2_SPARE0_BMSK                                                  0xffffff00
#define HWIO_ANTI_ROLLBACK_3_1_V2_SPARE0_SHFT                                                         0x8
#define HWIO_ANTI_ROLLBACK_3_1_V2_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                                 0xff
#define HWIO_ANTI_ROLLBACK_3_1_V2_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                                  0x0

#define HWIO_PK_HASH_0_V2_ADDR                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060c8)
#define HWIO_PK_HASH_0_V2_OFFS                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060c8)
#define HWIO_PK_HASH_0_V2_RMSK                                                                 0xffffffff
#define HWIO_PK_HASH_0_V2_IN          \
        in_dword_masked(HWIO_PK_HASH_0_V2_ADDR, HWIO_PK_HASH_0_V2_RMSK)
#define HWIO_PK_HASH_0_V2_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_0_V2_ADDR, m)
#define HWIO_PK_HASH_0_V2_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_PK_HASH_0_V2_HASH_DATA0_SHFT                                                             0x0

#define HWIO_PK_HASH_1_V2_ADDR                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060cc)
#define HWIO_PK_HASH_1_V2_OFFS                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060cc)
#define HWIO_PK_HASH_1_V2_RMSK                                                                 0xffffffff
#define HWIO_PK_HASH_1_V2_IN          \
        in_dword_masked(HWIO_PK_HASH_1_V2_ADDR, HWIO_PK_HASH_1_V2_RMSK)
#define HWIO_PK_HASH_1_V2_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_1_V2_ADDR, m)
#define HWIO_PK_HASH_1_V2_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_PK_HASH_1_V2_HASH_DATA0_SHFT                                                             0x0

#define HWIO_PK_HASH_2_V2_ADDR                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060d0)
#define HWIO_PK_HASH_2_V2_OFFS                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060d0)
#define HWIO_PK_HASH_2_V2_RMSK                                                                 0xffffffff
#define HWIO_PK_HASH_2_V2_IN          \
        in_dword_masked(HWIO_PK_HASH_2_V2_ADDR, HWIO_PK_HASH_2_V2_RMSK)
#define HWIO_PK_HASH_2_V2_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_2_V2_ADDR, m)
#define HWIO_PK_HASH_2_V2_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_PK_HASH_2_V2_HASH_DATA0_SHFT                                                             0x0

#define HWIO_PK_HASH_3_V2_ADDR                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060d4)
#define HWIO_PK_HASH_3_V2_OFFS                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060d4)
#define HWIO_PK_HASH_3_V2_RMSK                                                                 0xffffffff
#define HWIO_PK_HASH_3_V2_IN          \
        in_dword_masked(HWIO_PK_HASH_3_V2_ADDR, HWIO_PK_HASH_3_V2_RMSK)
#define HWIO_PK_HASH_3_V2_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_3_V2_ADDR, m)
#define HWIO_PK_HASH_3_V2_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_PK_HASH_3_V2_HASH_DATA0_SHFT                                                             0x0

#define HWIO_PK_HASH_4_V2_ADDR                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060d8)
#define HWIO_PK_HASH_4_V2_OFFS                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060d8)
#define HWIO_PK_HASH_4_V2_RMSK                                                                 0xffffffff
#define HWIO_PK_HASH_4_V2_IN          \
        in_dword_masked(HWIO_PK_HASH_4_V2_ADDR, HWIO_PK_HASH_4_V2_RMSK)
#define HWIO_PK_HASH_4_V2_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_4_V2_ADDR, m)
#define HWIO_PK_HASH_4_V2_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_PK_HASH_4_V2_HASH_DATA0_SHFT                                                             0x0

#define HWIO_PK_HASH_5_V2_ADDR                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060dc)
#define HWIO_PK_HASH_5_V2_OFFS                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060dc)
#define HWIO_PK_HASH_5_V2_RMSK                                                                 0xffffffff
#define HWIO_PK_HASH_5_V2_IN          \
        in_dword_masked(HWIO_PK_HASH_5_V2_ADDR, HWIO_PK_HASH_5_V2_RMSK)
#define HWIO_PK_HASH_5_V2_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_5_V2_ADDR, m)
#define HWIO_PK_HASH_5_V2_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_PK_HASH_5_V2_HASH_DATA0_SHFT                                                             0x0

#define HWIO_PK_HASH_6_V2_ADDR                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060e0)
#define HWIO_PK_HASH_6_V2_OFFS                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060e0)
#define HWIO_PK_HASH_6_V2_RMSK                                                                 0xffffffff
#define HWIO_PK_HASH_6_V2_IN          \
        in_dword_masked(HWIO_PK_HASH_6_V2_ADDR, HWIO_PK_HASH_6_V2_RMSK)
#define HWIO_PK_HASH_6_V2_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_6_V2_ADDR, m)
#define HWIO_PK_HASH_6_V2_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_PK_HASH_6_V2_HASH_DATA0_SHFT                                                             0x0

#define HWIO_PK_HASH_7_V2_ADDR                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x000060e4)
#define HWIO_PK_HASH_7_V2_OFFS                                                                 (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x000060e4)
#define HWIO_PK_HASH_7_V2_RMSK                                                                 0xffffffff
#define HWIO_PK_HASH_7_V2_IN          \
        in_dword_masked(HWIO_PK_HASH_7_V2_ADDR, HWIO_PK_HASH_7_V2_RMSK)
#define HWIO_PK_HASH_7_V2_INM(m)      \
        in_dword_masked(HWIO_PK_HASH_7_V2_ADDR, m)
#define HWIO_PK_HASH_7_V2_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_PK_HASH_7_V2_HASH_DATA0_SHFT                                                             0x0

#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006100)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006100)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_IN          \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_ADDR, HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_RMSK)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_ADDR, m)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_ADDR,v)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_ADDR,m,v,HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_IN)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION31_STICKY_BIT_BMSK                      0x80000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION31_STICKY_BIT_SHFT                            0x1f
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION30_STICKY_BIT_BMSK                      0x40000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION30_STICKY_BIT_SHFT                            0x1e
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION29_STICKY_BIT_BMSK                      0x20000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION29_STICKY_BIT_SHFT                            0x1d
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION28_STICKY_BIT_BMSK                      0x10000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION28_STICKY_BIT_SHFT                            0x1c
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION27_STICKY_BIT_BMSK                       0x8000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION27_STICKY_BIT_SHFT                            0x1b
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION26_STICKY_BIT_BMSK                       0x4000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION26_STICKY_BIT_SHFT                            0x1a
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION25_STICKY_BIT_BMSK                       0x2000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION25_STICKY_BIT_SHFT                            0x19
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION24_STICKY_BIT_BMSK                       0x1000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION24_STICKY_BIT_SHFT                            0x18
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION23_STICKY_BIT_BMSK                        0x800000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION23_STICKY_BIT_SHFT                            0x17
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION22_STICKY_BIT_BMSK                        0x400000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION22_STICKY_BIT_SHFT                            0x16
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION21_STICKY_BIT_BMSK                        0x200000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION21_STICKY_BIT_SHFT                            0x15
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION20_STICKY_BIT_BMSK                        0x100000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION20_STICKY_BIT_SHFT                            0x14
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION19_STICKY_BIT_BMSK                         0x80000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION19_STICKY_BIT_SHFT                            0x13
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION18_STICKY_BIT_BMSK                         0x40000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION18_STICKY_BIT_SHFT                            0x12
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION17_STICKY_BIT_BMSK                         0x20000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION17_STICKY_BIT_SHFT                            0x11
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION16_STICKY_BIT_BMSK                         0x10000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION16_STICKY_BIT_SHFT                            0x10
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION15_STICKY_BIT_BMSK                          0x8000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION15_STICKY_BIT_SHFT                             0xf
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION14_STICKY_BIT_BMSK                          0x4000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION14_STICKY_BIT_SHFT                             0xe
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION13_STICKY_BIT_BMSK                          0x2000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION13_STICKY_BIT_SHFT                             0xd
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION12_STICKY_BIT_BMSK                          0x1000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION12_STICKY_BIT_SHFT                             0xc
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION11_STICKY_BIT_BMSK                           0x800
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION11_STICKY_BIT_SHFT                             0xb
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION10_STICKY_BIT_BMSK                           0x400
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION10_STICKY_BIT_SHFT                             0xa
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION9_STICKY_BIT_BMSK                            0x200
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION9_STICKY_BIT_SHFT                              0x9
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION8_STICKY_BIT_BMSK                            0x100
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION8_STICKY_BIT_SHFT                              0x8
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION7_STICKY_BIT_BMSK                             0x80
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION7_STICKY_BIT_SHFT                              0x7
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION6_STICKY_BIT_BMSK                             0x40
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION6_STICKY_BIT_SHFT                              0x6
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION5_STICKY_BIT_BMSK                             0x20
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION5_STICKY_BIT_SHFT                              0x5
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION4_STICKY_BIT_BMSK                             0x10
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION4_STICKY_BIT_SHFT                              0x4
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION3_STICKY_BIT_BMSK                              0x8
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION3_STICKY_BIT_SHFT                              0x3
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION2_STICKY_BIT_BMSK                              0x4
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION2_STICKY_BIT_SHFT                              0x2
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION1_STICKY_BIT_BMSK                              0x2
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION1_STICKY_BIT_SHFT                              0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION0_STICKY_BIT_BMSK                              0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_V2_REGION0_STICKY_BIT_SHFT                              0x0

#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_ADDR                                          (SECURITY_CONTROL_CORE_V2_REG_BASE      + 0x00006104)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_OFFS                                          (SECURITY_CONTROL_CORE_V2_REG_BASE_OFFS + 0x00006104)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_RMSK                                          0xffffffff
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_IN          \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_ADDR, HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_RMSK)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_ADDR, m)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_OUT(v)      \
        out_dword(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_ADDR,v)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_ADDR,m,v,HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_IN)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_RSVD0_BMSK                                    0xffffe000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_RSVD0_SHFT                                           0xd
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION44_STICKY_BIT_BMSK                          0x1000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION44_STICKY_BIT_SHFT                             0xc
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION43_STICKY_BIT_BMSK                           0x800
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION43_STICKY_BIT_SHFT                             0xb
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION42_STICKY_BIT_BMSK                           0x400
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION42_STICKY_BIT_SHFT                             0xa
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION41_STICKY_BIT_BMSK                           0x200
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION41_STICKY_BIT_SHFT                             0x9
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION40_STICKY_BIT_BMSK                           0x100
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION40_STICKY_BIT_SHFT                             0x8
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION39_STICKY_BIT_BMSK                            0x80
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION39_STICKY_BIT_SHFT                             0x7
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION38_STICKY_BIT_BMSK                            0x40
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION38_STICKY_BIT_SHFT                             0x6
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION37_STICKY_BIT_BMSK                            0x20
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION37_STICKY_BIT_SHFT                             0x5
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION36_STICKY_BIT_BMSK                            0x10
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION36_STICKY_BIT_SHFT                             0x4
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION35_STICKY_BIT_BMSK                             0x8
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION35_STICKY_BIT_SHFT                             0x3
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION34_STICKY_BIT_BMSK                             0x4
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION34_STICKY_BIT_SHFT                             0x2
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION33_STICKY_BIT_BMSK                             0x2
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION33_STICKY_BIT_SHFT                             0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION32_STICKY_BIT_BMSK                             0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_V2_REGION32_STICKY_BIT_SHFT                             0x0


#endif /* __SECURITYCONTROLV2HWIO_H__ */
