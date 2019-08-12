#ifndef __CE0_MSMHWIOREG_H__
#define __CE0_MSMHWIOREG_H__
/*
===========================================================================
*/
/**
  @file ce0_msmhwioreg.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    CRYPTO0_CRYPTO

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

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/cryptodrivers/ce/chipset/msm8909/inc/ce0_msmhwioreg.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                        (SECURITY_CONTROL_BASE      + 0x00000000)

#define HWIO_HW_KEY_STATUS_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002080)
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

/*----------------------------------------------------------------------------
 * MODULE: CRYPTO0_CRYPTO
 *--------------------------------------------------------------------------*/

#define CRYPTO0_CRYPTO_REG_BASE                                                                (CRYPTO0_CRYPTO_TOP_BASE      + 0x0003a000)

#define HWIO_CRYPTO0_CRYPTO_VERSION_ADDR                                                       (CRYPTO0_CRYPTO_REG_BASE      + 0x00000000)
#define HWIO_CRYPTO0_CRYPTO_VERSION_RMSK                                                       0xffffffff
#define HWIO_CRYPTO0_CRYPTO_VERSION_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_VERSION_ADDR, HWIO_CRYPTO0_CRYPTO_VERSION_RMSK)
#define HWIO_CRYPTO0_CRYPTO_VERSION_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_VERSION_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_VERSION_MAJ_VER_BMSK                                               0xff000000
#define HWIO_CRYPTO0_CRYPTO_VERSION_MAJ_VER_SHFT                                                     0x18
#define HWIO_CRYPTO0_CRYPTO_VERSION_MIN_VER_BMSK                                                 0xff0000
#define HWIO_CRYPTO0_CRYPTO_VERSION_MIN_VER_SHFT                                                     0x10
#define HWIO_CRYPTO0_CRYPTO_VERSION_STEP_VER_BMSK                                                  0xffff
#define HWIO_CRYPTO0_CRYPTO_VERSION_STEP_VER_SHFT                                                     0x0

#define HWIO_CRYPTO0_CRYPTO_DATA_INn_ADDR(n)                                                   (CRYPTO0_CRYPTO_REG_BASE      + 0x00000010 + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_DATA_INn_RMSK                                                      0xffffffff
#define HWIO_CRYPTO0_CRYPTO_DATA_INn_MAXn                                                               3
#define HWIO_CRYPTO0_CRYPTO_DATA_INn_OUTI(n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_DATA_INn_ADDR(n),val)
#define HWIO_CRYPTO0_CRYPTO_DATA_INn_DATA_IN_BMSK                                              0xffffffff
#define HWIO_CRYPTO0_CRYPTO_DATA_INn_DATA_IN_SHFT                                                     0x0

#define HWIO_CRYPTO0_CRYPTO_DATA_OUTn_ADDR(n)                                                  (CRYPTO0_CRYPTO_REG_BASE      + 0x00000020 + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_DATA_OUTn_RMSK                                                     0xffffffff
#define HWIO_CRYPTO0_CRYPTO_DATA_OUTn_MAXn                                                              3
#define HWIO_CRYPTO0_CRYPTO_DATA_OUTn_INI(n)        \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_DATA_OUTn_ADDR(n), HWIO_CRYPTO0_CRYPTO_DATA_OUTn_RMSK)
#define HWIO_CRYPTO0_CRYPTO_DATA_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_DATA_OUTn_ADDR(n), mask)
#define HWIO_CRYPTO0_CRYPTO_DATA_OUTn_DATA_OUT_BMSK                                            0xffffffff
#define HWIO_CRYPTO0_CRYPTO_DATA_OUTn_DATA_OUT_SHFT                                                   0x0

#define HWIO_CRYPTO0_CRYPTO_STATUS_ADDR                                                        (CRYPTO0_CRYPTO_REG_BASE      + 0x00000100)
#define HWIO_CRYPTO0_CRYPTO_STATUS_RMSK                                                        0xffffffff
#define HWIO_CRYPTO0_CRYPTO_STATUS_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_STATUS_ADDR, HWIO_CRYPTO0_CRYPTO_STATUS_RMSK)
#define HWIO_CRYPTO0_CRYPTO_STATUS_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_STATUS_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_STATUS_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_STATUS_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_STATUS_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_STATUS_IN)
#define HWIO_CRYPTO0_CRYPTO_STATUS_MAC_FAILED_BMSK                                             0x80000000
#define HWIO_CRYPTO0_CRYPTO_STATUS_MAC_FAILED_SHFT                                                   0x1f
#define HWIO_CRYPTO0_CRYPTO_STATUS_DOUT_SIZE_AVAIL_BMSK                                        0x7c000000
#define HWIO_CRYPTO0_CRYPTO_STATUS_DOUT_SIZE_AVAIL_SHFT                                              0x1a
#define HWIO_CRYPTO0_CRYPTO_STATUS_DIN_SIZE_AVAIL_BMSK                                          0x3e00000
#define HWIO_CRYPTO0_CRYPTO_STATUS_DIN_SIZE_AVAIL_SHFT                                               0x15
#define HWIO_CRYPTO0_CRYPTO_STATUS_HSD_ERR_BMSK                                                  0x100000
#define HWIO_CRYPTO0_CRYPTO_STATUS_HSD_ERR_SHFT                                                      0x14
#define HWIO_CRYPTO0_CRYPTO_STATUS_ACCESS_VIOL_BMSK                                               0x80000
#define HWIO_CRYPTO0_CRYPTO_STATUS_ACCESS_VIOL_SHFT                                                  0x13
#define HWIO_CRYPTO0_CRYPTO_STATUS_PIPE_ACTIVE_ERR_BMSK                                           0x40000
#define HWIO_CRYPTO0_CRYPTO_STATUS_PIPE_ACTIVE_ERR_SHFT                                              0x12
#define HWIO_CRYPTO0_CRYPTO_STATUS_CFG_CHNG_ERR_BMSK                                              0x20000
#define HWIO_CRYPTO0_CRYPTO_STATUS_CFG_CHNG_ERR_SHFT                                                 0x11
#define HWIO_CRYPTO0_CRYPTO_STATUS_DOUT_ERR_BMSK                                                  0x10000
#define HWIO_CRYPTO0_CRYPTO_STATUS_DOUT_ERR_SHFT                                                     0x10
#define HWIO_CRYPTO0_CRYPTO_STATUS_DIN_ERR_BMSK                                                    0x8000
#define HWIO_CRYPTO0_CRYPTO_STATUS_DIN_ERR_SHFT                                                       0xf
#define HWIO_CRYPTO0_CRYPTO_STATUS_AXI_ERR_BMSK                                                    0x4000
#define HWIO_CRYPTO0_CRYPTO_STATUS_AXI_ERR_SHFT                                                       0xe
#define HWIO_CRYPTO0_CRYPTO_STATUS_CRYPTO_STATE_BMSK                                               0x3c00
#define HWIO_CRYPTO0_CRYPTO_STATUS_CRYPTO_STATE_SHFT                                                  0xa
#define HWIO_CRYPTO0_CRYPTO_STATUS_ENCR_BUSY_BMSK                                                   0x200
#define HWIO_CRYPTO0_CRYPTO_STATUS_ENCR_BUSY_SHFT                                                     0x9
#define HWIO_CRYPTO0_CRYPTO_STATUS_AUTH_BUSY_BMSK                                                   0x100
#define HWIO_CRYPTO0_CRYPTO_STATUS_AUTH_BUSY_SHFT                                                     0x8
#define HWIO_CRYPTO0_CRYPTO_STATUS_DOUT_INTR_BMSK                                                    0x80
#define HWIO_CRYPTO0_CRYPTO_STATUS_DOUT_INTR_SHFT                                                     0x7
#define HWIO_CRYPTO0_CRYPTO_STATUS_DIN_INTR_BMSK                                                     0x40
#define HWIO_CRYPTO0_CRYPTO_STATUS_DIN_INTR_SHFT                                                      0x6
#define HWIO_CRYPTO0_CRYPTO_STATUS_OP_DONE_INTR_BMSK                                                 0x20
#define HWIO_CRYPTO0_CRYPTO_STATUS_OP_DONE_INTR_SHFT                                                  0x5
#define HWIO_CRYPTO0_CRYPTO_STATUS_ERR_INTR_BMSK                                                     0x10
#define HWIO_CRYPTO0_CRYPTO_STATUS_ERR_INTR_SHFT                                                      0x4
#define HWIO_CRYPTO0_CRYPTO_STATUS_DOUT_RDY_BMSK                                                      0x8
#define HWIO_CRYPTO0_CRYPTO_STATUS_DOUT_RDY_SHFT                                                      0x3
#define HWIO_CRYPTO0_CRYPTO_STATUS_DIN_RDY_BMSK                                                       0x4
#define HWIO_CRYPTO0_CRYPTO_STATUS_DIN_RDY_SHFT                                                       0x2
#define HWIO_CRYPTO0_CRYPTO_STATUS_OPERATION_DONE_BMSK                                                0x2
#define HWIO_CRYPTO0_CRYPTO_STATUS_OPERATION_DONE_SHFT                                                0x1
#define HWIO_CRYPTO0_CRYPTO_STATUS_SW_ERR_BMSK                                                        0x1
#define HWIO_CRYPTO0_CRYPTO_STATUS_SW_ERR_SHFT                                                        0x0

#define HWIO_CRYPTO0_CRYPTO_STATUS2_ADDR                                                       (CRYPTO0_CRYPTO_REG_BASE      + 0x00000104)
#define HWIO_CRYPTO0_CRYPTO_STATUS2_RMSK                                                              0x3
#define HWIO_CRYPTO0_CRYPTO_STATUS2_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_STATUS2_ADDR, HWIO_CRYPTO0_CRYPTO_STATUS2_RMSK)
#define HWIO_CRYPTO0_CRYPTO_STATUS2_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_STATUS2_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_STATUS2_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_STATUS2_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_STATUS2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_STATUS2_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_STATUS2_IN)
#define HWIO_CRYPTO0_CRYPTO_STATUS2_AXI_EXTRA_BMSK                                                    0x2
#define HWIO_CRYPTO0_CRYPTO_STATUS2_AXI_EXTRA_SHFT                                                    0x1
#define HWIO_CRYPTO0_CRYPTO_STATUS2_LOCKED_BMSK                                                       0x1
#define HWIO_CRYPTO0_CRYPTO_STATUS2_LOCKED_SHFT                                                       0x0

#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ADDR                                                 (CRYPTO0_CRYPTO_REG_BASE      + 0x00000108)
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_RMSK                                                 0x1fffffff
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ADDR, HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ZUC_ENABLE_BMSK                                      0x10000000
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ZUC_ENABLE_SHFT                                            0x1c
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_ZUC_SEL_BMSK                                     0x8000000
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_ZUC_SEL_SHFT                                          0x1b
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ENCR_ZUC_SEL_BMSK                                     0x4000000
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ENCR_ZUC_SEL_SHFT                                          0x1a
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_MAX_AXI_RD_BEATS_BMSK                                 0x3f80000
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_MAX_AXI_RD_BEATS_SHFT                                      0x13
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_MAX_AXI_WR_BEATS_BMSK                                   0x7e000
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_MAX_AXI_WR_BEATS_SHFT                                       0xd
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_NUM_BAM_PIPE_SETS_BMSK                                   0x1e00
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_NUM_BAM_PIPE_SETS_SHFT                                      0x9
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_KASUMI_SEL_BMSK                                      0x100
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_KASUMI_SEL_SHFT                                        0x8
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_SNOW3G_SEL_BMSK                                       0x80
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_SNOW3G_SEL_SHFT                                        0x7
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_AES_SEL_BMSK                                          0x40
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_AES_SEL_SHFT                                           0x6
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_SHA512_SEL_BMSK                                       0x20
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_SHA512_SEL_SHFT                                        0x5
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_SHA_SEL_BMSK                                          0x10
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_AUTH_SHA_SEL_SHFT                                           0x4
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ENCR_KASUMI_SEL_BMSK                                        0x8
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ENCR_KASUMI_SEL_SHFT                                        0x3
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ENCR_SNOW3G_SEL_BMSK                                        0x4
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ENCR_SNOW3G_SEL_SHFT                                        0x2
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ENCR_DES_SEL_BMSK                                           0x2
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ENCR_DES_SEL_SHFT                                           0x1
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ENCR_AES_SEL_BMSK                                           0x1
#define HWIO_CRYPTO0_CRYPTO_ENGINES_AVAIL_ENCR_AES_SEL_SHFT                                           0x0

#define HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_ADDR                                                    (CRYPTO0_CRYPTO_REG_BASE      + 0x0000010c)
#define HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_RMSK                                                        0xffff
#define HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_ADDR, HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_RMSK)
#define HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_ENG_DOUT_FIFO_DEPTH_BMSK                                    0xff00
#define HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_ENG_DOUT_FIFO_DEPTH_SHFT                                       0x8
#define HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_ENG_DIN_FIFO_DEPTH_BMSK                                       0xff
#define HWIO_CRYPTO0_CRYPTO_FIFO_SIZES_ENG_DIN_FIFO_DEPTH_SHFT                                        0x0

#define HWIO_CRYPTO0_CRYPTO_SEG_SIZE_ADDR                                                      (CRYPTO0_CRYPTO_REG_BASE      + 0x00000110)
#define HWIO_CRYPTO0_CRYPTO_SEG_SIZE_RMSK                                                      0xffffffff
#define HWIO_CRYPTO0_CRYPTO_SEG_SIZE_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_SEG_SIZE_ADDR, HWIO_CRYPTO0_CRYPTO_SEG_SIZE_RMSK)
#define HWIO_CRYPTO0_CRYPTO_SEG_SIZE_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_SEG_SIZE_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_SEG_SIZE_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_SEG_SIZE_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_SEG_SIZE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_SEG_SIZE_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_SEG_SIZE_IN)
#define HWIO_CRYPTO0_CRYPTO_SEG_SIZE_SEG_SIZE_BMSK                                             0xffffffff
#define HWIO_CRYPTO0_CRYPTO_SEG_SIZE_SEG_SIZE_SHFT                                                    0x0

#define HWIO_CRYPTO0_CRYPTO_GOPROC_ADDR                                                        (CRYPTO0_CRYPTO_REG_BASE      + 0x00000120)
#define HWIO_CRYPTO0_CRYPTO_GOPROC_RMSK                                                               0x7
#define HWIO_CRYPTO0_CRYPTO_GOPROC_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_GOPROC_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_GOPROC_RESULTS_DUMP_BMSK                                                  0x4
#define HWIO_CRYPTO0_CRYPTO_GOPROC_RESULTS_DUMP_SHFT                                                  0x2
#define HWIO_CRYPTO0_CRYPTO_GOPROC_CLR_CNTXT_BMSK                                                     0x2
#define HWIO_CRYPTO0_CRYPTO_GOPROC_CLR_CNTXT_SHFT                                                     0x1
#define HWIO_CRYPTO0_CRYPTO_GOPROC_GO_BMSK                                                            0x1
#define HWIO_CRYPTO0_CRYPTO_GOPROC_GO_SHFT                                                            0x0

#define HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_ADDR                                                 (CRYPTO0_CRYPTO_REG_BASE      + 0x00001000)
#define HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_RMSK                                                        0x7
#define HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_RESULTS_DUMP_BMSK                                           0x4
#define HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_RESULTS_DUMP_SHFT                                           0x2
#define HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_CLR_CNTXT_BMSK                                              0x2
#define HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_CLR_CNTXT_SHFT                                              0x1
#define HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_GO_BMSK                                                     0x1
#define HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_GO_SHFT                                                     0x0

#define HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_ADDR                                                (CRYPTO0_CRYPTO_REG_BASE      + 0x00002000)
#define HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_RMSK                                                       0x7
#define HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_RESULTS_DUMP_BMSK                                          0x4
#define HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_RESULTS_DUMP_SHFT                                          0x2
#define HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_CLR_CNTXT_BMSK                                             0x2
#define HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_CLR_CNTXT_SHFT                                             0x1
#define HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_GO_BMSK                                                    0x1
#define HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_GO_SHFT                                                    0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ADDR                                                  (CRYPTO0_CRYPTO_REG_BASE      + 0x00000200)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_RMSK                                                     0x7ffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ODD_KEY_SEL_BMSK                                         0x40000
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ODD_KEY_SEL_SHFT                                            0x12
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_KEYSTREAM_ENABLE_BMSK                                    0x20000
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_KEYSTREAM_ENABLE_SHFT                                       0x11
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_F8_DIRECTION_BMSK                                        0x10000
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_F8_DIRECTION_SHFT                                           0x10
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_USE_PIPE_KEY_ENCR_BMSK                                    0x8000
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_USE_PIPE_KEY_ENCR_SHFT                                       0xf
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_USE_HW_KEY_ENCR_BMSK                                      0x4000
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_USE_HW_KEY_ENCR_SHFT                                         0xe
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_LAST_BMSK                                                 0x2000
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_LAST_SHFT                                                    0xd
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_CNTR_ALG_BMSK                                             0x1800
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_CNTR_ALG_SHFT                                                0xb
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCODE_BMSK                                                0x400
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCODE_SHFT                                                  0xa
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_MODE_BMSK                                             0x3c0
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_MODE_SHFT                                               0x6
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_KEY_SZ_BMSK                                            0x38
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_KEY_SZ_SHFT                                             0x3
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_ALG_BMSK                                                0x7
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_ALG_SHFT                                                0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_ADDR                                                 (CRYPTO0_CRYPTO_REG_BASE      + 0x00000204)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_RMSK                                                 0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_ENCR_SIZE_BMSK                                       0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_ENCR_SIZE_SHFT                                              0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_ADDR                                                (CRYPTO0_CRYPTO_REG_BASE      + 0x00000208)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_RMSK                                                0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_ENCR_START_BMSK                                     0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_SEG_START_ENCR_START_SHFT                                            0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_KEYn_ADDR(n)                                                  (CRYPTO0_CRYPTO_REG_BASE      + 0x00003000 + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_ENCR_KEYn_RMSK                                                     0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_KEYn_MAXn                                                              7
#define HWIO_CRYPTO0_CRYPTO_ENCR_KEYn_OUTI(n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_KEYn_ADDR(n),val)
#define HWIO_CRYPTO0_CRYPTO_ENCR_KEYn_CRYPTO_ENCR_KEY_BMSK                                     0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_KEYn_CRYPTO_ENCR_KEY_SHFT                                            0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_KEYn_ADDR(m,n)                                          (CRYPTO0_CRYPTO_REG_BASE      + 0x00004000 + 0x20 * (m) + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_KEYn_RMSK                                               0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_KEYn_MAXm                                                        3
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_KEYn_MAXn                                                        7
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_KEYn_OUTI2(m,n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_KEYn_ADDR(m,n),val)
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_KEYn_CRYPTO_ENCR_PIPE_KEY_BMSK                          0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_KEYn_CRYPTO_ENCR_PIPE_KEY_SHFT                                 0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_KEYn_ADDR(n)                                              (CRYPTO0_CRYPTO_REG_BASE      + 0x00003020 + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_KEYn_RMSK                                                 0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_KEYn_MAXn                                                          7
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_KEYn_OUTI(n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_XTS_KEYn_ADDR(n),val)
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_KEYn_CRYPTO_ENCR_XTS_KEY_BMSK                             0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_KEYn_CRYPTO_ENCR_XTS_KEY_SHFT                                    0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_KEYn_ADDR(m,n)                                      (CRYPTO0_CRYPTO_REG_BASE      + 0x00004200 + 0x20 * (m) + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_KEYn_RMSK                                           0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_KEYn_MAXm                                                    3
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_KEYn_MAXn                                                    7
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_KEYn_OUTI2(m,n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_KEYn_ADDR(m,n),val)
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_KEYn_CRYPTO_ENCR_PIPE_XTS_KEY_BMSK                  0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_KEYn_CRYPTO_ENCR_PIPE_XTS_KEY_SHFT                         0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_ODD_KEYn_ADDR(m,n)                                      (CRYPTO0_CRYPTO_REG_BASE      + 0x00004400 + 0x20 * (m) + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_ODD_KEYn_RMSK                                           0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_ODD_KEYn_MAXm                                                    3
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_ODD_KEYn_MAXn                                                    7
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_ODD_KEYn_OUTI2(m,n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_ODD_KEYn_ADDR(m,n),val)
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_ODD_KEYn_CRYPTO_ENCR_PIPE_ODD_KEY_BMSK                  0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_ODD_KEYn_CRYPTO_ENCR_PIPE_ODD_KEY_SHFT                         0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_ODD_KEYn_ADDR(m,n)                                  (CRYPTO0_CRYPTO_REG_BASE      + 0x00004600 + 0x20 * (m) + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_ODD_KEYn_RMSK                                       0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_ODD_KEYn_MAXm                                                3
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_ODD_KEYn_MAXn                                                7
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_ODD_KEYn_OUTI2(m,n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_ODD_KEYn_ADDR(m,n),val)
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_ODD_KEYn_CRYPTO_ENCR_PIPE_XTS_ODD_KEY_BMSK          0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_PIPEm_XTS_ODD_KEYn_CRYPTO_ENCR_PIPE_XTS_ODD_KEY_SHFT                 0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_ADDR                                                (CRYPTO0_CRYPTO_REG_BASE      + 0x0000020c)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_RMSK                                                0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_CRYPTO_CNTR0_IV0_BMSK                               0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR0_IV0_CRYPTO_CNTR0_IV0_SHFT                                      0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_ADDR                                                (CRYPTO0_CRYPTO_REG_BASE      + 0x00000210)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_RMSK                                                0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_CRYPTO_CNTR1_IV1_BMSK                               0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR1_IV1_CRYPTO_CNTR1_IV1_SHFT                                      0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_ADDR                                                (CRYPTO0_CRYPTO_REG_BASE      + 0x00000214)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_RMSK                                                0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_CRYPTO_CNTR2_IV2_BMSK                               0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR2_IV2_CRYPTO_CNTR2_IV2_SHFT                                      0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_ADDR                                                (CRYPTO0_CRYPTO_REG_BASE      + 0x00000218)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_RMSK                                                0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_CRYPTO_CNTR3_IV3_BMSK                               0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR3_IV3_CRYPTO_CNTR3_IV3_SHFT                                      0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_ADDR                                                (CRYPTO0_CRYPTO_REG_BASE      + 0x0000021c)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_RMSK                                                0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_CNTR_MASK_BMSK                                      0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK_CNTR_MASK_SHFT                                             0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_ADDR                                               (CRYPTO0_CRYPTO_REG_BASE      + 0x00000234)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_RMSK                                               0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_CNTR_MASK_BMSK                                     0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK2_CNTR_MASK_SHFT                                            0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_ADDR                                               (CRYPTO0_CRYPTO_REG_BASE      + 0x00000238)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_RMSK                                               0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_CNTR_MASK_BMSK                                     0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK1_CNTR_MASK_SHFT                                            0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_ADDR                                               (CRYPTO0_CRYPTO_REG_BASE      + 0x0000023c)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_RMSK                                               0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_CNTR_MASK_BMSK                                     0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CNTR_MASK0_CNTR_MASK_SHFT                                            0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_ADDR(n)                                        (CRYPTO0_CRYPTO_REG_BASE      + 0x00000220 + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_RMSK                                           0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_MAXn                                                    3
#define HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_INI(n)        \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_ADDR(n), HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_INMI(n,mask)    \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_ADDR(n), mask)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_OUTI(n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_ADDR(n),val)
#define HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_ADDR(n),mask,val,HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_INI(n))
#define HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_CCM_INIT_CNTR_BMSK                             0xffffffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_CCM_INIT_CNTRn_CCM_INIT_CNTR_SHFT                                    0x0

#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_ADDR                                              (CRYPTO0_CRYPTO_REG_BASE      + 0x00000230)
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_RMSK                                                 0xfffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_ADDR, HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_RMSK)
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_IN)
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_DU_SIZE_BMSK                                         0xfffff
#define HWIO_CRYPTO0_CRYPTO_ENCR_XTS_DU_SIZE_DU_SIZE_SHFT                                             0x0

#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_ADDR                                                  (CRYPTO0_CRYPTO_REG_BASE      + 0x00000300)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_RMSK                                                   0x1ffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_ADDR, HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_RMSK)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_IN)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_COMP_EXP_MAC_BMSK                                      0x1000000
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_COMP_EXP_MAC_SHFT                                           0x18
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_F9_DIRECTION_BMSK                                       0x800000
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_F9_DIRECTION_SHFT                                           0x17
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_NONCE_NUM_WORDS_BMSK                               0x700000
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_NONCE_NUM_WORDS_SHFT                                   0x14
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_USE_PIPE_KEY_AUTH_BMSK                                   0x80000
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_USE_PIPE_KEY_AUTH_SHFT                                      0x13
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_USE_HW_KEY_AUTH_BMSK                                     0x40000
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_USE_HW_KEY_AUTH_SHFT                                        0x12
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_FIRST_BMSK                                               0x20000
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_FIRST_SHFT                                                  0x11
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_LAST_BMSK                                                0x10000
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_LAST_SHFT                                                   0x10
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_POS_BMSK                                             0xc000
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_POS_SHFT                                                0xe
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_SIZE_BMSK                                            0x3e00
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_SIZE_SHFT                                               0x9
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_MODE_BMSK                                             0x1c0
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_MODE_SHFT                                               0x6
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_KEY_SZ_BMSK                                            0x38
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_KEY_SZ_SHFT                                             0x3
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_ALG_BMSK                                                0x7
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_ALG_SHFT                                                0x0

#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_ADDR                                                 (CRYPTO0_CRYPTO_REG_BASE      + 0x00000304)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_RMSK                                                 0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_ADDR, HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_RMSK)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_IN)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_AUTH_SIZE_BMSK                                       0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_AUTH_SIZE_SHFT                                              0x0

#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_ADDR                                                (CRYPTO0_CRYPTO_REG_BASE      + 0x00000308)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_RMSK                                                0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_ADDR, HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_RMSK)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_IN)
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_AUTH_START_BMSK                                     0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_SEG_START_AUTH_START_SHFT                                            0x0

#define HWIO_CRYPTO0_CRYPTO_AUTH_KEYn_ADDR(n)                                                  (CRYPTO0_CRYPTO_REG_BASE      + 0x00003040 + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_AUTH_KEYn_RMSK                                                     0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_KEYn_MAXn                                                             15
#define HWIO_CRYPTO0_CRYPTO_AUTH_KEYn_OUTI(n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_AUTH_KEYn_ADDR(n),val)
#define HWIO_CRYPTO0_CRYPTO_AUTH_KEYn_CRYPTO_AUTH_KEY_BMSK                                     0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_KEYn_CRYPTO_AUTH_KEY_SHFT                                            0x0

#define HWIO_CRYPTO0_CRYPTO_AUTH_PIPEm_KEYn_ADDR(m,n)                                          (CRYPTO0_CRYPTO_REG_BASE      + 0x00004800 + 0x80 * (m) + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_AUTH_PIPEm_KEYn_RMSK                                               0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_PIPEm_KEYn_MAXm                                                        3
#define HWIO_CRYPTO0_CRYPTO_AUTH_PIPEm_KEYn_MAXn                                                       15
#define HWIO_CRYPTO0_CRYPTO_AUTH_PIPEm_KEYn_OUTI2(m,n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_AUTH_PIPEm_KEYn_ADDR(m,n),val)
#define HWIO_CRYPTO0_CRYPTO_AUTH_PIPEm_KEYn_CRYPTO_AUTH_PIPE_KEY_BMSK                          0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_PIPEm_KEYn_CRYPTO_AUTH_PIPE_KEY_SHFT                                 0x0

#define HWIO_CRYPTO0_CRYPTO_AUTH_IVn_ADDR(n)                                                   (CRYPTO0_CRYPTO_REG_BASE      + 0x00000310 + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_AUTH_IVn_RMSK                                                      0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_IVn_MAXn                                                               7
#define HWIO_CRYPTO0_CRYPTO_AUTH_IVn_INI(n)        \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_IVn_ADDR(n), HWIO_CRYPTO0_CRYPTO_AUTH_IVn_RMSK)
#define HWIO_CRYPTO0_CRYPTO_AUTH_IVn_INMI(n,mask)    \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_IVn_ADDR(n), mask)
#define HWIO_CRYPTO0_CRYPTO_AUTH_IVn_OUTI(n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_AUTH_IVn_ADDR(n),val)
#define HWIO_CRYPTO0_CRYPTO_AUTH_IVn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_AUTH_IVn_ADDR(n),mask,val,HWIO_CRYPTO0_CRYPTO_AUTH_IVn_INI(n))
#define HWIO_CRYPTO0_CRYPTO_AUTH_IVn_AUTH_IVN_BMSK                                             0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_IVn_AUTH_IVN_SHFT                                                    0x0

#define HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_ADDR(n)                                           (CRYPTO0_CRYPTO_REG_BASE      + 0x00000350 + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_RMSK                                              0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_MAXn                                                       3
#define HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_INI(n)        \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_ADDR(n), HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_RMSK)
#define HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_INMI(n,mask)    \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_ADDR(n), mask)
#define HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_OUTI(n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_ADDR(n),val)
#define HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_ADDR(n),mask,val,HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_INI(n))
#define HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_CTRL_INFO_NONCE_BMSK                              0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_INFO_NONCEn_CTRL_INFO_NONCE_SHFT                                     0x0

#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_ADDR                                                 (CRYPTO0_CRYPTO_REG_BASE      + 0x00000390)
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_RMSK                                                 0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_ADDR, HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_RMSK)
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_IN)
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_AUTH_BYTECNT0_BMSK                                   0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT0_AUTH_BYTECNT0_SHFT                                          0x0

#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_ADDR                                                 (CRYPTO0_CRYPTO_REG_BASE      + 0x00000394)
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_RMSK                                                 0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_ADDR, HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_RMSK)
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_IN)
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_AUTH_BYTECNT1_BMSK                                   0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_BYTECNT1_AUTH_BYTECNT1_SHFT                                          0x0

#define HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_ADDR(n)                                              (CRYPTO0_CRYPTO_REG_BASE      + 0x000003a0 + 0x4 * (n))
#define HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_RMSK                                                 0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_MAXn                                                          7
#define HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_INI(n)        \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_ADDR(n), HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_RMSK)
#define HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_INMI(n,mask)    \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_ADDR(n), mask)
#define HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_OUTI(n,val)    \
        out_dword(HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_ADDR(n),val)
#define HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_ADDR(n),mask,val,HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_INI(n))
#define HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_EXP_MAC_BMSK                                         0xffffffff
#define HWIO_CRYPTO0_CRYPTO_AUTH_EXP_MACn_EXP_MAC_SHFT                                                0x0

#define HWIO_CRYPTO0_CRYPTO_CONFIG_ADDR                                                        (CRYPTO0_CRYPTO_REG_BASE      + 0x00000400)
#define HWIO_CRYPTO0_CRYPTO_CONFIG_RMSK                                                          0x1fffff
#define HWIO_CRYPTO0_CRYPTO_CONFIG_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_CONFIG_ADDR, HWIO_CRYPTO0_CRYPTO_CONFIG_RMSK)
#define HWIO_CRYPTO0_CRYPTO_CONFIG_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_CONFIG_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_CONFIG_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_CONFIG_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_CONFIG_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_CONFIG_IN)
#define HWIO_CRYPTO0_CRYPTO_CONFIG_REQ_SIZE_BMSK                                                 0x1e0000
#define HWIO_CRYPTO0_CRYPTO_CONFIG_REQ_SIZE_SHFT                                                     0x11
#define HWIO_CRYPTO0_CRYPTO_CONFIG_MAX_QUEUED_REQS_BMSK                                           0x1c000
#define HWIO_CRYPTO0_CRYPTO_CONFIG_MAX_QUEUED_REQS_SHFT                                               0xe
#define HWIO_CRYPTO0_CRYPTO_CONFIG_IRQ_ENABLE_BMSK                                                 0x3c00
#define HWIO_CRYPTO0_CRYPTO_CONFIG_IRQ_ENABLE_SHFT                                                    0xa
#define HWIO_CRYPTO0_CRYPTO_CONFIG_LITTLE_ENDIAN_MODE_BMSK                                          0x200
#define HWIO_CRYPTO0_CRYPTO_CONFIG_LITTLE_ENDIAN_MODE_SHFT                                            0x9
#define HWIO_CRYPTO0_CRYPTO_CONFIG_PIPE_SET_SELECT_BMSK                                             0x1e0
#define HWIO_CRYPTO0_CRYPTO_CONFIG_PIPE_SET_SELECT_SHFT                                               0x5
#define HWIO_CRYPTO0_CRYPTO_CONFIG_HIGH_SPD_DATA_EN_N_BMSK                                           0x10
#define HWIO_CRYPTO0_CRYPTO_CONFIG_HIGH_SPD_DATA_EN_N_SHFT                                            0x4
#define HWIO_CRYPTO0_CRYPTO_CONFIG_MASK_DOUT_INTR_BMSK                                                0x8
#define HWIO_CRYPTO0_CRYPTO_CONFIG_MASK_DOUT_INTR_SHFT                                                0x3
#define HWIO_CRYPTO0_CRYPTO_CONFIG_MASK_DIN_INTR_BMSK                                                 0x4
#define HWIO_CRYPTO0_CRYPTO_CONFIG_MASK_DIN_INTR_SHFT                                                 0x2
#define HWIO_CRYPTO0_CRYPTO_CONFIG_MASK_OP_DONE_INTR_BMSK                                             0x2
#define HWIO_CRYPTO0_CRYPTO_CONFIG_MASK_OP_DONE_INTR_SHFT                                             0x1
#define HWIO_CRYPTO0_CRYPTO_CONFIG_MASK_ERR_INTR_BMSK                                                 0x1
#define HWIO_CRYPTO0_CRYPTO_CONFIG_MASK_ERR_INTR_SHFT                                                 0x0

#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_ADDR                                                  (CRYPTO0_CRYPTO_REG_BASE      + 0x00005000)
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_RMSK                                                       0x3ff
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_ADDR, HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_RMSK)
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_IN)
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_DBG_EN_BMSK                                                0x200
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_DBG_EN_SHFT                                                  0x9
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_MAXI2AXI_DBG_SEL_BMSK                                      0x1c0
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_MAXI2AXI_DBG_SEL_SHFT                                        0x6
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_DBG_SEL_BMSK                                                0x3f
#define HWIO_CRYPTO0_CRYPTO_DEBUG_ENABLE_DBG_SEL_SHFT                                                 0x0

#define HWIO_CRYPTO0_CRYPTO_DEBUG_ADDR                                                         (CRYPTO0_CRYPTO_REG_BASE      + 0x00005004)
#define HWIO_CRYPTO0_CRYPTO_DEBUG_RMSK                                                         0xffffffff
#define HWIO_CRYPTO0_CRYPTO_DEBUG_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_DEBUG_ADDR, HWIO_CRYPTO0_CRYPTO_DEBUG_RMSK)
#define HWIO_CRYPTO0_CRYPTO_DEBUG_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_DEBUG_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_DEBUG_DEBUG_DISABLED_BMSK                                          0xffffffff
#define HWIO_CRYPTO0_CRYPTO_DEBUG_DEBUG_DISABLED_SHFT                                                 0x0

#define HWIO_CRYPTO0_CRYPTO_PWR_CTRL_ADDR                                                      (CRYPTO0_CRYPTO_REG_BASE      + 0x00005008)
#define HWIO_CRYPTO0_CRYPTO_PWR_CTRL_RMSK                                                             0x1
#define HWIO_CRYPTO0_CRYPTO_PWR_CTRL_IN          \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_PWR_CTRL_ADDR, HWIO_CRYPTO0_CRYPTO_PWR_CTRL_RMSK)
#define HWIO_CRYPTO0_CRYPTO_PWR_CTRL_INM(m)      \
        in_dword_masked(HWIO_CRYPTO0_CRYPTO_PWR_CTRL_ADDR, m)
#define HWIO_CRYPTO0_CRYPTO_PWR_CTRL_OUT(v)      \
        out_dword(HWIO_CRYPTO0_CRYPTO_PWR_CTRL_ADDR,v)
#define HWIO_CRYPTO0_CRYPTO_PWR_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CRYPTO0_CRYPTO_PWR_CTRL_ADDR,m,v,HWIO_CRYPTO0_CRYPTO_PWR_CTRL_IN)
#define HWIO_CRYPTO0_CRYPTO_PWR_CTRL_AUTO_SHUTDOWN_EN_BMSK                                            0x1
#define HWIO_CRYPTO0_CRYPTO_PWR_CTRL_AUTO_SHUTDOWN_EN_SHFT                                            0x0


#endif /* __CE0_MSMHWIOREG_H__ */
