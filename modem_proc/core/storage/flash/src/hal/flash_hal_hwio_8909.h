#ifndef __FLASH_HAL_HWIO_8X09_H__
#define __FLASH_HAL_HWIO_8X09_H__
/*
===========================================================================
*/
/**
  @file flash_hal_hwio_8x09.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    QPIC_EBI2ND
    QPIC_EBI2CR

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

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/flash_hal_hwio_8909.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: EBI2ND
 *--------------------------------------------------------------------------*/

#ifdef ENABLE_FLASH_BAM
  #define EBI2ND_REG_BASE                                                           0x00030000
  #define EBI2ND_REG_BASE_PHYS                                                      0x00030000
  #define EBI2ND_REG_BASE_OFFS                                                      0x00030000
#else
  #define EBI2ND_REG_BASE                                                           (QPIC_QPIC_BASE      + 0x00030000)
  #define EBI2ND_REG_BASE_PHYS                                                      (QPIC_QPIC_BASE_PHYS + 0x00030000)
  #define EBI2ND_REG_BASE_OFFS                                                      0x00030000
#endif

#define EBI2ND_REG_BASE_DCPU                                                        (QPIC_QPIC_BASE      + 0x00030000)
#define EBI2ND_REG_BASE_PHYS_DCPU                                                   (QPIC_QPIC_BASE_PHYS + 0x00030000)
#define EBI2ND_REG_BASE_OFFS_DCPU                                                   0x00030000

/*----------------------------------------------------------------------------
 * MODULE: EBI2ND
 *--------------------------------------------------------------------------*/

#define HWIO_NAND_FLASH_CMD_ADDR                                                  (EBI2ND_REG_BASE      + 0x00000000)
#define HWIO_NAND_FLASH_CMD_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x00000000)
#define HWIO_NAND_FLASH_CMD_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x00000000)
#define HWIO_NAND_FLASH_CMD_RMSK                                                     0x71fffff
#define HWIO_NAND_FLASH_CMD_IN          \
        in_dword_masked(HWIO_NAND_FLASH_CMD_ADDR, HWIO_NAND_FLASH_CMD_RMSK)
#define HWIO_NAND_FLASH_CMD_INM(m)      \
        in_dword_masked(HWIO_NAND_FLASH_CMD_ADDR, m)
#define HWIO_NAND_FLASH_CMD_OUT(v)      \
        out_dword(HWIO_NAND_FLASH_CMD_ADDR,v)
#define HWIO_NAND_FLASH_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_FLASH_CMD_ADDR,m,v,HWIO_NAND_FLASH_CMD_IN)
#define HWIO_NAND_FLASH_CMD_READ_CACHE_LAST_BMSK                                        0x4000000
#define HWIO_NAND_FLASH_CMD_READ_CACHE_LAST_SHFT                                             0x1a
#define HWIO_NAND_FLASH_CMD_READ_CACHE_SEQ_BMSK                                         0x2000000
#define HWIO_NAND_FLASH_CMD_READ_CACHE_SEQ_SHFT                                              0x19
#define HWIO_NAND_FLASH_CMD_EN_READ_CACHE_NEXT_CMD_BMSK                                 0x1000000
#define HWIO_NAND_FLASH_CMD_EN_READ_CACHE_NEXT_CMD_SHFT                                      0x18
#define HWIO_NAND_FLASH_CMD_EN_PROGRAM_PAGE_CACHE_NEXT_CMD_BMSK                          0x100000
#define HWIO_NAND_FLASH_CMD_EN_PROGRAM_PAGE_CACHE_NEXT_CMD_SHFT                              0x14
#define HWIO_NAND_FLASH_CMD_EXTENDED_FETCH_ID_BMSK                                        0x80000
#define HWIO_NAND_FLASH_CMD_EXTENDED_FETCH_ID_SHFT                                           0x13
#define HWIO_NAND_FLASH_CMD_ONE_NAND_INTR_STATUS_BMSK                                     0x40000
#define HWIO_NAND_FLASH_CMD_ONE_NAND_INTR_STATUS_SHFT                                        0x12
#define HWIO_NAND_FLASH_CMD_ONE_NAND_HOST_CFG_BMSK                                        0x20000
#define HWIO_NAND_FLASH_CMD_ONE_NAND_HOST_CFG_SHFT                                           0x11
#define HWIO_NAND_FLASH_CMD_AUTO_DETECT_DATA_XFR_SIZE_BMSK                                0x1ff80
#define HWIO_NAND_FLASH_CMD_AUTO_DETECT_DATA_XFR_SIZE_SHFT                                    0x7
#define HWIO_NAND_FLASH_CMD_AUTO_DETECT_BMSK                                                 0x40
#define HWIO_NAND_FLASH_CMD_AUTO_DETECT_SHFT                                                  0x6
#define HWIO_NAND_FLASH_CMD_LAST_PAGE_BMSK                                                   0x20
#define HWIO_NAND_FLASH_CMD_LAST_PAGE_SHFT                                                    0x5
#define HWIO_NAND_FLASH_CMD_PAGE_ACC_BMSK                                                    0x10
#define HWIO_NAND_FLASH_CMD_PAGE_ACC_SHFT                                                     0x4
#define HWIO_NAND_FLASH_CMD_PAGE_ACC_NON_PAGE_ACCESS_COMMAND_FVAL                             0x0
#define HWIO_NAND_FLASH_CMD_PAGE_ACC_PAGE_ACCESS_COMMAND_FVAL                                 0x1
#define HWIO_NAND_FLASH_CMD_OP_CMD_BMSK                                                       0xf
#define HWIO_NAND_FLASH_CMD_OP_CMD_SHFT                                                       0x0
#define HWIO_NAND_FLASH_CMD_OP_CMD_RESERVED_0_FVAL                                            0x0
#define HWIO_NAND_FLASH_CMD_OP_CMD_ABORT_TRANSACTION_FVAL                                     0x1
#define HWIO_NAND_FLASH_CMD_OP_CMD_PAGE_READ_FVAL                                             0x2
#define HWIO_NAND_FLASH_CMD_OP_CMD_PAGE_READ_WITH_ECC_FVAL                                    0x3
#define HWIO_NAND_FLASH_CMD_OP_CMD_PAGE_READ_WITH_ECC_SPARE_FVAL                              0x4
#define HWIO_NAND_FLASH_CMD_OP_CMD_RESERVED_5_FVAL                                            0x5
#define HWIO_NAND_FLASH_CMD_OP_CMD_PROGRAM_PAGE_FVAL                                          0x6
#define HWIO_NAND_FLASH_CMD_OP_CMD_PAGE_PROGRAM_WITH_ECC_FVAL                                 0x7
#define HWIO_NAND_FLASH_CMD_OP_CMD_RESERVED_8_FVAL                                            0x8
#define HWIO_NAND_FLASH_CMD_OP_CMD_PROGRAM_PAGE_WITH_SPARE_FVAL                               0x9
#define HWIO_NAND_FLASH_CMD_OP_CMD_BLOCK_ERASE_FVAL                                           0xa
#define HWIO_NAND_FLASH_CMD_OP_CMD_FETCH_ID_FVAL                                              0xb
#define HWIO_NAND_FLASH_CMD_OP_CMD_CHECK_STATUS_FVAL                                          0xc
#define HWIO_NAND_FLASH_CMD_OP_CMD_RESET_NAND_FLASH_DEVICE_OR_ONENAND_REGISTER_WRITE_FVAL        0xd
#define HWIO_NAND_FLASH_CMD_OP_CMD_RESERVED_E_FVAL                                            0xe
#define HWIO_NAND_FLASH_CMD_OP_CMD_RESERVED_F_FVAL                                            0xf

#define HWIO_NAND_ADDR0_ADDR                                                      (EBI2ND_REG_BASE      + 0x00000004)
#define HWIO_NAND_ADDR0_PHYS                                                      (EBI2ND_REG_BASE_PHYS + 0x00000004)
#define HWIO_NAND_ADDR0_OFFS                                                      (EBI2ND_REG_BASE_OFFS + 0x00000004)
#define HWIO_NAND_ADDR0_RMSK                                                      0xffffffff
#define HWIO_NAND_ADDR0_IN          \
        in_dword_masked(HWIO_NAND_ADDR0_ADDR, HWIO_NAND_ADDR0_RMSK)
#define HWIO_NAND_ADDR0_INM(m)      \
        in_dword_masked(HWIO_NAND_ADDR0_ADDR, m)
#define HWIO_NAND_ADDR0_OUT(v)      \
        out_dword(HWIO_NAND_ADDR0_ADDR,v)
#define HWIO_NAND_ADDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_ADDR0_ADDR,m,v,HWIO_NAND_ADDR0_IN)
#define HWIO_NAND_ADDR0_DEV_ADDR0_BMSK                                                 0xffffffff
#define HWIO_NAND_ADDR0_DEV_ADDR0_SHFT                                                        0x0

#define HWIO_NAND_ADDR1_ADDR                                                      (EBI2ND_REG_BASE      + 0x00000008)
#define HWIO_NAND_ADDR1_PHYS                                                      (EBI2ND_REG_BASE_PHYS + 0x00000008)
#define HWIO_NAND_ADDR1_OFFS                                                      (EBI2ND_REG_BASE_OFFS + 0x00000008)
#define HWIO_NAND_ADDR1_RMSK                                                      0xffffffff
#define HWIO_NAND_ADDR1_IN          \
        in_dword_masked(HWIO_NAND_ADDR1_ADDR, HWIO_NAND_ADDR1_RMSK)
#define HWIO_NAND_ADDR1_INM(m)      \
        in_dword_masked(HWIO_NAND_ADDR1_ADDR, m)
#define HWIO_NAND_ADDR1_OUT(v)      \
        out_dword(HWIO_NAND_ADDR1_ADDR,v)
#define HWIO_NAND_ADDR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_ADDR1_ADDR,m,v,HWIO_NAND_ADDR1_IN)
#define HWIO_NAND_ADDR1_DEV_ADDR1_BMSK                                                 0xffffffff
#define HWIO_NAND_ADDR1_DEV_ADDR1_SHFT                                                        0x0

#define HWIO_NAND_FLASH_CHIP_SELECT_ADDR                                          (EBI2ND_REG_BASE      + 0x0000000c)
#define HWIO_NAND_FLASH_CHIP_SELECT_PHYS                                          (EBI2ND_REG_BASE_PHYS + 0x0000000c)
#define HWIO_NAND_FLASH_CHIP_SELECT_OFFS                                          (EBI2ND_REG_BASE_OFFS + 0x0000000c)
#define HWIO_NAND_FLASH_CHIP_SELECT_RMSK                                                0x6f
#define HWIO_NAND_FLASH_CHIP_SELECT_IN          \
        in_dword_masked(HWIO_NAND_FLASH_CHIP_SELECT_ADDR, HWIO_NAND_FLASH_CHIP_SELECT_RMSK)
#define HWIO_NAND_FLASH_CHIP_SELECT_INM(m)      \
        in_dword_masked(HWIO_NAND_FLASH_CHIP_SELECT_ADDR, m)
#define HWIO_NAND_FLASH_CHIP_SELECT_OUT(v)      \
        out_dword(HWIO_NAND_FLASH_CHIP_SELECT_ADDR,v)
#define HWIO_NAND_FLASH_CHIP_SELECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_FLASH_CHIP_SELECT_ADDR,m,v,HWIO_NAND_FLASH_CHIP_SELECT_IN)
#define HWIO_NAND_FLASH_CHIP_SELECT_RESET_XFR_STEP_LOAD_DONE_STATUS_BMSK                     0x40
#define HWIO_NAND_FLASH_CHIP_SELECT_RESET_XFR_STEP_LOAD_DONE_STATUS_SHFT                      0x6
#define HWIO_NAND_FLASH_CHIP_SELECT_XFR_STEP2_SAFE_REG_EN_BMSK                               0x20
#define HWIO_NAND_FLASH_CHIP_SELECT_XFR_STEP2_SAFE_REG_EN_SHFT                                0x5
#define HWIO_NAND_FLASH_CHIP_SELECT_XFR_STEP2_SAFE_REG_EN_DYNAMICALLY_CHANGING_THE_XFR_STEP2_IS_ENABLED_FVAL        0x1
#define HWIO_NAND_FLASH_CHIP_SELECT_ONE_NAND_EN_BMSK                                          0x8
#define HWIO_NAND_FLASH_CHIP_SELECT_ONE_NAND_EN_SHFT                                          0x3
#define HWIO_NAND_FLASH_CHIP_SELECT_DM_EN_BMSK                                                0x4
#define HWIO_NAND_FLASH_CHIP_SELECT_DM_EN_SHFT                                                0x2
#define HWIO_NAND_FLASH_CHIP_SELECT_PARTIAL_XFR_BMSK                                          0x2
#define HWIO_NAND_FLASH_CHIP_SELECT_PARTIAL_XFR_SHFT                                          0x1
#define HWIO_NAND_FLASH_CHIP_SELECT_PARTIAL_XFR_DISABLE_FVAL                                  0x0
#define HWIO_NAND_FLASH_CHIP_SELECT_PARTIAL_XFR_ENABLE_FVAL                                   0x1
#define HWIO_NAND_FLASH_CHIP_SELECT_NAND_DEV_SEL_BMSK                                         0x1
#define HWIO_NAND_FLASH_CHIP_SELECT_NAND_DEV_SEL_SHFT                                         0x0
#define HWIO_NAND_FLASH_CHIP_SELECT_NAND_DEV_SEL_NAND_CS0_IS_SELECTED_FVAL                    0x0
#define HWIO_NAND_FLASH_CHIP_SELECT_NAND_DEV_SEL_NAND_CS1_IS_SELECTED_FVAL                    0x1

#define HWIO_NANDC_EXEC_CMD_ADDR                                                  (EBI2ND_REG_BASE      + 0x00000010)
#define HWIO_NANDC_EXEC_CMD_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x00000010)
#define HWIO_NANDC_EXEC_CMD_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x00000010)
#define HWIO_NANDC_EXEC_CMD_RMSK                                                         0x1
#define HWIO_NANDC_EXEC_CMD_IN          \
        in_dword_masked(HWIO_NANDC_EXEC_CMD_ADDR, HWIO_NANDC_EXEC_CMD_RMSK)
#define HWIO_NANDC_EXEC_CMD_INM(m)      \
        in_dword_masked(HWIO_NANDC_EXEC_CMD_ADDR, m)
#define HWIO_NANDC_EXEC_CMD_OUT(v)      \
        out_dword(HWIO_NANDC_EXEC_CMD_ADDR,v)
#define HWIO_NANDC_EXEC_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NANDC_EXEC_CMD_ADDR,m,v,HWIO_NANDC_EXEC_CMD_IN)
#define HWIO_NANDC_EXEC_CMD_EXEC_CMD_BMSK                                                     0x1
#define HWIO_NANDC_EXEC_CMD_EXEC_CMD_SHFT                                                     0x0
#define HWIO_NANDC_EXEC_CMD_EXEC_CMD_EXECUTE_THE_COMMAND_FVAL                                 0x1

#define HWIO_NAND_FLASH_STATUS_ADDR                                               (EBI2ND_REG_BASE      + 0x00000014)
#define HWIO_NAND_FLASH_STATUS_PHYS                                               (EBI2ND_REG_BASE_PHYS + 0x00000014)
#define HWIO_NAND_FLASH_STATUS_OFFS                                               (EBI2ND_REG_BASE_OFFS + 0x00000014)
#define HWIO_NAND_FLASH_STATUS_RMSK                                               0xffffffff
#define HWIO_NAND_FLASH_STATUS_IN          \
        in_dword_masked(HWIO_NAND_FLASH_STATUS_ADDR, HWIO_NAND_FLASH_STATUS_RMSK)
#define HWIO_NAND_FLASH_STATUS_INM(m)      \
        in_dword_masked(HWIO_NAND_FLASH_STATUS_ADDR, m)
#define HWIO_NAND_FLASH_STATUS_OUT(v)      \
        out_dword(HWIO_NAND_FLASH_STATUS_ADDR,v)
#define HWIO_NAND_FLASH_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_FLASH_STATUS_ADDR,m,v,HWIO_NAND_FLASH_STATUS_IN)
#define HWIO_NAND_FLASH_STATUS_DEV_STATUS_BMSK                                         0xffff0000
#define HWIO_NAND_FLASH_STATUS_DEV_STATUS_SHFT                                               0x10
#define HWIO_NAND_FLASH_STATUS_CODEWORD_CNTR_BMSK                                          0xf000
#define HWIO_NAND_FLASH_STATUS_CODEWORD_CNTR_SHFT                                             0xc
#define HWIO_NAND_FLASH_STATUS_FIELD_2KBYTE_DEVICE_BMSK                                     0x800
#define HWIO_NAND_FLASH_STATUS_FIELD_2KBYTE_DEVICE_SHFT                                       0xb
#define HWIO_NAND_FLASH_STATUS_FIELD_2KBYTE_DEVICE_NOT_A_2K_BYTE_PAGE_DEVICE_FVAL             0x0
#define HWIO_NAND_FLASH_STATUS_FIELD_2KBYTE_DEVICE_ENUM_2K_BYTE_PAGE_DEVICE_FVAL              0x1
#define HWIO_NAND_FLASH_STATUS_FIELD_512BYTE_DEVICE_BMSK                                    0x400
#define HWIO_NAND_FLASH_STATUS_FIELD_512BYTE_DEVICE_SHFT                                      0xa
#define HWIO_NAND_FLASH_STATUS_FIELD_512BYTE_DEVICE_NOT_A_512_BYTE_PAGE_DEVICE_FVAL           0x0
#define HWIO_NAND_FLASH_STATUS_FIELD_512BYTE_DEVICE_ENUM_512_BYTE_PAGE_DEVICE_FVAL            0x1
#define HWIO_NAND_FLASH_STATUS_AUTO_DETECT_DONE_BMSK                                        0x200
#define HWIO_NAND_FLASH_STATUS_AUTO_DETECT_DONE_SHFT                                          0x9
#define HWIO_NAND_FLASH_STATUS_MPU_ERROR_BMSK                                               0x100
#define HWIO_NAND_FLASH_STATUS_MPU_ERROR_SHFT                                                 0x8
#define HWIO_NAND_FLASH_STATUS_MPU_ERROR_NO_ERROR_FVAL                                        0x0
#define HWIO_NAND_FLASH_STATUS_MPU_ERROR_MPU_ERROR_FOR_THE_ACCESS_FVAL                        0x1
#define HWIO_NAND_FLASH_STATUS_PROG_ERASE_OP_RESULT_BMSK                                     0x80
#define HWIO_NAND_FLASH_STATUS_PROG_ERASE_OP_RESULT_SHFT                                      0x7
#define HWIO_NAND_FLASH_STATUS_NANDC_TIMEOUT_ERR_BMSK                                        0x40
#define HWIO_NAND_FLASH_STATUS_NANDC_TIMEOUT_ERR_SHFT                                         0x6
#define HWIO_NAND_FLASH_STATUS_NANDC_TIMEOUT_ERR_NO_ERROR_FVAL                                0x0
#define HWIO_NAND_FLASH_STATUS_NANDC_TIMEOUT_ERR_ERROR_FVAL                                   0x1
#define HWIO_NAND_FLASH_STATUS_READY_BSY_N_BMSK                                              0x20
#define HWIO_NAND_FLASH_STATUS_READY_BSY_N_SHFT                                               0x5
#define HWIO_NAND_FLASH_STATUS_READY_BSY_N_EXTERNAL_FLASH_IS_BUSY_FVAL                        0x0
#define HWIO_NAND_FLASH_STATUS_READY_BSY_N_EXTERNAL_FLASH_IS_READY_FVAL                       0x1
#define HWIO_NAND_FLASH_STATUS_OP_ERR_BMSK                                                   0x10
#define HWIO_NAND_FLASH_STATUS_OP_ERR_SHFT                                                    0x4
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_BMSK                                               0xf
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_SHFT                                               0x0
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_IDLE_STATE_FVAL                                    0x0
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_ABORT_TRANSACTION_FVAL                             0x1
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_PAGE_READ_FVAL                                     0x2
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_PAGE_READ_WITH_ECC_FVAL                            0x3
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_PAGE_READ_WITH_ECC_AND_SPARE_DATA_FVAL             0x4
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_SEQUENTIAL_PAGE_READ_FVAL                          0x5
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_PROGRAM_PAGE_FVAL                                  0x6
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_PROGRAM_PAGE_WITH_ECC_FVAL                         0x7
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_RESERVED_PROGRAMMING_FVAL                          0x8
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_PROGRAM_PAGE_WITH_SPARE_FVAL                       0x9
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_BLOCK_ERASE_FVAL                                   0xa
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_FETCH_ID_FVAL                                      0xb
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_CHECK_STATUS_FVAL                                  0xc
#define HWIO_NAND_FLASH_STATUS_OPER_STATUS_RESET_FLASH_DEVICE_FVAL                            0xd

#define HWIO_NANDC_BUFFER_STATUS_ADDR                                             (EBI2ND_REG_BASE      + 0x00000018)
#define HWIO_NANDC_BUFFER_STATUS_PHYS                                             (EBI2ND_REG_BASE_PHYS + 0x00000018)
#define HWIO_NANDC_BUFFER_STATUS_OFFS                                             (EBI2ND_REG_BASE_OFFS + 0x00000018)
#define HWIO_NANDC_BUFFER_STATUS_RMSK                                             0xffff031f
#define HWIO_NANDC_BUFFER_STATUS_IN          \
        in_dword_masked(HWIO_NANDC_BUFFER_STATUS_ADDR, HWIO_NANDC_BUFFER_STATUS_RMSK)
#define HWIO_NANDC_BUFFER_STATUS_INM(m)      \
        in_dword_masked(HWIO_NANDC_BUFFER_STATUS_ADDR, m)
#define HWIO_NANDC_BUFFER_STATUS_OUT(v)      \
        out_dword(HWIO_NANDC_BUFFER_STATUS_ADDR,v)
#define HWIO_NANDC_BUFFER_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NANDC_BUFFER_STATUS_ADDR,m,v,HWIO_NANDC_BUFFER_STATUS_IN)
#define HWIO_NANDC_BUFFER_STATUS_BAD_BLOCK_STATUS_BMSK                                 0xffff0000
#define HWIO_NANDC_BUFFER_STATUS_BAD_BLOCK_STATUS_SHFT                                       0x10
#define HWIO_NANDC_BUFFER_STATUS_XFR_STEP2_REG_UPDATE_DONE_BMSK                             0x200
#define HWIO_NANDC_BUFFER_STATUS_XFR_STEP2_REG_UPDATE_DONE_SHFT                               0x9
#define HWIO_NANDC_BUFFER_STATUS_UNCORRECTABLE_BMSK                                         0x100
#define HWIO_NANDC_BUFFER_STATUS_UNCORRECTABLE_SHFT                                           0x8
#define HWIO_NANDC_BUFFER_STATUS_NUM_ERRORS_BMSK                                             0x1f
#define HWIO_NANDC_BUFFER_STATUS_NUM_ERRORS_SHFT                                              0x0

#define HWIO_NAND_DEV0_CFG0_ADDR                                                  (EBI2ND_REG_BASE      + 0x00000020)
#define HWIO_NAND_DEV0_CFG0_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x00000020)
#define HWIO_NAND_DEV0_CFG0_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x00000020)
#define HWIO_NAND_DEV0_CFG0_RMSK                                                  0xffffffff
#define HWIO_NAND_DEV0_CFG0_IN          \
        in_dword_masked(HWIO_NAND_DEV0_CFG0_ADDR, HWIO_NAND_DEV0_CFG0_RMSK)
#define HWIO_NAND_DEV0_CFG0_INM(m)      \
        in_dword_masked(HWIO_NAND_DEV0_CFG0_ADDR, m)
#define HWIO_NAND_DEV0_CFG0_OUT(v)      \
        out_dword(HWIO_NAND_DEV0_CFG0_ADDR,v)
#define HWIO_NAND_DEV0_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_DEV0_CFG0_ADDR,m,v,HWIO_NAND_DEV0_CFG0_IN)
#define HWIO_NAND_DEV0_CFG0_SET_RD_MODE_AFTER_STATUS_BMSK                              0x80000000
#define HWIO_NAND_DEV0_CFG0_SET_RD_MODE_AFTER_STATUS_SHFT                                    0x1f
#define HWIO_NAND_DEV0_CFG0_STATUS_BFR_READ_BMSK                                       0x40000000
#define HWIO_NAND_DEV0_CFG0_STATUS_BFR_READ_SHFT                                             0x1e
#define HWIO_NAND_DEV0_CFG0_NUM_ADDR_CYCLES_BMSK                                       0x38000000
#define HWIO_NAND_DEV0_CFG0_NUM_ADDR_CYCLES_SHFT                                             0x1b
#define HWIO_NAND_DEV0_CFG0_NUM_ADDR_CYCLES_NO_ADDRESS_CYCLES_FVAL                            0x0
#define HWIO_NAND_DEV0_CFG0_SPARE_SIZE_BYTES_BMSK                                       0x7800000
#define HWIO_NAND_DEV0_CFG0_SPARE_SIZE_BYTES_SHFT                                            0x17
#define HWIO_NAND_DEV0_CFG0_ECC_PARITY_SIZE_BYTES_BMSK                                   0x780000
#define HWIO_NAND_DEV0_CFG0_ECC_PARITY_SIZE_BYTES_SHFT                                       0x13
#define HWIO_NAND_DEV0_CFG0_UD_SIZE_BYTES_BMSK                                            0x7fe00
#define HWIO_NAND_DEV0_CFG0_UD_SIZE_BYTES_SHFT                                                0x9
#define HWIO_NAND_DEV0_CFG0_CW_PER_PAGE_BMSK                                                0x1c0
#define HWIO_NAND_DEV0_CFG0_CW_PER_PAGE_SHFT                                                  0x6
#define HWIO_NAND_DEV0_CFG0_CW_PER_PAGE_ENUM_1_CODEWORD_PER_PAGE_FVAL                         0x0
#define HWIO_NAND_DEV0_CFG0_CW_PER_PAGE_ENUM_2_CODEWORDS_PER_PAGE_FVAL                        0x1
#define HWIO_NAND_DEV0_CFG0_CW_PER_PAGE_ENUM_3_CODEWORDS_PER_PAGE_FVAL                        0x2
#define HWIO_NAND_DEV0_CFG0_CW_PER_PAGE_ENUM_4_CODEWORDS_PER_PAGE_FVAL                        0x3
#define HWIO_NAND_DEV0_CFG0_CW_PER_PAGE_ENUM_5_CODEWORDS_PER_PAGE_FVAL                        0x4
#define HWIO_NAND_DEV0_CFG0_CW_PER_PAGE_ENUM_6_CODEWORDS_PER_PAGE_FVAL                        0x5
#define HWIO_NAND_DEV0_CFG0_CW_PER_PAGE_ENUM_7_CODEWORDS_PER_PAGE_FVAL                        0x6
#define HWIO_NAND_DEV0_CFG0_CW_PER_PAGE_ENUM_8_CODEWORDS_PER_PAGE_FVAL                        0x7
#define HWIO_NAND_DEV0_CFG0_MSB_CW_PER_PAGE_BMSK                                             0x20
#define HWIO_NAND_DEV0_CFG0_MSB_CW_PER_PAGE_SHFT                                              0x5
#define HWIO_NAND_DEV0_CFG0_DISABLE_STATUS_AFTER_WRITE_BMSK                                  0x10
#define HWIO_NAND_DEV0_CFG0_DISABLE_STATUS_AFTER_WRITE_SHFT                                   0x4
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_BMSK                                    0xf
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_SHFT                                    0x0
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_16_MS_FVAL                         0x0
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_32_MS_FVAL                         0x1
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_64_MS_FVAL                         0x2
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_128_MS_FVAL                        0x3
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_256_MS_FVAL                        0x4
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_512_MS_FVAL                        0x5
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_1_SEC_FVAL                         0x6
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_2_SEC_FVAL                         0x7
#define HWIO_NAND_DEV0_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_1_MS_FVAL                          0x8

#define HWIO_NAND_DEV1_CFG0_ADDR                                                  (EBI2ND_REG_BASE      + 0x00000030)
#define HWIO_NAND_DEV1_CFG0_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x00000030)
#define HWIO_NAND_DEV1_CFG0_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x00000030)
#define HWIO_NAND_DEV1_CFG0_RMSK                                                  0xffffffff
#define HWIO_NAND_DEV1_CFG0_IN          \
        in_dword_masked(HWIO_NAND_DEV1_CFG0_ADDR, HWIO_NAND_DEV1_CFG0_RMSK)
#define HWIO_NAND_DEV1_CFG0_INM(m)      \
        in_dword_masked(HWIO_NAND_DEV1_CFG0_ADDR, m)
#define HWIO_NAND_DEV1_CFG0_OUT(v)      \
        out_dword(HWIO_NAND_DEV1_CFG0_ADDR,v)
#define HWIO_NAND_DEV1_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_DEV1_CFG0_ADDR,m,v,HWIO_NAND_DEV1_CFG0_IN)
#define HWIO_NAND_DEV1_CFG0_SET_RD_MODE_AFTER_STATUS_BMSK                              0x80000000
#define HWIO_NAND_DEV1_CFG0_SET_RD_MODE_AFTER_STATUS_SHFT                                    0x1f
#define HWIO_NAND_DEV1_CFG0_STATUS_BFR_READ_BMSK                                       0x40000000
#define HWIO_NAND_DEV1_CFG0_STATUS_BFR_READ_SHFT                                             0x1e
#define HWIO_NAND_DEV1_CFG0_NUM_ADDR_CYCLES_BMSK                                       0x38000000
#define HWIO_NAND_DEV1_CFG0_NUM_ADDR_CYCLES_SHFT                                             0x1b
#define HWIO_NAND_DEV1_CFG0_NUM_ADDR_CYCLES_NO_ADDRESS_CYCLES_FVAL                            0x0
#define HWIO_NAND_DEV1_CFG0_SPARE_SIZE_BYTES_BMSK                                       0x7800000
#define HWIO_NAND_DEV1_CFG0_SPARE_SIZE_BYTES_SHFT                                            0x17
#define HWIO_NAND_DEV1_CFG0_ECC_PARITY_SIZE_BYTES_BMSK                                   0x780000
#define HWIO_NAND_DEV1_CFG0_ECC_PARITY_SIZE_BYTES_SHFT                                       0x13
#define HWIO_NAND_DEV1_CFG0_UD_SIZE_BYTES_BMSK                                            0x7fe00
#define HWIO_NAND_DEV1_CFG0_UD_SIZE_BYTES_SHFT                                                0x9
#define HWIO_NAND_DEV1_CFG0_CW_PER_PAGE_BMSK                                                0x1c0
#define HWIO_NAND_DEV1_CFG0_CW_PER_PAGE_SHFT                                                  0x6
#define HWIO_NAND_DEV1_CFG0_CW_PER_PAGE_ENUM_1_CODEWORD_PER_PAGE_FVAL                         0x0
#define HWIO_NAND_DEV1_CFG0_CW_PER_PAGE_ENUM_2_CODEWORDS_PER_PAGE_FVAL                        0x1
#define HWIO_NAND_DEV1_CFG0_CW_PER_PAGE_ENUM_3_CODEWORDS_PER_PAGE_FVAL                        0x2
#define HWIO_NAND_DEV1_CFG0_CW_PER_PAGE_ENUM_4_CODEWORDS_PER_PAGE_FVAL                        0x3
#define HWIO_NAND_DEV1_CFG0_CW_PER_PAGE_ENUM_5_CODEWORDS_PER_PAGE_FVAL                        0x4
#define HWIO_NAND_DEV1_CFG0_CW_PER_PAGE_ENUM_6_CODEWORDS_PER_PAGE_FVAL                        0x5
#define HWIO_NAND_DEV1_CFG0_CW_PER_PAGE_ENUM_7_CODEWORDS_PER_PAGE_FVAL                        0x6
#define HWIO_NAND_DEV1_CFG0_CW_PER_PAGE_ENUM_8_CODEWORDS_PER_PAGE_FVAL                        0x7
#define HWIO_NAND_DEV1_CFG0_MSB_CW_PER_PAGE_BMSK                                             0x20
#define HWIO_NAND_DEV1_CFG0_MSB_CW_PER_PAGE_SHFT                                              0x5
#define HWIO_NAND_DEV1_CFG0_DISABLE_STATUS_AFTER_WRITE_BMSK                                  0x10
#define HWIO_NAND_DEV1_CFG0_DISABLE_STATUS_AFTER_WRITE_SHFT                                   0x4
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_BMSK                                    0xf
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_SHFT                                    0x0
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_16_MS_FVAL                         0x0
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_32_MS_FVAL                         0x1
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_64_MS_FVAL                         0x2
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_128_MS_FVAL                        0x3
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_256_MS_FVAL                        0x4
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_512_MS_FVAL                        0x5
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_1_SEC_FVAL                         0x6
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_2_SEC_FVAL                         0x7
#define HWIO_NAND_DEV1_CFG0_BUSY_TIMEOUT_ERROR_SELECT_ENUM_1_MS_FVAL                          0x8

#define HWIO_NAND_DEVn_CFG1_ADDR(n)                                               (EBI2ND_REG_BASE      + 0x00000024 + 0x10 * (n))
#define HWIO_NAND_DEVn_CFG1_PHYS(n)                                               (EBI2ND_REG_BASE_PHYS + 0x00000024 + 0x10 * (n))
#define HWIO_NAND_DEVn_CFG1_OFFS(n)                                               (EBI2ND_REG_BASE_OFFS + 0x00000024 + 0x10 * (n))
#define HWIO_NAND_DEVn_CFG1_RMSK                                                  0x3bffffff
#define HWIO_NAND_DEVn_CFG1_MAXn                                                           1
#define HWIO_NAND_DEVn_CFG1_INI(n)        \
        in_dword_masked(HWIO_NAND_DEVn_CFG1_ADDR(n), HWIO_NAND_DEVn_CFG1_RMSK)
#define HWIO_NAND_DEVn_CFG1_INMI(n,mask)    \
        in_dword_masked(HWIO_NAND_DEVn_CFG1_ADDR(n), mask)
#define HWIO_NAND_DEVn_CFG1_OUTI(n,val)    \
        out_dword(HWIO_NAND_DEVn_CFG1_ADDR(n),val)
#define HWIO_NAND_DEVn_CFG1_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_NAND_DEVn_CFG1_ADDR(n),mask,val,HWIO_NAND_DEVn_CFG1_INI(n))
#define HWIO_NAND_DEVn_CFG1_ECC_MODE_BMSK                                              0x30000000
#define HWIO_NAND_DEVn_CFG1_ECC_MODE_SHFT                                                    0x1c
#define HWIO_NAND_DEVn_CFG1_ENABLE_BCH_ECC_BMSK                                         0x8000000
#define HWIO_NAND_DEVn_CFG1_ENABLE_BCH_ECC_SHFT                                              0x1b
#define HWIO_NAND_DEVn_CFG1_DISABLE_ECC_RESET_AFTER_OPDONE_BMSK                         0x2000000
#define HWIO_NAND_DEVn_CFG1_DISABLE_ECC_RESET_AFTER_OPDONE_SHFT                              0x19
#define HWIO_NAND_DEVn_CFG1_ECC_DECODER_CGC_EN_BMSK                                     0x1000000
#define HWIO_NAND_DEVn_CFG1_ECC_DECODER_CGC_EN_SHFT                                          0x18
#define HWIO_NAND_DEVn_CFG1_ECC_ENCODER_CGC_EN_BMSK                                      0x800000
#define HWIO_NAND_DEVn_CFG1_ECC_ENCODER_CGC_EN_SHFT                                          0x17
#define HWIO_NAND_DEVn_CFG1_WR_RD_BSY_GAP_BMSK                                           0x7e0000
#define HWIO_NAND_DEVn_CFG1_WR_RD_BSY_GAP_SHFT                                               0x11
#define HWIO_NAND_DEVn_CFG1_WR_RD_BSY_GAP_ENUM_2_CLOCK_CYCLE_GAP_FVAL                         0x0
#define HWIO_NAND_DEVn_CFG1_WR_RD_BSY_GAP_ENUM_4_CLOCK_CYCLES_GAP_FVAL                        0x1
#define HWIO_NAND_DEVn_CFG1_WR_RD_BSY_GAP_ENUM_6_CLOCK_CYCLES_GAP_FVAL                        0x2
#define HWIO_NAND_DEVn_CFG1_WR_RD_BSY_GAP_ENUM_8_CLOCK_CYCLES_GAP_FVAL                        0x3
#define HWIO_NAND_DEVn_CFG1_WR_RD_BSY_GAP_ENUM_10_CLOCK_CYCLES_GAP_FVAL                       0x4
#define HWIO_NAND_DEVn_CFG1_WR_RD_BSY_GAP_ENUM_128_CLOCK_CYCLES_GAP_FVAL                     0x3f
#define HWIO_NAND_DEVn_CFG1_BAD_BLOCK_IN_SPARE_AREA_BMSK                                  0x10000
#define HWIO_NAND_DEVn_CFG1_BAD_BLOCK_IN_SPARE_AREA_SHFT                                     0x10
#define HWIO_NAND_DEVn_CFG1_BAD_BLOCK_IN_SPARE_AREA_IN_USER_DATA_AREA_FVAL                    0x0
#define HWIO_NAND_DEVn_CFG1_BAD_BLOCK_IN_SPARE_AREA_IN_SPARE_AREA_FVAL                        0x1
#define HWIO_NAND_DEVn_CFG1_BAD_BLOCK_BYTE_NUM_BMSK                                        0xffc0
#define HWIO_NAND_DEVn_CFG1_BAD_BLOCK_BYTE_NUM_SHFT                                           0x6
#define HWIO_NAND_DEVn_CFG1_CS_ACTIVE_BSY_BMSK                                               0x20
#define HWIO_NAND_DEVn_CFG1_CS_ACTIVE_BSY_SHFT                                                0x5
#define HWIO_NAND_DEVn_CFG1_CS_ACTIVE_BSY_ALLOW_CS_DE_ASSERTION_FVAL                          0x0
#define HWIO_NAND_DEVn_CFG1_CS_ACTIVE_BSY_ASSERT_CS_DURING_BUSY_FVAL                          0x1
#define HWIO_NAND_DEVn_CFG1_NAND_RECOVERY_CYCLES_BMSK                                        0x1c
#define HWIO_NAND_DEVn_CFG1_NAND_RECOVERY_CYCLES_SHFT                                         0x2
#define HWIO_NAND_DEVn_CFG1_NAND_RECOVERY_CYCLES_ENUM_1_RECOVERY_CYCLE_FVAL                   0x0
#define HWIO_NAND_DEVn_CFG1_NAND_RECOVERY_CYCLES_ENUM_2_RECOVERY_CYCLES_FVAL                  0x1
#define HWIO_NAND_DEVn_CFG1_NAND_RECOVERY_CYCLES_ENUM_3_RECOVERY_CYCLES_FVAL                  0x2
#define HWIO_NAND_DEVn_CFG1_NAND_RECOVERY_CYCLES_ENUM_8_RECOVERY_CYCLES_FVAL                  0x7
#define HWIO_NAND_DEVn_CFG1_WIDE_FLASH_BMSK                                                   0x2
#define HWIO_NAND_DEVn_CFG1_WIDE_FLASH_SHFT                                                   0x1
#define HWIO_NAND_DEVn_CFG1_WIDE_FLASH_ENUM_8_BIT_DATA_BUS_FVAL                               0x0
#define HWIO_NAND_DEVn_CFG1_WIDE_FLASH_ENUM_16_BIT_DATA_BUS_FVAL                              0x1
#define HWIO_NAND_DEVn_CFG1_ECC_DISABLE_BMSK                                                  0x1
#define HWIO_NAND_DEVn_CFG1_ECC_DISABLE_SHFT                                                  0x0

#define HWIO_NAND_DEV0_ECC_CFG_ADDR                                               (EBI2ND_REG_BASE      + 0x00000028)
#define HWIO_NAND_DEV0_ECC_CFG_PHYS                                               (EBI2ND_REG_BASE_PHYS + 0x00000028)
#define HWIO_NAND_DEV0_ECC_CFG_OFFS                                               (EBI2ND_REG_BASE_OFFS + 0x00000028)
#define HWIO_NAND_DEV0_ECC_CFG_RMSK                                               0x73ff1f33
#define HWIO_NAND_DEV0_ECC_CFG_IN          \
        in_dword_masked(HWIO_NAND_DEV0_ECC_CFG_ADDR, HWIO_NAND_DEV0_ECC_CFG_RMSK)
#define HWIO_NAND_DEV0_ECC_CFG_INM(m)      \
        in_dword_masked(HWIO_NAND_DEV0_ECC_CFG_ADDR, m)
#define HWIO_NAND_DEV0_ECC_CFG_OUT(v)      \
        out_dword(HWIO_NAND_DEV0_ECC_CFG_ADDR,v)
#define HWIO_NAND_DEV0_ECC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_DEV0_ECC_CFG_ADDR,m,v,HWIO_NAND_DEV0_ECC_CFG_IN)
#define HWIO_NAND_DEV0_ECC_CFG_ECC_FORCE_CLK_OPEN_BMSK                                 0x40000000
#define HWIO_NAND_DEV0_ECC_CFG_ECC_FORCE_CLK_OPEN_SHFT                                       0x1e
#define HWIO_NAND_DEV0_ECC_CFG_ECC_FORCE_CLK_OPEN_DISABLE_FVAL                                0x0
#define HWIO_NAND_DEV0_ECC_CFG_ECC_FORCE_CLK_OPEN_ENABLE_FVAL                                 0x1
#define HWIO_NAND_DEV0_ECC_CFG_ECC_DEC_CLK_SHUTDOWN_BMSK                               0x20000000
#define HWIO_NAND_DEV0_ECC_CFG_ECC_DEC_CLK_SHUTDOWN_SHFT                                     0x1d
#define HWIO_NAND_DEV0_ECC_CFG_ECC_DEC_CLK_SHUTDOWN_DISABLE_FVAL                              0x0
#define HWIO_NAND_DEV0_ECC_CFG_ECC_DEC_CLK_SHUTDOWN_ENABLE_FVAL                               0x1
#define HWIO_NAND_DEV0_ECC_CFG_ECC_ENC_CLK_SHUTDOWN_BMSK                               0x10000000
#define HWIO_NAND_DEV0_ECC_CFG_ECC_ENC_CLK_SHUTDOWN_SHFT                                     0x1c
#define HWIO_NAND_DEV0_ECC_CFG_ECC_ENC_CLK_SHUTDOWN_DISABLE_FVAL                              0x0
#define HWIO_NAND_DEV0_ECC_CFG_ECC_ENC_CLK_SHUTDOWN_ENABLE_FVAL                               0x1
#define HWIO_NAND_DEV0_ECC_CFG_ECC_NUM_DATA_BYTES_BMSK                                  0x3ff0000
#define HWIO_NAND_DEV0_ECC_CFG_ECC_NUM_DATA_BYTES_SHFT                                       0x10
#define HWIO_NAND_DEV0_ECC_CFG_ECC_NUM_DATA_BYTES_ENUM_512_BYTES_FVAL                       0x200
#define HWIO_NAND_DEV0_ECC_CFG_ECC_NUM_DATA_BYTES_ENUM_516_BYTES_FVAL                       0x204
#define HWIO_NAND_DEV0_ECC_CFG_ECC_NUM_DATA_BYTES_ENUM_517_BYTES_FVAL                       0x205
#define HWIO_NAND_DEV0_ECC_CFG_ECC_PARITY_SIZE_BYTES_BMSK                                  0x1f00
#define HWIO_NAND_DEV0_ECC_CFG_ECC_PARITY_SIZE_BYTES_SHFT                                     0x8
#define HWIO_NAND_DEV0_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_7_BYTES_FVAL                        0x7
#define HWIO_NAND_DEV0_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_8_BYTES_FVAL                        0x8
#define HWIO_NAND_DEV0_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_13_BYTES_FVAL                       0xd
#define HWIO_NAND_DEV0_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_14_BYTES_FVAL                       0xe
#define HWIO_NAND_DEV0_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_20_BYTES_FVAL                      0x14
#define HWIO_NAND_DEV0_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_26_BYTES_FVAL                      0x1a
#define HWIO_NAND_DEV0_ECC_CFG_ECC_MODE_BMSK                                                 0x30
#define HWIO_NAND_DEV0_ECC_CFG_ECC_MODE_SHFT                                                  0x4
#define HWIO_NAND_DEV0_ECC_CFG_ECC_MODE_ENUM_7_BIT_ECC_FVAL                                   0x0
#define HWIO_NAND_DEV0_ECC_CFG_ECC_MODE_ENUM_8_BIT_ECC_FVAL                                   0x1
#define HWIO_NAND_DEV0_ECC_CFG_ECC_MODE_ENUM_12_BIT_ECC_FVAL                                  0x2
#define HWIO_NAND_DEV0_ECC_CFG_ECC_MODE_ENUM_16_BIT_ECC_FVAL                                  0x3
#define HWIO_NAND_DEV0_ECC_CFG_ECC_SW_RESET_BMSK                                              0x2
#define HWIO_NAND_DEV0_ECC_CFG_ECC_SW_RESET_SHFT                                              0x1
#define HWIO_NAND_DEV0_ECC_CFG_ECC_SW_RESET_OP_MODE_FVAL                                      0x0
#define HWIO_NAND_DEV0_ECC_CFG_ECC_SW_RESET_RESET_FVAL                                        0x1
#define HWIO_NAND_DEV0_ECC_CFG_ECC_DISABLE_BMSK                                               0x1
#define HWIO_NAND_DEV0_ECC_CFG_ECC_DISABLE_SHFT                                               0x0
#define HWIO_NAND_DEV0_ECC_CFG_ECC_DISABLE_ECC_ENABLED_FVAL                                   0x0
#define HWIO_NAND_DEV0_ECC_CFG_ECC_DISABLE_ECC_DISABLED_FVAL                                  0x1

#define HWIO_NAND_DEV1_ECC_CFG_ADDR                                               (EBI2ND_REG_BASE      + 0x0000002c)
#define HWIO_NAND_DEV1_ECC_CFG_PHYS                                               (EBI2ND_REG_BASE_PHYS + 0x0000002c)
#define HWIO_NAND_DEV1_ECC_CFG_OFFS                                               (EBI2ND_REG_BASE_OFFS + 0x0000002c)
#define HWIO_NAND_DEV1_ECC_CFG_RMSK                                               0x73ff1f33
#define HWIO_NAND_DEV1_ECC_CFG_IN          \
        in_dword_masked(HWIO_NAND_DEV1_ECC_CFG_ADDR, HWIO_NAND_DEV1_ECC_CFG_RMSK)
#define HWIO_NAND_DEV1_ECC_CFG_INM(m)      \
        in_dword_masked(HWIO_NAND_DEV1_ECC_CFG_ADDR, m)
#define HWIO_NAND_DEV1_ECC_CFG_OUT(v)      \
        out_dword(HWIO_NAND_DEV1_ECC_CFG_ADDR,v)
#define HWIO_NAND_DEV1_ECC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_DEV1_ECC_CFG_ADDR,m,v,HWIO_NAND_DEV1_ECC_CFG_IN)
#define HWIO_NAND_DEV1_ECC_CFG_ECC_FORCE_CLK_OPEN_BMSK                                 0x40000000
#define HWIO_NAND_DEV1_ECC_CFG_ECC_FORCE_CLK_OPEN_SHFT                                       0x1e
#define HWIO_NAND_DEV1_ECC_CFG_ECC_FORCE_CLK_OPEN_DISABLE_FVAL                                0x0
#define HWIO_NAND_DEV1_ECC_CFG_ECC_FORCE_CLK_OPEN_ENABLE_FVAL                                 0x1
#define HWIO_NAND_DEV1_ECC_CFG_ECC_DEC_CLK_SHUTDOWN_BMSK                               0x20000000
#define HWIO_NAND_DEV1_ECC_CFG_ECC_DEC_CLK_SHUTDOWN_SHFT                                     0x1d
#define HWIO_NAND_DEV1_ECC_CFG_ECC_DEC_CLK_SHUTDOWN_DISABLE_FVAL                              0x0
#define HWIO_NAND_DEV1_ECC_CFG_ECC_DEC_CLK_SHUTDOWN_ENABLE_FVAL                               0x1
#define HWIO_NAND_DEV1_ECC_CFG_ECC_ENC_CLK_SHUTDOWN_BMSK                               0x10000000
#define HWIO_NAND_DEV1_ECC_CFG_ECC_ENC_CLK_SHUTDOWN_SHFT                                     0x1c
#define HWIO_NAND_DEV1_ECC_CFG_ECC_ENC_CLK_SHUTDOWN_DISABLE_FVAL                              0x0
#define HWIO_NAND_DEV1_ECC_CFG_ECC_ENC_CLK_SHUTDOWN_ENABLE_FVAL                               0x1
#define HWIO_NAND_DEV1_ECC_CFG_ECC_NUM_DATA_BYTES_BMSK                                  0x3ff0000
#define HWIO_NAND_DEV1_ECC_CFG_ECC_NUM_DATA_BYTES_SHFT                                       0x10
#define HWIO_NAND_DEV1_ECC_CFG_ECC_NUM_DATA_BYTES_ENUM_512_BYTES_FVAL                       0x200
#define HWIO_NAND_DEV1_ECC_CFG_ECC_NUM_DATA_BYTES_ENUM_516_BYTES_FVAL                       0x204
#define HWIO_NAND_DEV1_ECC_CFG_ECC_NUM_DATA_BYTES_ENUM_517_BYTES_FVAL                       0x205
#define HWIO_NAND_DEV1_ECC_CFG_ECC_PARITY_SIZE_BYTES_BMSK                                  0x1f00
#define HWIO_NAND_DEV1_ECC_CFG_ECC_PARITY_SIZE_BYTES_SHFT                                     0x8
#define HWIO_NAND_DEV1_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_7_BYTES_FVAL                        0x7
#define HWIO_NAND_DEV1_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_8_BYTES_FVAL                        0x8
#define HWIO_NAND_DEV1_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_13_BYTES_FVAL                       0xd
#define HWIO_NAND_DEV1_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_14_BYTES_FVAL                       0xe
#define HWIO_NAND_DEV1_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_20_BYTES_FVAL                      0x14
#define HWIO_NAND_DEV1_ECC_CFG_ECC_PARITY_SIZE_BYTES_ENUM_26_BYTES_FVAL                      0x1a
#define HWIO_NAND_DEV1_ECC_CFG_ECC_MODE_BMSK                                                 0x30
#define HWIO_NAND_DEV1_ECC_CFG_ECC_MODE_SHFT                                                  0x4
#define HWIO_NAND_DEV1_ECC_CFG_ECC_MODE_ENUM_7_BIT_ECC_FVAL                                   0x0
#define HWIO_NAND_DEV1_ECC_CFG_ECC_MODE_ENUM_8_BIT_ECC_FVAL                                   0x1
#define HWIO_NAND_DEV1_ECC_CFG_ECC_MODE_ENUM_12_BIT_ECC_FVAL                                  0x2
#define HWIO_NAND_DEV1_ECC_CFG_ECC_MODE_ENUM_16_BIT_ECC_FVAL                                  0x3
#define HWIO_NAND_DEV1_ECC_CFG_ECC_SW_RESET_BMSK                                              0x2
#define HWIO_NAND_DEV1_ECC_CFG_ECC_SW_RESET_SHFT                                              0x1
#define HWIO_NAND_DEV1_ECC_CFG_ECC_SW_RESET_OP_MODE_FVAL                                      0x0
#define HWIO_NAND_DEV1_ECC_CFG_ECC_SW_RESET_RESET_FVAL                                        0x1
#define HWIO_NAND_DEV1_ECC_CFG_ECC_DISABLE_BMSK                                               0x1
#define HWIO_NAND_DEV1_ECC_CFG_ECC_DISABLE_SHFT                                               0x0
#define HWIO_NAND_DEV1_ECC_CFG_ECC_DISABLE_ECC_ENABLED_FVAL                                   0x0
#define HWIO_NAND_DEV1_ECC_CFG_ECC_DISABLE_ECC_DISABLED_FVAL                                  0x1

#define HWIO_NAND_FLASH_READ_ID_ADDR                                              (EBI2ND_REG_BASE      + 0x00000040)
#define HWIO_NAND_FLASH_READ_ID_PHYS                                              (EBI2ND_REG_BASE_PHYS + 0x00000040)
#define HWIO_NAND_FLASH_READ_ID_OFFS                                              (EBI2ND_REG_BASE_OFFS + 0x00000040)
#define HWIO_NAND_FLASH_READ_ID_RMSK                                              0xffffffff
#define HWIO_NAND_FLASH_READ_ID_IN          \
        in_dword_masked(HWIO_NAND_FLASH_READ_ID_ADDR, HWIO_NAND_FLASH_READ_ID_RMSK)
#define HWIO_NAND_FLASH_READ_ID_INM(m)      \
        in_dword_masked(HWIO_NAND_FLASH_READ_ID_ADDR, m)
#define HWIO_NAND_FLASH_READ_ID_OUT(v)      \
        out_dword(HWIO_NAND_FLASH_READ_ID_ADDR,v)
#define HWIO_NAND_FLASH_READ_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_FLASH_READ_ID_ADDR,m,v,HWIO_NAND_FLASH_READ_ID_IN)
#define HWIO_NAND_FLASH_READ_ID_READ_ID_BMSK                                           0xffffffff
#define HWIO_NAND_FLASH_READ_ID_READ_ID_SHFT                                                  0x0

#define HWIO_NAND_FLASH_READ_ID2_ADDR                                             (EBI2ND_REG_BASE      + 0x00000048)
#define HWIO_NAND_FLASH_READ_ID2_PHYS                                             (EBI2ND_REG_BASE_PHYS + 0x00000048)
#define HWIO_NAND_FLASH_READ_ID2_OFFS                                             (EBI2ND_REG_BASE_OFFS + 0x00000048)
#define HWIO_NAND_FLASH_READ_ID2_RMSK                                             0xffffffff
#define HWIO_NAND_FLASH_READ_ID2_IN          \
        in_dword_masked(HWIO_NAND_FLASH_READ_ID2_ADDR, HWIO_NAND_FLASH_READ_ID2_RMSK)
#define HWIO_NAND_FLASH_READ_ID2_INM(m)      \
        in_dword_masked(HWIO_NAND_FLASH_READ_ID2_ADDR, m)
#define HWIO_NAND_FLASH_READ_ID2_OUT(v)      \
        out_dword(HWIO_NAND_FLASH_READ_ID2_ADDR,v)
#define HWIO_NAND_FLASH_READ_ID2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_FLASH_READ_ID2_ADDR,m,v,HWIO_NAND_FLASH_READ_ID2_IN)
#define HWIO_NAND_FLASH_READ_ID2_READ_ID_BMSK                                          0xffffffff
#define HWIO_NAND_FLASH_READ_ID2_READ_ID_SHFT                                                 0x0

#define HWIO_NAND_FLASH_READ_STATUS_ADDR                                          (EBI2ND_REG_BASE      + 0x00000044)
#define HWIO_NAND_FLASH_READ_STATUS_PHYS                                          (EBI2ND_REG_BASE_PHYS + 0x00000044)
#define HWIO_NAND_FLASH_READ_STATUS_OFFS                                          (EBI2ND_REG_BASE_OFFS + 0x00000044)
#define HWIO_NAND_FLASH_READ_STATUS_RMSK                                          0xffffffff
#define HWIO_NAND_FLASH_READ_STATUS_IN          \
        in_dword_masked(HWIO_NAND_FLASH_READ_STATUS_ADDR, HWIO_NAND_FLASH_READ_STATUS_RMSK)
#define HWIO_NAND_FLASH_READ_STATUS_INM(m)      \
        in_dword_masked(HWIO_NAND_FLASH_READ_STATUS_ADDR, m)
#define HWIO_NAND_FLASH_READ_STATUS_OUT(v)      \
        out_dword(HWIO_NAND_FLASH_READ_STATUS_ADDR,v)
#define HWIO_NAND_FLASH_READ_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_FLASH_READ_STATUS_ADDR,m,v,HWIO_NAND_FLASH_READ_STATUS_IN)
#define HWIO_NAND_FLASH_READ_STATUS_ECC_STATUS_BMSK                                    0xffff0000
#define HWIO_NAND_FLASH_READ_STATUS_ECC_STATUS_SHFT                                          0x10
#define HWIO_NAND_FLASH_READ_STATUS_DEV_STATUS_BMSK                                        0xffff
#define HWIO_NAND_FLASH_READ_STATUS_DEV_STATUS_SHFT                                           0x0

#define HWIO_NAND_FLASH_CONFIG_DATA_ADDR                                          (EBI2ND_REG_BASE      + 0x00000050)
#define HWIO_NAND_FLASH_CONFIG_DATA_PHYS                                          (EBI2ND_REG_BASE_PHYS + 0x00000050)
#define HWIO_NAND_FLASH_CONFIG_DATA_OFFS                                          (EBI2ND_REG_BASE_OFFS + 0x00000050)
#define HWIO_NAND_FLASH_CONFIG_DATA_RMSK                                          0xffffffff
#define HWIO_NAND_FLASH_CONFIG_DATA_IN          \
        in_dword_masked(HWIO_NAND_FLASH_CONFIG_DATA_ADDR, HWIO_NAND_FLASH_CONFIG_DATA_RMSK)
#define HWIO_NAND_FLASH_CONFIG_DATA_INM(m)      \
        in_dword_masked(HWIO_NAND_FLASH_CONFIG_DATA_ADDR, m)
#define HWIO_NAND_FLASH_CONFIG_DATA_OUT(v)      \
        out_dword(HWIO_NAND_FLASH_CONFIG_DATA_ADDR,v)
#define HWIO_NAND_FLASH_CONFIG_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_FLASH_CONFIG_DATA_ADDR,m,v,HWIO_NAND_FLASH_CONFIG_DATA_IN)
#define HWIO_NAND_FLASH_CONFIG_DATA_DATA_IN_BMSK                                       0xffffffff
#define HWIO_NAND_FLASH_CONFIG_DATA_DATA_IN_SHFT                                              0x0

#define HWIO_NAND_FLASH_CONFIG_ADDR                                               (EBI2ND_REG_BASE      + 0x00000054)
#define HWIO_NAND_FLASH_CONFIG_PHYS                                               (EBI2ND_REG_BASE_PHYS + 0x00000054)
#define HWIO_NAND_FLASH_CONFIG_OFFS                                               (EBI2ND_REG_BASE_OFFS + 0x00000054)
#define HWIO_NAND_FLASH_CONFIG_RMSK                                               0xffff3fff
#define HWIO_NAND_FLASH_CONFIG_IN          \
        in_dword_masked(HWIO_NAND_FLASH_CONFIG_ADDR, HWIO_NAND_FLASH_CONFIG_RMSK)
#define HWIO_NAND_FLASH_CONFIG_INM(m)      \
        in_dword_masked(HWIO_NAND_FLASH_CONFIG_ADDR, m)
#define HWIO_NAND_FLASH_CONFIG_OUT(v)      \
        out_dword(HWIO_NAND_FLASH_CONFIG_ADDR,v)
#define HWIO_NAND_FLASH_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_FLASH_CONFIG_ADDR,m,v,HWIO_NAND_FLASH_CONFIG_IN)
#define HWIO_NAND_FLASH_CONFIG_DATA_OUT_BMSK                                           0xffff0000
#define HWIO_NAND_FLASH_CONFIG_DATA_OUT_SHFT                                                 0x10
#define HWIO_NAND_FLASH_CONFIG_DATA_OUT_EN_BMSK                                            0x2000
#define HWIO_NAND_FLASH_CONFIG_DATA_OUT_EN_SHFT                                               0xd
#define HWIO_NAND_FLASH_CONFIG_DATA_IN_EN_BMSK                                             0x1c00
#define HWIO_NAND_FLASH_CONFIG_DATA_IN_EN_SHFT                                                0xa
#define HWIO_NAND_FLASH_CONFIG_DATA_IN_EN_DONT_READ_FVAL                                      0x0
#define HWIO_NAND_FLASH_CONFIG_DATA_IN_EN_WRITE_NAND_FLASH_CONFIG_DATA_15_0_FVAL              0x1
#define HWIO_NAND_FLASH_CONFIG_DATA_IN_EN_WRITE_NAND_FLASH_CONFIG_DATA_31_16_FVAL             0x2
#define HWIO_NAND_FLASH_CONFIG_DATA_IN_EN_WRITE_NAND_FLASH_READ_STATUS_15_0_FVAL              0x3
#define HWIO_NAND_FLASH_CONFIG_CS5_N_BMSK                                                   0x200
#define HWIO_NAND_FLASH_CONFIG_CS5_N_SHFT                                                     0x9
#define HWIO_NAND_FLASH_CONFIG_CS4_N_BMSK                                                   0x100
#define HWIO_NAND_FLASH_CONFIG_CS4_N_SHFT                                                     0x8
#define HWIO_NAND_FLASH_CONFIG_CS3_N_BMSK                                                    0x80
#define HWIO_NAND_FLASH_CONFIG_CS3_N_SHFT                                                     0x7
#define HWIO_NAND_FLASH_CONFIG_CS2_N_BMSK                                                    0x40
#define HWIO_NAND_FLASH_CONFIG_CS2_N_SHFT                                                     0x6
#define HWIO_NAND_FLASH_CONFIG_CS1_N_BMSK                                                    0x20
#define HWIO_NAND_FLASH_CONFIG_CS1_N_SHFT                                                     0x5
#define HWIO_NAND_FLASH_CONFIG_CS0_N_BMSK                                                    0x10
#define HWIO_NAND_FLASH_CONFIG_CS0_N_SHFT                                                     0x4
#define HWIO_NAND_FLASH_CONFIG_ALE_BMSK                                                       0x8
#define HWIO_NAND_FLASH_CONFIG_ALE_SHFT                                                       0x3
#define HWIO_NAND_FLASH_CONFIG_CLE_BMSK                                                       0x4
#define HWIO_NAND_FLASH_CONFIG_CLE_SHFT                                                       0x2
#define HWIO_NAND_FLASH_CONFIG_WE_N_BMSK                                                      0x2
#define HWIO_NAND_FLASH_CONFIG_WE_N_SHFT                                                      0x1
#define HWIO_NAND_FLASH_CONFIG_RE_N_BMSK                                                      0x1
#define HWIO_NAND_FLASH_CONFIG_RE_N_SHFT                                                      0x0

#define HWIO_NAND_FLASH_CONFIG_MODE_ADDR                                          (EBI2ND_REG_BASE      + 0x00000058)
#define HWIO_NAND_FLASH_CONFIG_MODE_PHYS                                          (EBI2ND_REG_BASE_PHYS + 0x00000058)
#define HWIO_NAND_FLASH_CONFIG_MODE_OFFS                                          (EBI2ND_REG_BASE_OFFS + 0x00000058)
#define HWIO_NAND_FLASH_CONFIG_MODE_RMSK                                                 0x1
#define HWIO_NAND_FLASH_CONFIG_MODE_IN          \
        in_dword_masked(HWIO_NAND_FLASH_CONFIG_MODE_ADDR, HWIO_NAND_FLASH_CONFIG_MODE_RMSK)
#define HWIO_NAND_FLASH_CONFIG_MODE_INM(m)      \
        in_dword_masked(HWIO_NAND_FLASH_CONFIG_MODE_ADDR, m)
#define HWIO_NAND_FLASH_CONFIG_MODE_OUT(v)      \
        out_dword(HWIO_NAND_FLASH_CONFIG_MODE_ADDR,v)
#define HWIO_NAND_FLASH_CONFIG_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_FLASH_CONFIG_MODE_ADDR,m,v,HWIO_NAND_FLASH_CONFIG_MODE_IN)
#define HWIO_NAND_FLASH_CONFIG_MODE_CONFIG_ACC_BMSK                                           0x1
#define HWIO_NAND_FLASH_CONFIG_MODE_CONFIG_ACC_SHFT                                           0x0

#define HWIO_NAND_FLASH_CONFIG_STATUS_ADDR                                        (EBI2ND_REG_BASE      + 0x00000060)
#define HWIO_NAND_FLASH_CONFIG_STATUS_PHYS                                        (EBI2ND_REG_BASE_PHYS + 0x00000060)
#define HWIO_NAND_FLASH_CONFIG_STATUS_OFFS                                        (EBI2ND_REG_BASE_OFFS + 0x00000060)
#define HWIO_NAND_FLASH_CONFIG_STATUS_RMSK                                               0x1
#define HWIO_NAND_FLASH_CONFIG_STATUS_IN          \
        in_dword_masked(HWIO_NAND_FLASH_CONFIG_STATUS_ADDR, HWIO_NAND_FLASH_CONFIG_STATUS_RMSK)
#define HWIO_NAND_FLASH_CONFIG_STATUS_INM(m)      \
        in_dword_masked(HWIO_NAND_FLASH_CONFIG_STATUS_ADDR, m)
#define HWIO_NAND_FLASH_CONFIG_STATUS_OUT(v)      \
        out_dword(HWIO_NAND_FLASH_CONFIG_STATUS_ADDR,v)
#define HWIO_NAND_FLASH_CONFIG_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_FLASH_CONFIG_STATUS_ADDR,m,v,HWIO_NAND_FLASH_CONFIG_STATUS_IN)
#define HWIO_NAND_FLASH_CONFIG_STATUS_CONFIG_MODE_BMSK                                        0x1
#define HWIO_NAND_FLASH_CONFIG_STATUS_CONFIG_MODE_SHFT                                        0x0

#define HWIO_FLASH_MACRO1_REG_ADDR                                                (EBI2ND_REG_BASE      + 0x00000064)
#define HWIO_FLASH_MACRO1_REG_PHYS                                                (EBI2ND_REG_BASE_PHYS + 0x00000064)
#define HWIO_FLASH_MACRO1_REG_OFFS                                                (EBI2ND_REG_BASE_OFFS + 0x00000064)
#define HWIO_FLASH_MACRO1_REG_RMSK                                                   0x8ffff
#define HWIO_FLASH_MACRO1_REG_IN          \
        in_dword_masked(HWIO_FLASH_MACRO1_REG_ADDR, HWIO_FLASH_MACRO1_REG_RMSK)
#define HWIO_FLASH_MACRO1_REG_INM(m)      \
        in_dword_masked(HWIO_FLASH_MACRO1_REG_ADDR, m)
#define HWIO_FLASH_MACRO1_REG_OUT(v)      \
        out_dword(HWIO_FLASH_MACRO1_REG_ADDR,v)
#define HWIO_FLASH_MACRO1_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_MACRO1_REG_ADDR,m,v,HWIO_FLASH_MACRO1_REG_IN)
#define HWIO_FLASH_MACRO1_REG_ADDR_BUS_HOLD_CYCLE_BMSK                                    0x80000
#define HWIO_FLASH_MACRO1_REG_ADDR_BUS_HOLD_CYCLE_SHFT                                       0x13
#define HWIO_FLASH_MACRO1_REG_DATA_START_ADDR_BMSK                                         0xffff
#define HWIO_FLASH_MACRO1_REG_DATA_START_ADDR_SHFT                                            0x0

#define HWIO_HSDDR_NAND_CFG_ADDR                                                  (EBI2ND_REG_BASE      + 0x00000068)
#define HWIO_HSDDR_NAND_CFG_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x00000068)
#define HWIO_HSDDR_NAND_CFG_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x00000068)
#define HWIO_HSDDR_NAND_CFG_RMSK                                                  0xffffffc3
#define HWIO_HSDDR_NAND_CFG_IN          \
        in_dword_masked(HWIO_HSDDR_NAND_CFG_ADDR, HWIO_HSDDR_NAND_CFG_RMSK)
#define HWIO_HSDDR_NAND_CFG_INM(m)      \
        in_dword_masked(HWIO_HSDDR_NAND_CFG_ADDR, m)
#define HWIO_HSDDR_NAND_CFG_OUT(v)      \
        out_dword(HWIO_HSDDR_NAND_CFG_ADDR,v)
#define HWIO_HSDDR_NAND_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_HSDDR_NAND_CFG_ADDR,m,v,HWIO_HSDDR_NAND_CFG_IN)
#define HWIO_HSDDR_NAND_CFG_NAND_CDC_SLAVE_OFFSET_VAL_BMSK                             0xff000000
#define HWIO_HSDDR_NAND_CFG_NAND_CDC_SLAVE_OFFSET_VAL_SHFT                                   0x18
#define HWIO_HSDDR_NAND_CFG_NAND_CDC_SLAVE_BYPASS_N_BMSK                                 0x800000
#define HWIO_HSDDR_NAND_CFG_NAND_CDC_SLAVE_BYPASS_N_SHFT                                     0x17
#define HWIO_HSDDR_NAND_CFG_NAND_CDC_SLAVE_BYPASS_N_BYPASS_ENABLE_FVAL                        0x0
#define HWIO_HSDDR_NAND_CFG_NAND_CDC_SLAVE_BYPASS_N_BYPASS_DISABLE_FVAL                       0x1
#define HWIO_HSDDR_NAND_CFG_NAND_CDC_LOAD_SLAVE_BMSK                                     0x400000
#define HWIO_HSDDR_NAND_CFG_NAND_CDC_LOAD_SLAVE_SHFT                                         0x16
#define HWIO_HSDDR_NAND_CFG_NAND_CDC_SLAVE_OFFSET_SEL_BMSK                               0x200000
#define HWIO_HSDDR_NAND_CFG_NAND_CDC_SLAVE_OFFSET_SEL_SHFT                                   0x15
#define HWIO_HSDDR_NAND_CFG_NANDC_CDC_GATE_LOAD_SLAVE_BMSK                               0x100000
#define HWIO_HSDDR_NAND_CFG_NANDC_CDC_GATE_LOAD_SLAVE_SHFT                                   0x14
#define HWIO_HSDDR_NAND_CFG_NANDC_CDC_GATE_LOAD_SLAVE_DO_NOT_GATE_THE_AUTO_CAL_LOAD_SIGNAL_AND_LOAD_A_VALUE_WHENEVER_THE_AUTO_CAL_LOAD_IS_GENERATED_BY_THE_SDRAM_CONTROLLER_FVAL        0x0
#define HWIO_HSDDR_NAND_CFG_NANDC_CDC_GATE_LOAD_SLAVE_GATE_THE_AUTO_CAL_LOAD_SIGNAL_FVAL        0x1
#define HWIO_HSDDR_NAND_CFG_NANDC_CAL_VALO_DOUBLE_BMSK                                    0x80000
#define HWIO_HSDDR_NAND_CFG_NANDC_CAL_VALO_DOUBLE_SHFT                                       0x13
#define HWIO_HSDDR_NAND_CFG_NANDC_CAL_VALO_DOUBLE_CAL_VALO_IS_SENT_TO_SLAVE_WITHOUT_DOUBLING_FVAL        0x0
#define HWIO_HSDDR_NAND_CFG_NANDC_CAL_VALO_DOUBLE_CAL_VALO_IS_DOUBLED_AND_SENT_TO_SLAVE_FVAL        0x1
#define HWIO_HSDDR_NAND_CFG_FIFO_READ_LATENCY_BMSK                                        0x60000
#define HWIO_HSDDR_NAND_CFG_FIFO_READ_LATENCY_SHFT                                           0x11
#define HWIO_HSDDR_NAND_CFG_FIFO_READ_LATENCY_DEFAULT_FVAL                                    0x0
#define HWIO_HSDDR_NAND_CFG_FIFO_READ_LATENCY_ENUM_6_CYCLE_LATENCY_FVAL                       0x1
#define HWIO_HSDDR_NAND_CFG_FIFO_READ_LATENCY_ENUM_7_CYCLE_LATENCY_FVAL                       0x2
#define HWIO_HSDDR_NAND_CFG_FIFO_READ_LATENCY_ENUM_8_CYCLE_LATENCY_FVAL                       0x3
#define HWIO_HSDDR_NAND_CFG_SEL_FDCDC_BMSK                                                0x10000
#define HWIO_HSDDR_NAND_CFG_SEL_FDCDC_SHFT                                                   0x10
#define HWIO_HSDDR_NAND_CFG_SEL_FDCDC_FREQUENCY_INDEPENDENT_CDC_FVAL                          0x0
#define HWIO_HSDDR_NAND_CFG_SEL_FDCDC_FREQUENCY_DEPENDENT_CDC_FVAL                            0x1
#define HWIO_HSDDR_NAND_CFG_SEL_FICDC_MASTER_BMSK                                          0x8000
#define HWIO_HSDDR_NAND_CFG_SEL_FICDC_MASTER_SHFT                                             0xf
#define HWIO_HSDDR_NAND_CFG_SW_LOAD_SLAVE_BMSK                                             0x4000
#define HWIO_HSDDR_NAND_CFG_SW_LOAD_SLAVE_SHFT                                                0xe
#define HWIO_HSDDR_NAND_CFG_SW_CAL_VALO_BMSK                                               0x3fc0
#define HWIO_HSDDR_NAND_CFG_SW_CAL_VALO_SHFT                                                  0x6
#define HWIO_HSDDR_NAND_CFG_DISABLE_CLOCK_DURING_BUSY_BMSK                                    0x2
#define HWIO_HSDDR_NAND_CFG_DISABLE_CLOCK_DURING_BUSY_SHFT                                    0x1
#define HWIO_HSDDR_NAND_CFG_DISABLE_CLOCK_DURING_BUSY_DO_NOT_DISABLE_CLOCK_DURING_BUSY_FVAL        0x0
#define HWIO_HSDDR_NAND_CFG_DISABLE_CLOCK_DURING_BUSY_DISABLE_CLOCK_DURING_BUSY_FVAL          0x1
#define HWIO_HSDDR_NAND_CFG_HS_DDR_IF_ENABLE_BMSK                                             0x1
#define HWIO_HSDDR_NAND_CFG_HS_DDR_IF_ENABLE_SHFT                                             0x0

#define HWIO_FLASH_XFR_STEP1_ADDR                                                 (EBI2ND_REG_BASE      + 0x00000070)
#define HWIO_FLASH_XFR_STEP1_PHYS                                                 (EBI2ND_REG_BASE_PHYS + 0x00000070)
#define HWIO_FLASH_XFR_STEP1_OFFS                                                 (EBI2ND_REG_BASE_OFFS + 0x00000070)
#define HWIO_FLASH_XFR_STEP1_RMSK                                                 0xfffcffff
#define HWIO_FLASH_XFR_STEP1_IN          \
        in_dword_masked(HWIO_FLASH_XFR_STEP1_ADDR, HWIO_FLASH_XFR_STEP1_RMSK)
#define HWIO_FLASH_XFR_STEP1_INM(m)      \
        in_dword_masked(HWIO_FLASH_XFR_STEP1_ADDR, m)
#define HWIO_FLASH_XFR_STEP1_OUT(v)      \
        out_dword(HWIO_FLASH_XFR_STEP1_ADDR,v)
#define HWIO_FLASH_XFR_STEP1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_XFR_STEP1_ADDR,m,v,HWIO_FLASH_XFR_STEP1_IN)
#define HWIO_FLASH_XFR_STEP1_CMD_SEQ_STEP_NUMBER_BMSK                                  0xc0000000
#define HWIO_FLASH_XFR_STEP1_CMD_SEQ_STEP_NUMBER_SHFT                                        0x1e
#define HWIO_FLASH_XFR_STEP1_CMD_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                             0x0
#define HWIO_FLASH_XFR_STEP1_CMD_SEQ_STEP_NUMBER_LOOP_START_FVAL                              0x1
#define HWIO_FLASH_XFR_STEP1_CMD_SEQ_STEP_NUMBER_LOOP_END_FVAL                                0x2
#define HWIO_FLASH_XFR_STEP1_CMD_SEQ_STEP_NUMBER_LAST_STEP_FVAL                               0x3
#define HWIO_FLASH_XFR_STEP1_CMD_STEP1_WAIT_BMSK                                       0x3c000000
#define HWIO_FLASH_XFR_STEP1_CMD_STEP1_WAIT_SHFT                                             0x1a
#define HWIO_FLASH_XFR_STEP1_CMD_AOUT_EN_BMSK                                           0x2000000
#define HWIO_FLASH_XFR_STEP1_CMD_AOUT_EN_SHFT                                                0x19
#define HWIO_FLASH_XFR_STEP1_CMD_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                          0x0
#define HWIO_FLASH_XFR_STEP1_CMD_AOUT_EN_DRIVE_ADDRESS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP1_CMD_DATA_EN_BMSK                                           0x1000000
#define HWIO_FLASH_XFR_STEP1_CMD_DATA_EN_SHFT                                                0x18
#define HWIO_FLASH_XFR_STEP1_CMD_DATA_EN_DISABLE_DATA_BUS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP1_CMD_DATA_EN_DRIVE_DATA_FVAL                                      0x1
#define HWIO_FLASH_XFR_STEP1_CMD_CE_EN_BMSK                                              0x800000
#define HWIO_FLASH_XFR_STEP1_CMD_CE_EN_SHFT                                                  0x17
#define HWIO_FLASH_XFR_STEP1_CMD_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP1_CMD_CE_EN_ASSERT_CHIP_SELECT_FVAL                                0x1
#define HWIO_FLASH_XFR_STEP1_CMD_CLE_EN_BMSK                                             0x400000
#define HWIO_FLASH_XFR_STEP1_CMD_CLE_EN_SHFT                                                 0x16
#define HWIO_FLASH_XFR_STEP1_CMD_CLE_EN_THE_LOGIC_DE_ASSERTS_FVAL                             0x0
#define HWIO_FLASH_XFR_STEP1_CMD_CLE_EN_THE_LOGIC_ASSERTS_FVAL                                0x1
#define HWIO_FLASH_XFR_STEP1_CMD_ALE_PIN_BMSK                                            0x200000
#define HWIO_FLASH_XFR_STEP1_CMD_ALE_PIN_SHFT                                                0x15
#define HWIO_FLASH_XFR_STEP1_CMD_ALE_PIN_THE_LOGIC_DE_ASSERTS_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP1_CMD_ALE_PIN_THE_LOGIC_ASSERTS_FVAL                               0x1
#define HWIO_FLASH_XFR_STEP1_CMD_WE_EN_BMSK                                              0x100000
#define HWIO_FLASH_XFR_STEP1_CMD_WE_EN_SHFT                                                  0x14
#define HWIO_FLASH_XFR_STEP1_CMD_WE_EN_THE_LOGIC_DE_ASSERTS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP1_CMD_WE_EN_THE_LOGIC_ASSERTS_FVAL                                 0x1
#define HWIO_FLASH_XFR_STEP1_CMD_RE_EN_BMSK                                               0x80000
#define HWIO_FLASH_XFR_STEP1_CMD_RE_EN_SHFT                                                  0x13
#define HWIO_FLASH_XFR_STEP1_CMD_RE_EN_THE_LOGIC_DE_ASSERTS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP1_CMD_RE_EN_THE_LOGIC_ASSERTS_FVAL                                 0x1
#define HWIO_FLASH_XFR_STEP1_CMD_WIDE_BMSK                                                0x40000
#define HWIO_FLASH_XFR_STEP1_CMD_WIDE_SHFT                                                   0x12
#define HWIO_FLASH_XFR_STEP1_CMD_WIDE_SEND_CMD_ON_8_BIT_BUS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP1_CMD_WIDE_SEND_CMD_ON_16_BIT_BUS_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP1_DATA_SEQ_STEP_NUMBER_BMSK                                     0xc000
#define HWIO_FLASH_XFR_STEP1_DATA_SEQ_STEP_NUMBER_SHFT                                        0xe
#define HWIO_FLASH_XFR_STEP1_DATA_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP1_DATA_SEQ_STEP_NUMBER_LOOP_START_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP1_DATA_SEQ_STEP_NUMBER_LOOP_END_FVAL                               0x2
#define HWIO_FLASH_XFR_STEP1_DATA_SEQ_STEP_NUMBER_LAST_STEP_FVAL                              0x3
#define HWIO_FLASH_XFR_STEP1_DATA_STEP1_WAIT_BMSK                                          0x3c00
#define HWIO_FLASH_XFR_STEP1_DATA_STEP1_WAIT_SHFT                                             0xa
#define HWIO_FLASH_XFR_STEP1_DATA_AOUT_EN_BMSK                                              0x200
#define HWIO_FLASH_XFR_STEP1_DATA_AOUT_EN_SHFT                                                0x9
#define HWIO_FLASH_XFR_STEP1_DATA_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                         0x0
#define HWIO_FLASH_XFR_STEP1_DATA_AOUT_EN_DRIVE_ADDRESS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP1_DATA_DATA_EN_BMSK                                              0x100
#define HWIO_FLASH_XFR_STEP1_DATA_DATA_EN_SHFT                                                0x8
#define HWIO_FLASH_XFR_STEP1_DATA_DATA_EN_DISABLE_DATA_BUS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP1_DATA_DATA_EN_DRIVE_DATA_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP1_DATA_CE_EN_BMSK                                                 0x80
#define HWIO_FLASH_XFR_STEP1_DATA_CE_EN_SHFT                                                  0x7
#define HWIO_FLASH_XFR_STEP1_DATA_CE_EN_DE_ASSERT_BOTH_CHIP_SELECTS_FVAL                      0x0
#define HWIO_FLASH_XFR_STEP1_DATA_CE_EN_ASSERT_CHIP_SELECT_FVAL                               0x1
#define HWIO_FLASH_XFR_STEP1_DATA_CLE_EN_BMSK                                                0x40
#define HWIO_FLASH_XFR_STEP1_DATA_CLE_EN_SHFT                                                 0x6
#define HWIO_FLASH_XFR_STEP1_DATA_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP1_DATA_CLE_EN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP1_DATA_ALE_PIN_BMSK                                               0x20
#define HWIO_FLASH_XFR_STEP1_DATA_ALE_PIN_SHFT                                                0x5
#define HWIO_FLASH_XFR_STEP1_DATA_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP1_DATA_ALE_PIN_LOGIC_ASSERTS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP1_DATA_WE_EN_BMSK                                                 0x10
#define HWIO_FLASH_XFR_STEP1_DATA_WE_EN_SHFT                                                  0x4
#define HWIO_FLASH_XFR_STEP1_DATA_WE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP1_DATA_WE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP1_DATA_RE_EN_BMSK                                                  0x8
#define HWIO_FLASH_XFR_STEP1_DATA_RE_EN_SHFT                                                  0x3
#define HWIO_FLASH_XFR_STEP1_DATA_RE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP1_DATA_RE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP1_DATA_WIDE_BMSK                                                   0x4
#define HWIO_FLASH_XFR_STEP1_DATA_WIDE_SHFT                                                   0x2
#define HWIO_FLASH_XFR_STEP1_DATA_WIDE_USE_8_BIT_DATA_BUS_FOR_DATA_FVAL                       0x0
#define HWIO_FLASH_XFR_STEP1_DATA_WIDE_USE16_BIT_DATA_BUS_FOR_DATA_FVAL                       0x1
#define HWIO_FLASH_XFR_STEP1_EXTA_READ_WAIT_BMSK                                              0x3
#define HWIO_FLASH_XFR_STEP1_EXTA_READ_WAIT_SHFT                                              0x0

#define HWIO_FLASH_XFR_STEP2_ADDR                                                 (EBI2ND_REG_BASE      + 0x00000074)
#define HWIO_FLASH_XFR_STEP2_PHYS                                                 (EBI2ND_REG_BASE_PHYS + 0x00000074)
#define HWIO_FLASH_XFR_STEP2_OFFS                                                 (EBI2ND_REG_BASE_OFFS + 0x00000074)
#define HWIO_FLASH_XFR_STEP2_RMSK                                                 0xfffcffff
#define HWIO_FLASH_XFR_STEP2_IN          \
        in_dword_masked(HWIO_FLASH_XFR_STEP2_ADDR, HWIO_FLASH_XFR_STEP2_RMSK)
#define HWIO_FLASH_XFR_STEP2_INM(m)      \
        in_dword_masked(HWIO_FLASH_XFR_STEP2_ADDR, m)
#define HWIO_FLASH_XFR_STEP2_OUT(v)      \
        out_dword(HWIO_FLASH_XFR_STEP2_ADDR,v)
#define HWIO_FLASH_XFR_STEP2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_XFR_STEP2_ADDR,m,v,HWIO_FLASH_XFR_STEP2_IN)
#define HWIO_FLASH_XFR_STEP2_CMD_SEQ_STEP_NUMBER_BMSK                                  0xc0000000
#define HWIO_FLASH_XFR_STEP2_CMD_SEQ_STEP_NUMBER_SHFT                                        0x1e
#define HWIO_FLASH_XFR_STEP2_CMD_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                             0x0
#define HWIO_FLASH_XFR_STEP2_CMD_SEQ_STEP_NUMBER_LOOP_START_FVAL                              0x1
#define HWIO_FLASH_XFR_STEP2_CMD_SEQ_STEP_NUMBER_LOOP_END_FVAL                                0x2
#define HWIO_FLASH_XFR_STEP2_CMD_SEQ_STEP_NUMBER_LAST_STEP_FVAL                               0x3
#define HWIO_FLASH_XFR_STEP2_CMD_STEP1_WAIT_BMSK                                       0x3c000000
#define HWIO_FLASH_XFR_STEP2_CMD_STEP1_WAIT_SHFT                                             0x1a
#define HWIO_FLASH_XFR_STEP2_CMD_AOUT_EN_BMSK                                           0x2000000
#define HWIO_FLASH_XFR_STEP2_CMD_AOUT_EN_SHFT                                                0x19
#define HWIO_FLASH_XFR_STEP2_CMD_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                          0x0
#define HWIO_FLASH_XFR_STEP2_CMD_AOUT_EN_DRIVE_ADDRESS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP2_CMD_DATA_EN_BMSK                                           0x1000000
#define HWIO_FLASH_XFR_STEP2_CMD_DATA_EN_SHFT                                                0x18
#define HWIO_FLASH_XFR_STEP2_CMD_DATA_EN_DISABLE_DATA_BUS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP2_CMD_DATA_EN_DRIVE_DATA_FVAL                                      0x1
#define HWIO_FLASH_XFR_STEP2_CMD_CE_EN_BMSK                                              0x800000
#define HWIO_FLASH_XFR_STEP2_CMD_CE_EN_SHFT                                                  0x17
#define HWIO_FLASH_XFR_STEP2_CMD_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP2_CMD_CE_EN_ASSERT_CHIP_SELECT_FVAL                                0x1
#define HWIO_FLASH_XFR_STEP2_CMD_CLE_EN_BMSK                                             0x400000
#define HWIO_FLASH_XFR_STEP2_CMD_CLE_EN_SHFT                                                 0x16
#define HWIO_FLASH_XFR_STEP2_CMD_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP2_CMD_CLE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP2_CMD_ALE_PIN_BMSK                                            0x200000
#define HWIO_FLASH_XFR_STEP2_CMD_ALE_PIN_SHFT                                                0x15
#define HWIO_FLASH_XFR_STEP2_CMD_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP2_CMD_ALE_PIN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP2_CMD_WE_EN_BMSK                                              0x100000
#define HWIO_FLASH_XFR_STEP2_CMD_WE_EN_SHFT                                                  0x14
#define HWIO_FLASH_XFR_STEP2_CMD_WE_EN_LOGIC_DE_ASSERTS_FVAL                                  0x0
#define HWIO_FLASH_XFR_STEP2_CMD_WE_EN_LOGIC_ASSERTS_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP2_CMD_RE_EN_BMSK                                               0x80000
#define HWIO_FLASH_XFR_STEP2_CMD_RE_EN_SHFT                                                  0x13
#define HWIO_FLASH_XFR_STEP2_CMD_RE_EN_LOGIC_DE_ASSERTS_FVAL                                  0x0
#define HWIO_FLASH_XFR_STEP2_CMD_RE_EN_LOGIC_ASSERTS_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP2_CMD_WIDE_BMSK                                                0x40000
#define HWIO_FLASH_XFR_STEP2_CMD_WIDE_SHFT                                                   0x12
#define HWIO_FLASH_XFR_STEP2_CMD_WIDE_SEND_CMD_ON_8_BIT_BUS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP2_CMD_WIDE_SEND_CMD_ON_16_BIT_BUS_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP2_DATA_SEQ_STEP_NUMBER_BMSK                                     0xc000
#define HWIO_FLASH_XFR_STEP2_DATA_SEQ_STEP_NUMBER_SHFT                                        0xe
#define HWIO_FLASH_XFR_STEP2_DATA_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP2_DATA_SEQ_STEP_NUMBER_LOOP_START_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP2_DATA_SEQ_STEP_NUMBER_LOOP_END_FVAL                               0x2
#define HWIO_FLASH_XFR_STEP2_DATA_SEQ_STEP_NUMBER_LAST_STEP_FVAL                              0x3
#define HWIO_FLASH_XFR_STEP2_DATA_STEP1_WAIT_BMSK                                          0x3c00
#define HWIO_FLASH_XFR_STEP2_DATA_STEP1_WAIT_SHFT                                             0xa
#define HWIO_FLASH_XFR_STEP2_DATA_AOUT_EN_BMSK                                              0x200
#define HWIO_FLASH_XFR_STEP2_DATA_AOUT_EN_SHFT                                                0x9
#define HWIO_FLASH_XFR_STEP2_DATA_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                         0x0
#define HWIO_FLASH_XFR_STEP2_DATA_AOUT_EN_DRIVE_ADDRESS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP2_DATA_DATA_EN_BMSK                                              0x100
#define HWIO_FLASH_XFR_STEP2_DATA_DATA_EN_SHFT                                                0x8
#define HWIO_FLASH_XFR_STEP2_DATA_DATA_EN_DISABLE_DATA_BUS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP2_DATA_DATA_EN_DRIVE_DATA_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP2_DATA_CE_EN_BMSK                                                 0x80
#define HWIO_FLASH_XFR_STEP2_DATA_CE_EN_SHFT                                                  0x7
#define HWIO_FLASH_XFR_STEP2_DATA_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                           0x0
#define HWIO_FLASH_XFR_STEP2_DATA_CE_EN_ASSERT_CHIP_SELECT_FVAL                               0x1
#define HWIO_FLASH_XFR_STEP2_DATA_CLE_EN_BMSK                                                0x40
#define HWIO_FLASH_XFR_STEP2_DATA_CLE_EN_SHFT                                                 0x6
#define HWIO_FLASH_XFR_STEP2_DATA_CLE_EN_THE_LOGIC_DE_ASSERTS_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP2_DATA_CLE_EN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP2_DATA_ALE_PIN_BMSK                                               0x20
#define HWIO_FLASH_XFR_STEP2_DATA_ALE_PIN_SHFT                                                0x5
#define HWIO_FLASH_XFR_STEP2_DATA_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP2_DATA_ALE_PIN_LOGIC_ASSERTS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP2_DATA_WE_EN_BMSK                                                 0x10
#define HWIO_FLASH_XFR_STEP2_DATA_WE_EN_SHFT                                                  0x4
#define HWIO_FLASH_XFR_STEP2_DATA_WE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP2_DATA_WE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP2_DATA_RE_EN_BMSK                                                  0x8
#define HWIO_FLASH_XFR_STEP2_DATA_RE_EN_SHFT                                                  0x3
#define HWIO_FLASH_XFR_STEP2_DATA_RE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP2_DATA_RE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP2_DATA_WIDE_BMSK                                                   0x4
#define HWIO_FLASH_XFR_STEP2_DATA_WIDE_SHFT                                                   0x2
#define HWIO_FLASH_XFR_STEP2_DATA_WIDE_USE_8_BIT_DATA_BUS_FOR_DATA_FVAL                       0x0
#define HWIO_FLASH_XFR_STEP2_DATA_WIDE_USE16_BIT_DATA_BUS_FOR_DATA_FVAL                       0x1
#define HWIO_FLASH_XFR_STEP2_EXTA_READ_WAIT_BMSK                                              0x3
#define HWIO_FLASH_XFR_STEP2_EXTA_READ_WAIT_SHFT                                              0x0

#define HWIO_FLASH_XFR_STEP3_ADDR                                                 (EBI2ND_REG_BASE      + 0x00000078)
#define HWIO_FLASH_XFR_STEP3_PHYS                                                 (EBI2ND_REG_BASE_PHYS + 0x00000078)
#define HWIO_FLASH_XFR_STEP3_OFFS                                                 (EBI2ND_REG_BASE_OFFS + 0x00000078)
#define HWIO_FLASH_XFR_STEP3_RMSK                                                 0xfffcffff
#define HWIO_FLASH_XFR_STEP3_IN          \
        in_dword_masked(HWIO_FLASH_XFR_STEP3_ADDR, HWIO_FLASH_XFR_STEP3_RMSK)
#define HWIO_FLASH_XFR_STEP3_INM(m)      \
        in_dword_masked(HWIO_FLASH_XFR_STEP3_ADDR, m)
#define HWIO_FLASH_XFR_STEP3_OUT(v)      \
        out_dword(HWIO_FLASH_XFR_STEP3_ADDR,v)
#define HWIO_FLASH_XFR_STEP3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_XFR_STEP3_ADDR,m,v,HWIO_FLASH_XFR_STEP3_IN)
#define HWIO_FLASH_XFR_STEP3_CMD_SEQ_STEP_NUMBER_BMSK                                  0xc0000000
#define HWIO_FLASH_XFR_STEP3_CMD_SEQ_STEP_NUMBER_SHFT                                        0x1e
#define HWIO_FLASH_XFR_STEP3_CMD_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                             0x0
#define HWIO_FLASH_XFR_STEP3_CMD_SEQ_STEP_NUMBER_LOOP_START_FVAL                              0x1
#define HWIO_FLASH_XFR_STEP3_CMD_SEQ_STEP_NUMBER_LOOP_END_FVAL                                0x2
#define HWIO_FLASH_XFR_STEP3_CMD_SEQ_STEP_NUMBER_LAST_STEP_FVAL                               0x3
#define HWIO_FLASH_XFR_STEP3_CMD_STEP1_WAIT_BMSK                                       0x3c000000
#define HWIO_FLASH_XFR_STEP3_CMD_STEP1_WAIT_SHFT                                             0x1a
#define HWIO_FLASH_XFR_STEP3_CMD_AOUT_EN_BMSK                                           0x2000000
#define HWIO_FLASH_XFR_STEP3_CMD_AOUT_EN_SHFT                                                0x19
#define HWIO_FLASH_XFR_STEP3_CMD_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                          0x0
#define HWIO_FLASH_XFR_STEP3_CMD_AOUT_EN_DRIVE_ADDRESS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP3_CMD_DATA_EN_BMSK                                           0x1000000
#define HWIO_FLASH_XFR_STEP3_CMD_DATA_EN_SHFT                                                0x18
#define HWIO_FLASH_XFR_STEP3_CMD_DATA_EN_DISABLE_DATA_BUS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP3_CMD_DATA_EN_DRIVE_DATA_FVAL                                      0x1
#define HWIO_FLASH_XFR_STEP3_CMD_CE_EN_BMSK                                              0x800000
#define HWIO_FLASH_XFR_STEP3_CMD_CE_EN_SHFT                                                  0x17
#define HWIO_FLASH_XFR_STEP3_CMD_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP3_CMD_CE_EN_ASSERT_CHIP_SELECT_FVAL                                0x1
#define HWIO_FLASH_XFR_STEP3_CMD_CLE_EN_BMSK                                             0x400000
#define HWIO_FLASH_XFR_STEP3_CMD_CLE_EN_SHFT                                                 0x16
#define HWIO_FLASH_XFR_STEP3_CMD_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP3_CMD_CLE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP3_CMD_ALE_PIN_BMSK                                            0x200000
#define HWIO_FLASH_XFR_STEP3_CMD_ALE_PIN_SHFT                                                0x15
#define HWIO_FLASH_XFR_STEP3_CMD_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP3_CMD_ALE_PIN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP3_CMD_WE_EN_BMSK                                              0x100000
#define HWIO_FLASH_XFR_STEP3_CMD_WE_EN_SHFT                                                  0x14
#define HWIO_FLASH_XFR_STEP3_CMD_WE_EN_LOGIC_DE_ASSERTS_FVAL                                  0x0
#define HWIO_FLASH_XFR_STEP3_CMD_WE_EN_LOGIC_ASSERTS_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP3_CMD_RE_EN_BMSK                                               0x80000
#define HWIO_FLASH_XFR_STEP3_CMD_RE_EN_SHFT                                                  0x13
#define HWIO_FLASH_XFR_STEP3_CMD_RE_EN_LOGIC_DE_ASSERTS_FVAL                                  0x0
#define HWIO_FLASH_XFR_STEP3_CMD_RE_EN_LOGIC_ASSERTS_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP3_CMD_WIDE_BMSK                                                0x40000
#define HWIO_FLASH_XFR_STEP3_CMD_WIDE_SHFT                                                   0x12
#define HWIO_FLASH_XFR_STEP3_CMD_WIDE_SEND_CMD_ON_8_BIT_BUS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP3_CMD_WIDE_SEND_CMD_ON_16_BIT_BUS_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP3_DATA_SEQ_STEP_NUMBER_BMSK                                     0xc000
#define HWIO_FLASH_XFR_STEP3_DATA_SEQ_STEP_NUMBER_SHFT                                        0xe
#define HWIO_FLASH_XFR_STEP3_DATA_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP3_DATA_SEQ_STEP_NUMBER_LOOP_START_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP3_DATA_SEQ_STEP_NUMBER_LOOP_END_FVAL                               0x2
#define HWIO_FLASH_XFR_STEP3_DATA_SEQ_STEP_NUMBER_LAST_STEP_FVAL                              0x3
#define HWIO_FLASH_XFR_STEP3_DATA_STEP1_WAIT_BMSK                                          0x3c00
#define HWIO_FLASH_XFR_STEP3_DATA_STEP1_WAIT_SHFT                                             0xa
#define HWIO_FLASH_XFR_STEP3_DATA_AOUT_EN_BMSK                                              0x200
#define HWIO_FLASH_XFR_STEP3_DATA_AOUT_EN_SHFT                                                0x9
#define HWIO_FLASH_XFR_STEP3_DATA_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                         0x0
#define HWIO_FLASH_XFR_STEP3_DATA_AOUT_EN_DRIVE_ADDRESS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP3_DATA_DATA_EN_BMSK                                              0x100
#define HWIO_FLASH_XFR_STEP3_DATA_DATA_EN_SHFT                                                0x8
#define HWIO_FLASH_XFR_STEP3_DATA_DATA_EN_DISABLE_DATA_BUS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP3_DATA_DATA_EN_DRIVE_DATA_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP3_DATA_CE_EN_BMSK                                                 0x80
#define HWIO_FLASH_XFR_STEP3_DATA_CE_EN_SHFT                                                  0x7
#define HWIO_FLASH_XFR_STEP3_DATA_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                           0x0
#define HWIO_FLASH_XFR_STEP3_DATA_CE_EN_ASSERT_CHIP_SELECT_FVAL                               0x1
#define HWIO_FLASH_XFR_STEP3_DATA_CLE_EN_BMSK                                                0x40
#define HWIO_FLASH_XFR_STEP3_DATA_CLE_EN_SHFT                                                 0x6
#define HWIO_FLASH_XFR_STEP3_DATA_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP3_DATA_CLE_EN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP3_DATA_ALE_PIN_BMSK                                               0x20
#define HWIO_FLASH_XFR_STEP3_DATA_ALE_PIN_SHFT                                                0x5
#define HWIO_FLASH_XFR_STEP3_DATA_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP3_DATA_ALE_PIN_LOGIC_ASSERTS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP3_DATA_WE_EN_BMSK                                                 0x10
#define HWIO_FLASH_XFR_STEP3_DATA_WE_EN_SHFT                                                  0x4
#define HWIO_FLASH_XFR_STEP3_DATA_WE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP3_DATA_WE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP3_DATA_RE_EN_BMSK                                                  0x8
#define HWIO_FLASH_XFR_STEP3_DATA_RE_EN_SHFT                                                  0x3
#define HWIO_FLASH_XFR_STEP3_DATA_RE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP3_DATA_RE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP3_DATA_WIDE_BMSK                                                   0x4
#define HWIO_FLASH_XFR_STEP3_DATA_WIDE_SHFT                                                   0x2
#define HWIO_FLASH_XFR_STEP3_DATA_WIDE_USE_8_BIT_DATA_BUS_FOR_DATA_FVAL                       0x0
#define HWIO_FLASH_XFR_STEP3_DATA_WIDE_USE16_BIT_DATA_BUS_FOR_DATA_FVAL                       0x1
#define HWIO_FLASH_XFR_STEP3_EXTA_READ_WAIT_BMSK                                              0x3
#define HWIO_FLASH_XFR_STEP3_EXTA_READ_WAIT_SHFT                                              0x0

#define HWIO_FLASH_XFR_STEP4_ADDR                                                 (EBI2ND_REG_BASE      + 0x0000007c)
#define HWIO_FLASH_XFR_STEP4_PHYS                                                 (EBI2ND_REG_BASE_PHYS + 0x0000007c)
#define HWIO_FLASH_XFR_STEP4_OFFS                                                 (EBI2ND_REG_BASE_OFFS + 0x0000007c)
#define HWIO_FLASH_XFR_STEP4_RMSK                                                 0xfffcffff
#define HWIO_FLASH_XFR_STEP4_IN          \
        in_dword_masked(HWIO_FLASH_XFR_STEP4_ADDR, HWIO_FLASH_XFR_STEP4_RMSK)
#define HWIO_FLASH_XFR_STEP4_INM(m)      \
        in_dword_masked(HWIO_FLASH_XFR_STEP4_ADDR, m)
#define HWIO_FLASH_XFR_STEP4_OUT(v)      \
        out_dword(HWIO_FLASH_XFR_STEP4_ADDR,v)
#define HWIO_FLASH_XFR_STEP4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_XFR_STEP4_ADDR,m,v,HWIO_FLASH_XFR_STEP4_IN)
#define HWIO_FLASH_XFR_STEP4_CMD_SEQ_STEP_NUMBER_BMSK                                  0xc0000000
#define HWIO_FLASH_XFR_STEP4_CMD_SEQ_STEP_NUMBER_SHFT                                        0x1e
#define HWIO_FLASH_XFR_STEP4_CMD_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                             0x0
#define HWIO_FLASH_XFR_STEP4_CMD_SEQ_STEP_NUMBER_LOOP_START_FVAL                              0x1
#define HWIO_FLASH_XFR_STEP4_CMD_SEQ_STEP_NUMBER_LOOP_END_FVAL                                0x2
#define HWIO_FLASH_XFR_STEP4_CMD_SEQ_STEP_NUMBER_LAST_STEP_FVAL                               0x3
#define HWIO_FLASH_XFR_STEP4_CMD_STEP1_WAIT_BMSK                                       0x3c000000
#define HWIO_FLASH_XFR_STEP4_CMD_STEP1_WAIT_SHFT                                             0x1a
#define HWIO_FLASH_XFR_STEP4_CMD_AOUT_EN_BMSK                                           0x2000000
#define HWIO_FLASH_XFR_STEP4_CMD_AOUT_EN_SHFT                                                0x19
#define HWIO_FLASH_XFR_STEP4_CMD_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                          0x0
#define HWIO_FLASH_XFR_STEP4_CMD_AOUT_EN_DRIVE_ADDRESS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP4_CMD_DATA_EN_BMSK                                           0x1000000
#define HWIO_FLASH_XFR_STEP4_CMD_DATA_EN_SHFT                                                0x18
#define HWIO_FLASH_XFR_STEP4_CMD_DATA_EN_DISABLE_DATA_BUS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP4_CMD_DATA_EN_DRIVE_DATA_FVAL                                      0x1
#define HWIO_FLASH_XFR_STEP4_CMD_CE_EN_BMSK                                              0x800000
#define HWIO_FLASH_XFR_STEP4_CMD_CE_EN_SHFT                                                  0x17
#define HWIO_FLASH_XFR_STEP4_CMD_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP4_CMD_CE_EN_ASSERT_CHIP_SELECT_FVAL                                0x1
#define HWIO_FLASH_XFR_STEP4_CMD_CLE_EN_BMSK                                             0x400000
#define HWIO_FLASH_XFR_STEP4_CMD_CLE_EN_SHFT                                                 0x16
#define HWIO_FLASH_XFR_STEP4_CMD_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP4_CMD_CLE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP4_CMD_ALE_PIN_BMSK                                            0x200000
#define HWIO_FLASH_XFR_STEP4_CMD_ALE_PIN_SHFT                                                0x15
#define HWIO_FLASH_XFR_STEP4_CMD_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP4_CMD_ALE_PIN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP4_CMD_WE_EN_BMSK                                              0x100000
#define HWIO_FLASH_XFR_STEP4_CMD_WE_EN_SHFT                                                  0x14
#define HWIO_FLASH_XFR_STEP4_CMD_WE_EN_THE_LOGIC_DE_ASSERTS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP4_CMD_WE_EN_THE_LOGIC_ASSERTS_FVAL                                 0x1
#define HWIO_FLASH_XFR_STEP4_CMD_RE_EN_BMSK                                               0x80000
#define HWIO_FLASH_XFR_STEP4_CMD_RE_EN_SHFT                                                  0x13
#define HWIO_FLASH_XFR_STEP4_CMD_RE_EN_THE_LOGIC_DE_ASSERTS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP4_CMD_RE_EN_THE_LOGIC_ASSERTS_FVAL                                 0x1
#define HWIO_FLASH_XFR_STEP4_CMD_WIDE_BMSK                                                0x40000
#define HWIO_FLASH_XFR_STEP4_CMD_WIDE_SHFT                                                   0x12
#define HWIO_FLASH_XFR_STEP4_CMD_WIDE_SEND_CMD_ON_8_BIT_BUS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP4_CMD_WIDE_SEND_CMD_ON_16_BIT_BUS_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP4_DATA_SEQ_STEP_NUMBER_BMSK                                     0xc000
#define HWIO_FLASH_XFR_STEP4_DATA_SEQ_STEP_NUMBER_SHFT                                        0xe
#define HWIO_FLASH_XFR_STEP4_DATA_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP4_DATA_SEQ_STEP_NUMBER_LOOP_START_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP4_DATA_SEQ_STEP_NUMBER_LOOP_END_FVAL                               0x2
#define HWIO_FLASH_XFR_STEP4_DATA_SEQ_STEP_NUMBER_LAST_STEP_FVAL                              0x3
#define HWIO_FLASH_XFR_STEP4_DATA_STEP1_WAIT_BMSK                                          0x3c00
#define HWIO_FLASH_XFR_STEP4_DATA_STEP1_WAIT_SHFT                                             0xa
#define HWIO_FLASH_XFR_STEP4_DATA_AOUT_EN_BMSK                                              0x200
#define HWIO_FLASH_XFR_STEP4_DATA_AOUT_EN_SHFT                                                0x9
#define HWIO_FLASH_XFR_STEP4_DATA_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                         0x0
#define HWIO_FLASH_XFR_STEP4_DATA_AOUT_EN_DRIVE_ADDRESS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP4_DATA_DATA_EN_BMSK                                              0x100
#define HWIO_FLASH_XFR_STEP4_DATA_DATA_EN_SHFT                                                0x8
#define HWIO_FLASH_XFR_STEP4_DATA_DATA_EN_DISABLE_DATA_BUS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP4_DATA_DATA_EN_DRIVE_DATA_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP4_DATA_CE_EN_BMSK                                                 0x80
#define HWIO_FLASH_XFR_STEP4_DATA_CE_EN_SHFT                                                  0x7
#define HWIO_FLASH_XFR_STEP4_DATA_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                           0x0
#define HWIO_FLASH_XFR_STEP4_DATA_CE_EN_ASSERT_CHIP_SELECT_FVAL                               0x1
#define HWIO_FLASH_XFR_STEP4_DATA_CLE_EN_BMSK                                                0x40
#define HWIO_FLASH_XFR_STEP4_DATA_CLE_EN_SHFT                                                 0x6
#define HWIO_FLASH_XFR_STEP4_DATA_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP4_DATA_CLE_EN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP4_DATA_ALE_PIN_BMSK                                               0x20
#define HWIO_FLASH_XFR_STEP4_DATA_ALE_PIN_SHFT                                                0x5
#define HWIO_FLASH_XFR_STEP4_DATA_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP4_DATA_ALE_PIN_LOGIC_ASSERTS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP4_DATA_WE_EN_BMSK                                                 0x10
#define HWIO_FLASH_XFR_STEP4_DATA_WE_EN_SHFT                                                  0x4
#define HWIO_FLASH_XFR_STEP4_DATA_WE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP4_DATA_WE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP4_DATA_RE_EN_BMSK                                                  0x8
#define HWIO_FLASH_XFR_STEP4_DATA_RE_EN_SHFT                                                  0x3
#define HWIO_FLASH_XFR_STEP4_DATA_RE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP4_DATA_RE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP4_DATA_WIDE_BMSK                                                   0x4
#define HWIO_FLASH_XFR_STEP4_DATA_WIDE_SHFT                                                   0x2
#define HWIO_FLASH_XFR_STEP4_DATA_WIDE_USE_8_BIT_DATA_BUS_FOR_DATA_FVAL                       0x0
#define HWIO_FLASH_XFR_STEP4_DATA_WIDE_USE16_BIT_DATA_BUS_FOR_DATA_FVAL                       0x1
#define HWIO_FLASH_XFR_STEP4_EXTA_READ_WAIT_BMSK                                              0x3
#define HWIO_FLASH_XFR_STEP4_EXTA_READ_WAIT_SHFT                                              0x0

#define HWIO_FLASH_XFR_STEP5_ADDR                                                 (EBI2ND_REG_BASE      + 0x00000080)
#define HWIO_FLASH_XFR_STEP5_PHYS                                                 (EBI2ND_REG_BASE_PHYS + 0x00000080)
#define HWIO_FLASH_XFR_STEP5_OFFS                                                 (EBI2ND_REG_BASE_OFFS + 0x00000080)
#define HWIO_FLASH_XFR_STEP5_RMSK                                                 0xfffcffff
#define HWIO_FLASH_XFR_STEP5_IN          \
        in_dword_masked(HWIO_FLASH_XFR_STEP5_ADDR, HWIO_FLASH_XFR_STEP5_RMSK)
#define HWIO_FLASH_XFR_STEP5_INM(m)      \
        in_dword_masked(HWIO_FLASH_XFR_STEP5_ADDR, m)
#define HWIO_FLASH_XFR_STEP5_OUT(v)      \
        out_dword(HWIO_FLASH_XFR_STEP5_ADDR,v)
#define HWIO_FLASH_XFR_STEP5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_XFR_STEP5_ADDR,m,v,HWIO_FLASH_XFR_STEP5_IN)
#define HWIO_FLASH_XFR_STEP5_CMD_SEQ_STEP_NUMBER_BMSK                                  0xc0000000
#define HWIO_FLASH_XFR_STEP5_CMD_SEQ_STEP_NUMBER_SHFT                                        0x1e
#define HWIO_FLASH_XFR_STEP5_CMD_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                             0x0
#define HWIO_FLASH_XFR_STEP5_CMD_SEQ_STEP_NUMBER_LOOP_START_FVAL                              0x1
#define HWIO_FLASH_XFR_STEP5_CMD_SEQ_STEP_NUMBER_LOOP_END_FVAL                                0x2
#define HWIO_FLASH_XFR_STEP5_CMD_SEQ_STEP_NUMBER_LAST_STEP_FVAL                               0x3
#define HWIO_FLASH_XFR_STEP5_CMD_STEP1_WAIT_BMSK                                       0x3c000000
#define HWIO_FLASH_XFR_STEP5_CMD_STEP1_WAIT_SHFT                                             0x1a
#define HWIO_FLASH_XFR_STEP5_CMD_AOUT_EN_BMSK                                           0x2000000
#define HWIO_FLASH_XFR_STEP5_CMD_AOUT_EN_SHFT                                                0x19
#define HWIO_FLASH_XFR_STEP5_CMD_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                          0x0
#define HWIO_FLASH_XFR_STEP5_CMD_AOUT_EN_DRIVE_ADDRESS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP5_CMD_DATA_EN_BMSK                                           0x1000000
#define HWIO_FLASH_XFR_STEP5_CMD_DATA_EN_SHFT                                                0x18
#define HWIO_FLASH_XFR_STEP5_CMD_DATA_EN_DISABLE_DATA_BUS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP5_CMD_DATA_EN_DRIVE_DATA_FVAL                                      0x1
#define HWIO_FLASH_XFR_STEP5_CMD_CE_EN_BMSK                                              0x800000
#define HWIO_FLASH_XFR_STEP5_CMD_CE_EN_SHFT                                                  0x17
#define HWIO_FLASH_XFR_STEP5_CMD_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP5_CMD_CE_EN_ASSERT_CHIP_SELECT_FVAL                                0x1
#define HWIO_FLASH_XFR_STEP5_CMD_CLE_EN_BMSK                                             0x400000
#define HWIO_FLASH_XFR_STEP5_CMD_CLE_EN_SHFT                                                 0x16
#define HWIO_FLASH_XFR_STEP5_CMD_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP5_CMD_CLE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP5_CMD_ALE_PIN_BMSK                                            0x200000
#define HWIO_FLASH_XFR_STEP5_CMD_ALE_PIN_SHFT                                                0x15
#define HWIO_FLASH_XFR_STEP5_CMD_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP5_CMD_ALE_PIN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP5_CMD_WE_EN_BMSK                                              0x100000
#define HWIO_FLASH_XFR_STEP5_CMD_WE_EN_SHFT                                                  0x14
#define HWIO_FLASH_XFR_STEP5_CMD_WE_EN_LOGIC_DE_ASSERTS_FVAL                                  0x0
#define HWIO_FLASH_XFR_STEP5_CMD_WE_EN_LOGIC_ASSERTS_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP5_CMD_RE_EN_BMSK                                               0x80000
#define HWIO_FLASH_XFR_STEP5_CMD_RE_EN_SHFT                                                  0x13
#define HWIO_FLASH_XFR_STEP5_CMD_RE_EN_LOGIC_DE_ASSERTS_FVAL                                  0x0
#define HWIO_FLASH_XFR_STEP5_CMD_RE_EN_LOGIC_ASSERTS_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP5_CMD_WIDE_BMSK                                                0x40000
#define HWIO_FLASH_XFR_STEP5_CMD_WIDE_SHFT                                                   0x12
#define HWIO_FLASH_XFR_STEP5_CMD_WIDE_SEND_CMD_ON_8_BIT_BUS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP5_CMD_WIDE_SEND_CMD_ON_16_BIT_BUS_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP5_DATA_SEQ_STEP_NUMBER_BMSK                                     0xc000
#define HWIO_FLASH_XFR_STEP5_DATA_SEQ_STEP_NUMBER_SHFT                                        0xe
#define HWIO_FLASH_XFR_STEP5_DATA_SEQ_STEP_NUMBER_LOOP_START_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP5_DATA_SEQ_STEP_NUMBER_LOOP_END_FVAL                               0x2
#define HWIO_FLASH_XFR_STEP5_DATA_SEQ_STEP_NUMBER_LAST_STEP_FVAL                              0x3
#define HWIO_FLASH_XFR_STEP5_DATA_STEP1_WAIT_BMSK                                          0x3c00
#define HWIO_FLASH_XFR_STEP5_DATA_STEP1_WAIT_SHFT                                             0xa
#define HWIO_FLASH_XFR_STEP5_DATA_AOUT_EN_BMSK                                              0x200
#define HWIO_FLASH_XFR_STEP5_DATA_AOUT_EN_SHFT                                                0x9
#define HWIO_FLASH_XFR_STEP5_DATA_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                         0x0
#define HWIO_FLASH_XFR_STEP5_DATA_AOUT_EN_DRIVE_ADDRESS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP5_DATA_DATA_EN_BMSK                                              0x100
#define HWIO_FLASH_XFR_STEP5_DATA_DATA_EN_SHFT                                                0x8
#define HWIO_FLASH_XFR_STEP5_DATA_DATA_EN_DISABLE_DATA_BUS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP5_DATA_DATA_EN_DRIVE_DATA_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP5_DATA_CE_EN_BMSK                                                 0x80
#define HWIO_FLASH_XFR_STEP5_DATA_CE_EN_SHFT                                                  0x7
#define HWIO_FLASH_XFR_STEP5_DATA_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                           0x0
#define HWIO_FLASH_XFR_STEP5_DATA_CE_EN_ASSERT_CHIP_SELECT_FVAL                               0x1
#define HWIO_FLASH_XFR_STEP5_DATA_CLE_EN_BMSK                                                0x40
#define HWIO_FLASH_XFR_STEP5_DATA_CLE_EN_SHFT                                                 0x6
#define HWIO_FLASH_XFR_STEP5_DATA_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP5_DATA_CLE_EN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP5_DATA_ALE_PIN_BMSK                                               0x20
#define HWIO_FLASH_XFR_STEP5_DATA_ALE_PIN_SHFT                                                0x5
#define HWIO_FLASH_XFR_STEP5_DATA_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP5_DATA_ALE_PIN_LOGIC_ASSERTS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP5_DATA_WE_EN_BMSK                                                 0x10
#define HWIO_FLASH_XFR_STEP5_DATA_WE_EN_SHFT                                                  0x4
#define HWIO_FLASH_XFR_STEP5_DATA_WE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP5_DATA_WE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP5_DATA_RE_EN_BMSK                                                  0x8
#define HWIO_FLASH_XFR_STEP5_DATA_RE_EN_SHFT                                                  0x3
#define HWIO_FLASH_XFR_STEP5_DATA_RE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP5_DATA_RE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP5_DATA_WIDE_BMSK                                                   0x4
#define HWIO_FLASH_XFR_STEP5_DATA_WIDE_SHFT                                                   0x2
#define HWIO_FLASH_XFR_STEP5_DATA_WIDE_USE_8_BIT_DATA_BUS_FOR_DATA_FVAL                       0x0
#define HWIO_FLASH_XFR_STEP5_DATA_WIDE_USE16_BIT_DATA_BUS_FOR_DATA_FVAL                       0x1
#define HWIO_FLASH_XFR_STEP5_EXTA_READ_WAIT_BMSK                                              0x3
#define HWIO_FLASH_XFR_STEP5_EXTA_READ_WAIT_SHFT                                              0x0

#define HWIO_FLASH_XFR_STEP6_ADDR                                                 (EBI2ND_REG_BASE      + 0x00000084)
#define HWIO_FLASH_XFR_STEP6_PHYS                                                 (EBI2ND_REG_BASE_PHYS + 0x00000084)
#define HWIO_FLASH_XFR_STEP6_OFFS                                                 (EBI2ND_REG_BASE_OFFS + 0x00000084)
#define HWIO_FLASH_XFR_STEP6_RMSK                                                 0xfffcffff
#define HWIO_FLASH_XFR_STEP6_IN          \
        in_dword_masked(HWIO_FLASH_XFR_STEP6_ADDR, HWIO_FLASH_XFR_STEP6_RMSK)
#define HWIO_FLASH_XFR_STEP6_INM(m)      \
        in_dword_masked(HWIO_FLASH_XFR_STEP6_ADDR, m)
#define HWIO_FLASH_XFR_STEP6_OUT(v)      \
        out_dword(HWIO_FLASH_XFR_STEP6_ADDR,v)
#define HWIO_FLASH_XFR_STEP6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_XFR_STEP6_ADDR,m,v,HWIO_FLASH_XFR_STEP6_IN)
#define HWIO_FLASH_XFR_STEP6_CMD_SEQ_STEP_NUMBER_BMSK                                  0xc0000000
#define HWIO_FLASH_XFR_STEP6_CMD_SEQ_STEP_NUMBER_SHFT                                        0x1e
#define HWIO_FLASH_XFR_STEP6_CMD_SEQ_STEP_NUMBER_LOOP_START_FVAL                              0x1
#define HWIO_FLASH_XFR_STEP6_CMD_SEQ_STEP_NUMBER_LOOP_END_FVAL                                0x2
#define HWIO_FLASH_XFR_STEP6_CMD_SEQ_STEP_NUMBER_LAST_STEP_FVAL                               0x3
#define HWIO_FLASH_XFR_STEP6_CMD_STEP1_WAIT_BMSK                                       0x3c000000
#define HWIO_FLASH_XFR_STEP6_CMD_STEP1_WAIT_SHFT                                             0x1a
#define HWIO_FLASH_XFR_STEP6_CMD_AOUT_EN_BMSK                                           0x2000000
#define HWIO_FLASH_XFR_STEP6_CMD_AOUT_EN_SHFT                                                0x19
#define HWIO_FLASH_XFR_STEP6_CMD_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                          0x0
#define HWIO_FLASH_XFR_STEP6_CMD_AOUT_EN_DRIVE_ADDRESS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP6_CMD_DATA_EN_BMSK                                           0x1000000
#define HWIO_FLASH_XFR_STEP6_CMD_DATA_EN_SHFT                                                0x18
#define HWIO_FLASH_XFR_STEP6_CMD_DATA_EN_DISABLE_DATA_BUS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP6_CMD_DATA_EN_DRIVE_DATA_FVAL                                      0x1
#define HWIO_FLASH_XFR_STEP6_CMD_CE_EN_BMSK                                              0x800000
#define HWIO_FLASH_XFR_STEP6_CMD_CE_EN_SHFT                                                  0x17
#define HWIO_FLASH_XFR_STEP6_CMD_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP6_CMD_CE_EN_ASSERT_CHIP_SELECT_FVAL                                0x1
#define HWIO_FLASH_XFR_STEP6_CMD_CLE_EN_BMSK                                             0x400000
#define HWIO_FLASH_XFR_STEP6_CMD_CLE_EN_SHFT                                                 0x16
#define HWIO_FLASH_XFR_STEP6_CMD_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP6_CMD_CLE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP6_CMD_ALE_PIN_BMSK                                            0x200000
#define HWIO_FLASH_XFR_STEP6_CMD_ALE_PIN_SHFT                                                0x15
#define HWIO_FLASH_XFR_STEP6_CMD_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP6_CMD_ALE_PIN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP6_CMD_WE_EN_BMSK                                              0x100000
#define HWIO_FLASH_XFR_STEP6_CMD_WE_EN_SHFT                                                  0x14
#define HWIO_FLASH_XFR_STEP6_CMD_WE_EN_LOGIC_DE_ASSERTS_FVAL                                  0x0
#define HWIO_FLASH_XFR_STEP6_CMD_WE_EN_LOGIC_ASSERTS_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP6_CMD_RE_EN_BMSK                                               0x80000
#define HWIO_FLASH_XFR_STEP6_CMD_RE_EN_SHFT                                                  0x13
#define HWIO_FLASH_XFR_STEP6_CMD_RE_EN_LOGIC_DE_ASSERTS_FVAL                                  0x0
#define HWIO_FLASH_XFR_STEP6_CMD_RE_EN_LOGIC_ASSERTS_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP6_CMD_WIDE_BMSK                                                0x40000
#define HWIO_FLASH_XFR_STEP6_CMD_WIDE_SHFT                                                   0x12
#define HWIO_FLASH_XFR_STEP6_CMD_WIDE_SEND_CMD_ON_8_BIT_BUS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP6_CMD_WIDE_SEND_CMD_ON_16_BIT_BUS_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP6_DATA_SEQ_STEP_NUMBER_BMSK                                     0xc000
#define HWIO_FLASH_XFR_STEP6_DATA_SEQ_STEP_NUMBER_SHFT                                        0xe
#define HWIO_FLASH_XFR_STEP6_DATA_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP6_DATA_SEQ_STEP_NUMBER_LOOP_START_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP6_DATA_SEQ_STEP_NUMBER_LOOP_END_FVAL                               0x2
#define HWIO_FLASH_XFR_STEP6_DATA_SEQ_STEP_NUMBER_LAST_STEP_FVAL                              0x3
#define HWIO_FLASH_XFR_STEP6_DATA_STEP1_WAIT_BMSK                                          0x3c00
#define HWIO_FLASH_XFR_STEP6_DATA_STEP1_WAIT_SHFT                                             0xa
#define HWIO_FLASH_XFR_STEP6_DATA_AOUT_EN_BMSK                                              0x200
#define HWIO_FLASH_XFR_STEP6_DATA_AOUT_EN_SHFT                                                0x9
#define HWIO_FLASH_XFR_STEP6_DATA_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                         0x0
#define HWIO_FLASH_XFR_STEP6_DATA_AOUT_EN_DRIVE_ADDRESS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP6_DATA_DATA_EN_BMSK                                              0x100
#define HWIO_FLASH_XFR_STEP6_DATA_DATA_EN_SHFT                                                0x8
#define HWIO_FLASH_XFR_STEP6_DATA_DATA_EN_DISABLE_DATA_BUS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP6_DATA_DATA_EN_DRIVE_DATA_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP6_DATA_CE_EN_BMSK                                                 0x80
#define HWIO_FLASH_XFR_STEP6_DATA_CE_EN_SHFT                                                  0x7
#define HWIO_FLASH_XFR_STEP6_DATA_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                           0x0
#define HWIO_FLASH_XFR_STEP6_DATA_CE_EN_ASSERT_CHIP_SELECT_FVAL                               0x1
#define HWIO_FLASH_XFR_STEP6_DATA_CLE_EN_BMSK                                                0x40
#define HWIO_FLASH_XFR_STEP6_DATA_CLE_EN_SHFT                                                 0x6
#define HWIO_FLASH_XFR_STEP6_DATA_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP6_DATA_CLE_EN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP6_DATA_ALE_PIN_BMSK                                               0x20
#define HWIO_FLASH_XFR_STEP6_DATA_ALE_PIN_SHFT                                                0x5
#define HWIO_FLASH_XFR_STEP6_DATA_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP6_DATA_ALE_PIN_LOGIC_ASSERTS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP6_DATA_WE_EN_BMSK                                                 0x10
#define HWIO_FLASH_XFR_STEP6_DATA_WE_EN_SHFT                                                  0x4
#define HWIO_FLASH_XFR_STEP6_DATA_WE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP6_DATA_WE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP6_DATA_RE_EN_BMSK                                                  0x8
#define HWIO_FLASH_XFR_STEP6_DATA_RE_EN_SHFT                                                  0x3
#define HWIO_FLASH_XFR_STEP6_DATA_RE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP6_DATA_RE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP6_DATA_WIDE_BMSK                                                   0x4
#define HWIO_FLASH_XFR_STEP6_DATA_WIDE_SHFT                                                   0x2
#define HWIO_FLASH_XFR_STEP6_DATA_WIDE_USE_8_BIT_DATA_BUS_FOR_DATA_FVAL                       0x0
#define HWIO_FLASH_XFR_STEP6_DATA_WIDE_USE16_BIT_DATA_BUS_FOR_DATA_FVAL                       0x1
#define HWIO_FLASH_XFR_STEP6_EXTA_READ_WAIT_BMSK                                              0x3
#define HWIO_FLASH_XFR_STEP6_EXTA_READ_WAIT_SHFT                                              0x0

#define HWIO_FLASH_XFR_STEP7_ADDR                                                 (EBI2ND_REG_BASE      + 0x00000088)
#define HWIO_FLASH_XFR_STEP7_PHYS                                                 (EBI2ND_REG_BASE_PHYS + 0x00000088)
#define HWIO_FLASH_XFR_STEP7_OFFS                                                 (EBI2ND_REG_BASE_OFFS + 0x00000088)
#define HWIO_FLASH_XFR_STEP7_RMSK                                                 0xfffcffff
#define HWIO_FLASH_XFR_STEP7_IN          \
        in_dword_masked(HWIO_FLASH_XFR_STEP7_ADDR, HWIO_FLASH_XFR_STEP7_RMSK)
#define HWIO_FLASH_XFR_STEP7_INM(m)      \
        in_dword_masked(HWIO_FLASH_XFR_STEP7_ADDR, m)
#define HWIO_FLASH_XFR_STEP7_OUT(v)      \
        out_dword(HWIO_FLASH_XFR_STEP7_ADDR,v)
#define HWIO_FLASH_XFR_STEP7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_XFR_STEP7_ADDR,m,v,HWIO_FLASH_XFR_STEP7_IN)
#define HWIO_FLASH_XFR_STEP7_CMD_SEQ_STEP_NUMBER_BMSK                                  0xc0000000
#define HWIO_FLASH_XFR_STEP7_CMD_SEQ_STEP_NUMBER_SHFT                                        0x1e
#define HWIO_FLASH_XFR_STEP7_CMD_SEQ_STEP_NUMBER_SIMPLE_STEP_FVAL                             0x0
#define HWIO_FLASH_XFR_STEP7_CMD_SEQ_STEP_NUMBER_LOOP_START_FVAL                              0x1
#define HWIO_FLASH_XFR_STEP7_CMD_SEQ_STEP_NUMBER_LOOP_END_FVAL                                0x2
#define HWIO_FLASH_XFR_STEP7_CMD_SEQ_STEP_NUMBER_LAST_STEP_FVAL                               0x3
#define HWIO_FLASH_XFR_STEP7_CMD_STEP1_WAIT_BMSK                                       0x3c000000
#define HWIO_FLASH_XFR_STEP7_CMD_STEP1_WAIT_SHFT                                             0x1a
#define HWIO_FLASH_XFR_STEP7_CMD_AOUT_EN_BMSK                                           0x2000000
#define HWIO_FLASH_XFR_STEP7_CMD_AOUT_EN_SHFT                                                0x19
#define HWIO_FLASH_XFR_STEP7_CMD_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                          0x0
#define HWIO_FLASH_XFR_STEP7_CMD_AOUT_EN_DRIVE_ADDRESS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP7_CMD_DATA_EN_BMSK                                           0x1000000
#define HWIO_FLASH_XFR_STEP7_CMD_DATA_EN_SHFT                                                0x18
#define HWIO_FLASH_XFR_STEP7_CMD_DATA_EN_DISABLE_DATA_BUS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP7_CMD_DATA_EN_DRIVE_DATA_FVAL                                      0x1
#define HWIO_FLASH_XFR_STEP7_CMD_CE_EN_BMSK                                              0x800000
#define HWIO_FLASH_XFR_STEP7_CMD_CE_EN_SHFT                                                  0x17
#define HWIO_FLASH_XFR_STEP7_CMD_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP7_CMD_CE_EN_ASSERT_CHIP_SELECT_FVAL                                0x1
#define HWIO_FLASH_XFR_STEP7_CMD_CLE_EN_BMSK                                             0x400000
#define HWIO_FLASH_XFR_STEP7_CMD_CLE_EN_SHFT                                                 0x16
#define HWIO_FLASH_XFR_STEP7_CMD_CLE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP7_CMD_CLE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP7_CMD_ALE_PIN_BMSK                                            0x200000
#define HWIO_FLASH_XFR_STEP7_CMD_ALE_PIN_SHFT                                                0x15
#define HWIO_FLASH_XFR_STEP7_CMD_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                                0x0
#define HWIO_FLASH_XFR_STEP7_CMD_ALE_PIN_LOGIC_ASSERTS_FVAL                                   0x1
#define HWIO_FLASH_XFR_STEP7_CMD_WE_EN_BMSK                                              0x100000
#define HWIO_FLASH_XFR_STEP7_CMD_WE_EN_SHFT                                                  0x14
#define HWIO_FLASH_XFR_STEP7_CMD_WE_EN_LOGIC_DE_ASSERTS_FVAL                                  0x0
#define HWIO_FLASH_XFR_STEP7_CMD_WE_EN_LOGIC_ASSERTS_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP7_CMD_RE_EN_BMSK                                               0x80000
#define HWIO_FLASH_XFR_STEP7_CMD_RE_EN_SHFT                                                  0x13
#define HWIO_FLASH_XFR_STEP7_CMD_RE_EN_LOGIC_DE_ASSERTS_FVAL                                  0x0
#define HWIO_FLASH_XFR_STEP7_CMD_RE_EN_LOGIC_ASSERTS_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP7_CMD_WIDE_BMSK                                                0x40000
#define HWIO_FLASH_XFR_STEP7_CMD_WIDE_SHFT                                                   0x12
#define HWIO_FLASH_XFR_STEP7_CMD_WIDE_SEND_CMD_ON_8_BIT_BUS_FVAL                              0x0
#define HWIO_FLASH_XFR_STEP7_CMD_WIDE_SEND_CMD_ON_16_BIT_BUS_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP7_DATA_SEQ_STEP_NUMBER_BMSK                                     0xc000
#define HWIO_FLASH_XFR_STEP7_DATA_SEQ_STEP_NUMBER_SHFT                                        0xe
#define HWIO_FLASH_XFR_STEP7_DATA_SEQ_STEP_NUMBER_LOOP_START_FVAL                             0x1
#define HWIO_FLASH_XFR_STEP7_DATA_SEQ_STEP_NUMBER_LOOP_END_FVAL                               0x2
#define HWIO_FLASH_XFR_STEP7_DATA_SEQ_STEP_NUMBER_LAST_STEP_FVAL                              0x3
#define HWIO_FLASH_XFR_STEP7_DATA_STEP1_WAIT_BMSK                                          0x3c00
#define HWIO_FLASH_XFR_STEP7_DATA_STEP1_WAIT_SHFT                                             0xa
#define HWIO_FLASH_XFR_STEP7_DATA_AOUT_EN_BMSK                                              0x200
#define HWIO_FLASH_XFR_STEP7_DATA_AOUT_EN_SHFT                                                0x9
#define HWIO_FLASH_XFR_STEP7_DATA_AOUT_EN_ADDRESS_LOGIC_DISABLED_FVAL                         0x0
#define HWIO_FLASH_XFR_STEP7_DATA_AOUT_EN_DRIVE_ADDRESS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP7_DATA_DATA_EN_BMSK                                              0x100
#define HWIO_FLASH_XFR_STEP7_DATA_DATA_EN_SHFT                                                0x8
#define HWIO_FLASH_XFR_STEP7_DATA_DATA_EN_DISABLE_DATA_BUS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP7_DATA_DATA_EN_DRIVE_DATA_FVAL                                     0x1
#define HWIO_FLASH_XFR_STEP7_DATA_CE_EN_BMSK                                                 0x80
#define HWIO_FLASH_XFR_STEP7_DATA_CE_EN_SHFT                                                  0x7
#define HWIO_FLASH_XFR_STEP7_DATA_CE_EN_DE_ASSERT_CHIP_SELECTS_FVAL                           0x0
#define HWIO_FLASH_XFR_STEP7_DATA_CE_EN_ASSERT_CHIP_SELECT_FVAL                               0x1
#define HWIO_FLASH_XFR_STEP7_DATA_CLE_EN_BMSK                                                0x40
#define HWIO_FLASH_XFR_STEP7_DATA_CLE_EN_SHFT                                                 0x6
#define HWIO_FLASH_XFR_STEP7_DATA_CLE_EN_THE_LOGIC_DE_ASSERTS_FVAL                            0x0
#define HWIO_FLASH_XFR_STEP7_DATA_CLE_EN_THE_LOGIC_ASSERTS_FVAL                               0x1
#define HWIO_FLASH_XFR_STEP7_DATA_ALE_PIN_BMSK                                               0x20
#define HWIO_FLASH_XFR_STEP7_DATA_ALE_PIN_SHFT                                                0x5
#define HWIO_FLASH_XFR_STEP7_DATA_ALE_PIN_LOGIC_DE_ASSERTS_FVAL                               0x0
#define HWIO_FLASH_XFR_STEP7_DATA_ALE_PIN_LOGIC_ASSERTS_FVAL                                  0x1
#define HWIO_FLASH_XFR_STEP7_DATA_WE_EN_BMSK                                                 0x10
#define HWIO_FLASH_XFR_STEP7_DATA_WE_EN_SHFT                                                  0x4
#define HWIO_FLASH_XFR_STEP7_DATA_WE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP7_DATA_WE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP7_DATA_RE_EN_BMSK                                                  0x8
#define HWIO_FLASH_XFR_STEP7_DATA_RE_EN_SHFT                                                  0x3
#define HWIO_FLASH_XFR_STEP7_DATA_RE_EN_LOGIC_DE_ASSERTS_FVAL                                 0x0
#define HWIO_FLASH_XFR_STEP7_DATA_RE_EN_LOGIC_ASSERTS_FVAL                                    0x1
#define HWIO_FLASH_XFR_STEP7_DATA_WIDE_BMSK                                                   0x4
#define HWIO_FLASH_XFR_STEP7_DATA_WIDE_SHFT                                                   0x2
#define HWIO_FLASH_XFR_STEP7_DATA_WIDE_USE_8_BIT_DATA_BUS_FOR_DATA_FVAL                       0x0
#define HWIO_FLASH_XFR_STEP7_DATA_WIDE_USE16_BIT_DATA_BUS_FOR_DATA_FVAL                       0x1
#define HWIO_FLASH_XFR_STEP7_EXTA_READ_WAIT_BMSK                                              0x3
#define HWIO_FLASH_XFR_STEP7_EXTA_READ_WAIT_SHFT                                              0x0

#define HWIO_NAND_GENP_REG0_ADDR                                                  (EBI2ND_REG_BASE      + 0x00000090)
#define HWIO_NAND_GENP_REG0_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x00000090)
#define HWIO_NAND_GENP_REG0_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x00000090)
#define HWIO_NAND_GENP_REG0_RMSK                                                  0xffffffff
#define HWIO_NAND_GENP_REG0_IN          \
        in_dword_masked(HWIO_NAND_GENP_REG0_ADDR, HWIO_NAND_GENP_REG0_RMSK)
#define HWIO_NAND_GENP_REG0_INM(m)      \
        in_dword_masked(HWIO_NAND_GENP_REG0_ADDR, m)
#define HWIO_NAND_GENP_REG0_OUT(v)      \
        out_dword(HWIO_NAND_GENP_REG0_ADDR,v)
#define HWIO_NAND_GENP_REG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_GENP_REG0_ADDR,m,v,HWIO_NAND_GENP_REG0_IN)
#define HWIO_NAND_GENP_REG0_NAND_GENP_REG0_BMSK                                        0xffffffff
#define HWIO_NAND_GENP_REG0_NAND_GENP_REG0_SHFT                                               0x0

#define HWIO_NAND_GENP_REG1_ADDR                                                  (EBI2ND_REG_BASE      + 0x00000094)
#define HWIO_NAND_GENP_REG1_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x00000094)
#define HWIO_NAND_GENP_REG1_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x00000094)
#define HWIO_NAND_GENP_REG1_RMSK                                                  0xffffffff
#define HWIO_NAND_GENP_REG1_IN          \
        in_dword_masked(HWIO_NAND_GENP_REG1_ADDR, HWIO_NAND_GENP_REG1_RMSK)
#define HWIO_NAND_GENP_REG1_INM(m)      \
        in_dword_masked(HWIO_NAND_GENP_REG1_ADDR, m)
#define HWIO_NAND_GENP_REG1_OUT(v)      \
        out_dword(HWIO_NAND_GENP_REG1_ADDR,v)
#define HWIO_NAND_GENP_REG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_GENP_REG1_ADDR,m,v,HWIO_NAND_GENP_REG1_IN)
#define HWIO_NAND_GENP_REG1_NAND_GENP_REG1_BMSK                                        0xffffffff
#define HWIO_NAND_GENP_REG1_NAND_GENP_REG1_SHFT                                               0x0

#define HWIO_NAND_GENP_REG2_ADDR                                                  (EBI2ND_REG_BASE      + 0x00000098)
#define HWIO_NAND_GENP_REG2_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x00000098)
#define HWIO_NAND_GENP_REG2_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x00000098)
#define HWIO_NAND_GENP_REG2_RMSK                                                  0xffffffff
#define HWIO_NAND_GENP_REG2_IN          \
        in_dword_masked(HWIO_NAND_GENP_REG2_ADDR, HWIO_NAND_GENP_REG2_RMSK)
#define HWIO_NAND_GENP_REG2_INM(m)      \
        in_dword_masked(HWIO_NAND_GENP_REG2_ADDR, m)
#define HWIO_NAND_GENP_REG2_OUT(v)      \
        out_dword(HWIO_NAND_GENP_REG2_ADDR,v)
#define HWIO_NAND_GENP_REG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_GENP_REG2_ADDR,m,v,HWIO_NAND_GENP_REG2_IN)
#define HWIO_NAND_GENP_REG2_NAND_GENP_REG2_BMSK                                        0xffffffff
#define HWIO_NAND_GENP_REG2_NAND_GENP_REG2_SHFT                                               0x0

#define HWIO_NAND_GENP_REG3_ADDR                                                  (EBI2ND_REG_BASE      + 0x0000009c)
#define HWIO_NAND_GENP_REG3_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x0000009c)
#define HWIO_NAND_GENP_REG3_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x0000009c)
#define HWIO_NAND_GENP_REG3_RMSK                                                  0xffffffff
#define HWIO_NAND_GENP_REG3_IN          \
        in_dword_masked(HWIO_NAND_GENP_REG3_ADDR, HWIO_NAND_GENP_REG3_RMSK)
#define HWIO_NAND_GENP_REG3_INM(m)      \
        in_dword_masked(HWIO_NAND_GENP_REG3_ADDR, m)
#define HWIO_NAND_GENP_REG3_OUT(v)      \
        out_dword(HWIO_NAND_GENP_REG3_ADDR,v)
#define HWIO_NAND_GENP_REG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_GENP_REG3_ADDR,m,v,HWIO_NAND_GENP_REG3_IN)
#define HWIO_NAND_GENP_REG3_NAND_GENP_REG3_BMSK                                        0xffffffff
#define HWIO_NAND_GENP_REG3_NAND_GENP_REG3_SHFT                                               0x0

#define HWIO_FLASH_DEV_CMD0_ADDR                                                  (EBI2ND_REG_BASE      + 0x000000a0)
#define HWIO_FLASH_DEV_CMD0_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x000000a0)
#define HWIO_FLASH_DEV_CMD0_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x000000a0)
#define HWIO_FLASH_DEV_CMD0_RMSK                                                  0xffffffff
#define HWIO_FLASH_DEV_CMD0_IN          \
        in_dword_masked(HWIO_FLASH_DEV_CMD0_ADDR, HWIO_FLASH_DEV_CMD0_RMSK)
#define HWIO_FLASH_DEV_CMD0_INM(m)      \
        in_dword_masked(HWIO_FLASH_DEV_CMD0_ADDR, m)
#define HWIO_FLASH_DEV_CMD0_OUT(v)      \
        out_dword(HWIO_FLASH_DEV_CMD0_ADDR,v)
#define HWIO_FLASH_DEV_CMD0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_DEV_CMD0_ADDR,m,v,HWIO_FLASH_DEV_CMD0_IN)
#define HWIO_FLASH_DEV_CMD0_WRITE_START_BMSK                                           0xff000000
#define HWIO_FLASH_DEV_CMD0_WRITE_START_SHFT                                                 0x18
#define HWIO_FLASH_DEV_CMD0_WRITE_ADDR_BMSK                                              0xff0000
#define HWIO_FLASH_DEV_CMD0_WRITE_ADDR_SHFT                                                  0x10
#define HWIO_FLASH_DEV_CMD0_ERASE_START_BMSK                                               0xff00
#define HWIO_FLASH_DEV_CMD0_ERASE_START_SHFT                                                  0x8
#define HWIO_FLASH_DEV_CMD0_ERASE_ADDR_BMSK                                                  0xff
#define HWIO_FLASH_DEV_CMD0_ERASE_ADDR_SHFT                                                   0x0

#define HWIO_FLASH_DEV_CMD1_ADDR                                                  (EBI2ND_REG_BASE      + 0x000000a4)
#define HWIO_FLASH_DEV_CMD1_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x000000a4)
#define HWIO_FLASH_DEV_CMD1_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x000000a4)
#define HWIO_FLASH_DEV_CMD1_RMSK                                                  0xffffffff
#define HWIO_FLASH_DEV_CMD1_IN          \
        in_dword_masked(HWIO_FLASH_DEV_CMD1_ADDR, HWIO_FLASH_DEV_CMD1_RMSK)
#define HWIO_FLASH_DEV_CMD1_INM(m)      \
        in_dword_masked(HWIO_FLASH_DEV_CMD1_ADDR, m)
#define HWIO_FLASH_DEV_CMD1_OUT(v)      \
        out_dword(HWIO_FLASH_DEV_CMD1_ADDR,v)
#define HWIO_FLASH_DEV_CMD1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_DEV_CMD1_ADDR,m,v,HWIO_FLASH_DEV_CMD1_IN)
#define HWIO_FLASH_DEV_CMD1_SEQ_READ_MODE_START_BMSK                                   0xff000000
#define HWIO_FLASH_DEV_CMD1_SEQ_READ_MODE_START_SHFT                                         0x18
#define HWIO_FLASH_DEV_CMD1_SEQ_READ_MODE_ADDR_BMSK                                      0xff0000
#define HWIO_FLASH_DEV_CMD1_SEQ_READ_MODE_ADDR_SHFT                                          0x10
#define HWIO_FLASH_DEV_CMD1_READ_START_BMSK                                                0xff00
#define HWIO_FLASH_DEV_CMD1_READ_START_SHFT                                                   0x8
#define HWIO_FLASH_DEV_CMD1_READ_ADDR_BMSK                                                   0xff
#define HWIO_FLASH_DEV_CMD1_READ_ADDR_SHFT                                                    0x0

#define HWIO_FLASH_DEV_CMD2_ADDR                                                  (EBI2ND_REG_BASE      + 0x000000a8)
#define HWIO_FLASH_DEV_CMD2_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x000000a8)
#define HWIO_FLASH_DEV_CMD2_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x000000a8)
#define HWIO_FLASH_DEV_CMD2_RMSK                                                  0xffffffff
#define HWIO_FLASH_DEV_CMD2_IN          \
        in_dword_masked(HWIO_FLASH_DEV_CMD2_ADDR, HWIO_FLASH_DEV_CMD2_RMSK)
#define HWIO_FLASH_DEV_CMD2_INM(m)      \
        in_dword_masked(HWIO_FLASH_DEV_CMD2_ADDR, m)
#define HWIO_FLASH_DEV_CMD2_OUT(v)      \
        out_dword(HWIO_FLASH_DEV_CMD2_ADDR,v)
#define HWIO_FLASH_DEV_CMD2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_DEV_CMD2_ADDR,m,v,HWIO_FLASH_DEV_CMD2_IN)
#define HWIO_FLASH_DEV_CMD2_READ_STOP_CMD_BMSK                                         0xff000000
#define HWIO_FLASH_DEV_CMD2_READ_STOP_CMD_SHFT                                               0x18
#define HWIO_FLASH_DEV_CMD2_RESET_CMD_BMSK                                               0xff0000
#define HWIO_FLASH_DEV_CMD2_RESET_CMD_SHFT                                                   0x10
#define HWIO_FLASH_DEV_CMD2_READ_STATUS_BMSK                                               0xff00
#define HWIO_FLASH_DEV_CMD2_READ_STATUS_SHFT                                                  0x8
#define HWIO_FLASH_DEV_CMD2_READ_ID_BMSK                                                     0xff
#define HWIO_FLASH_DEV_CMD2_READ_ID_SHFT                                                      0x0

#define HWIO_FLASH_DEV_CMD_VLD_ADDR                                               (EBI2ND_REG_BASE      + 0x000000ac)
#define HWIO_FLASH_DEV_CMD_VLD_PHYS                                               (EBI2ND_REG_BASE_PHYS + 0x000000ac)
#define HWIO_FLASH_DEV_CMD_VLD_OFFS                                               (EBI2ND_REG_BASE_OFFS + 0x000000ac)
#define HWIO_FLASH_DEV_CMD_VLD_RMSK                                               0xff00001f
#define HWIO_FLASH_DEV_CMD_VLD_IN          \
        in_dword_masked(HWIO_FLASH_DEV_CMD_VLD_ADDR, HWIO_FLASH_DEV_CMD_VLD_RMSK)
#define HWIO_FLASH_DEV_CMD_VLD_INM(m)      \
        in_dword_masked(HWIO_FLASH_DEV_CMD_VLD_ADDR, m)
#define HWIO_FLASH_DEV_CMD_VLD_OUT(v)      \
        out_dword(HWIO_FLASH_DEV_CMD_VLD_ADDR,v)
#define HWIO_FLASH_DEV_CMD_VLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_DEV_CMD_VLD_ADDR,m,v,HWIO_FLASH_DEV_CMD_VLD_IN)
#define HWIO_FLASH_DEV_CMD_VLD_READ_PARAMETER_PAGE_CODE_BMSK                           0xff000000
#define HWIO_FLASH_DEV_CMD_VLD_READ_PARAMETER_PAGE_CODE_SHFT                                 0x18
#define HWIO_FLASH_DEV_CMD_VLD_SEQ_READ_START_VLD_BMSK                                       0x10
#define HWIO_FLASH_DEV_CMD_VLD_SEQ_READ_START_VLD_SHFT                                        0x4
#define HWIO_FLASH_DEV_CMD_VLD_ERASE_START_VLD_BMSK                                           0x8
#define HWIO_FLASH_DEV_CMD_VLD_ERASE_START_VLD_SHFT                                           0x3
#define HWIO_FLASH_DEV_CMD_VLD_WRITE_START_VLD_BMSK                                           0x4
#define HWIO_FLASH_DEV_CMD_VLD_WRITE_START_VLD_SHFT                                           0x2
#define HWIO_FLASH_DEV_CMD_VLD_READ_STOP_VLD_BMSK                                             0x2
#define HWIO_FLASH_DEV_CMD_VLD_READ_STOP_VLD_SHFT                                             0x1
#define HWIO_FLASH_DEV_CMD_VLD_READ_START_VLD_BMSK                                            0x1
#define HWIO_FLASH_DEV_CMD_VLD_READ_START_VLD_SHFT                                            0x0

#define HWIO_NAND_ADDR2_ADDR                                                      (EBI2ND_REG_BASE      + 0x000000c0)
#define HWIO_NAND_ADDR2_PHYS                                                      (EBI2ND_REG_BASE_PHYS + 0x000000c0)
#define HWIO_NAND_ADDR2_OFFS                                                      (EBI2ND_REG_BASE_OFFS + 0x000000c0)
#define HWIO_NAND_ADDR2_RMSK                                                      0xffffffff
#define HWIO_NAND_ADDR2_IN          \
        in_dword_masked(HWIO_NAND_ADDR2_ADDR, HWIO_NAND_ADDR2_RMSK)
#define HWIO_NAND_ADDR2_INM(m)      \
        in_dword_masked(HWIO_NAND_ADDR2_ADDR, m)
#define HWIO_NAND_ADDR2_OUT(v)      \
        out_dword(HWIO_NAND_ADDR2_ADDR,v)
#define HWIO_NAND_ADDR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_ADDR2_ADDR,m,v,HWIO_NAND_ADDR2_IN)
#define HWIO_NAND_ADDR2_DEV_ADDR2_BMSK                                                 0xffffffff
#define HWIO_NAND_ADDR2_DEV_ADDR2_SHFT                                                        0x0

#define HWIO_NAND_ADDR3_ADDR                                                      (EBI2ND_REG_BASE      + 0x000000c4)
#define HWIO_NAND_ADDR3_PHYS                                                      (EBI2ND_REG_BASE_PHYS + 0x000000c4)
#define HWIO_NAND_ADDR3_OFFS                                                      (EBI2ND_REG_BASE_OFFS + 0x000000c4)
#define HWIO_NAND_ADDR3_RMSK                                                      0xffffffff
#define HWIO_NAND_ADDR3_IN          \
        in_dword_masked(HWIO_NAND_ADDR3_ADDR, HWIO_NAND_ADDR3_RMSK)
#define HWIO_NAND_ADDR3_INM(m)      \
        in_dword_masked(HWIO_NAND_ADDR3_ADDR, m)
#define HWIO_NAND_ADDR3_OUT(v)      \
        out_dword(HWIO_NAND_ADDR3_ADDR,v)
#define HWIO_NAND_ADDR3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_ADDR3_ADDR,m,v,HWIO_NAND_ADDR3_IN)
#define HWIO_NAND_ADDR3_DEV_ADDR3_BMSK                                                 0xffffffff
#define HWIO_NAND_ADDR3_DEV_ADDR3_SHFT                                                        0x0

#define HWIO_NAND_ADDR4_ADDR                                                      (EBI2ND_REG_BASE      + 0x000000c8)
#define HWIO_NAND_ADDR4_PHYS                                                      (EBI2ND_REG_BASE_PHYS + 0x000000c8)
#define HWIO_NAND_ADDR4_OFFS                                                      (EBI2ND_REG_BASE_OFFS + 0x000000c8)
#define HWIO_NAND_ADDR4_RMSK                                                      0xffffffff
#define HWIO_NAND_ADDR4_IN          \
        in_dword_masked(HWIO_NAND_ADDR4_ADDR, HWIO_NAND_ADDR4_RMSK)
#define HWIO_NAND_ADDR4_INM(m)      \
        in_dword_masked(HWIO_NAND_ADDR4_ADDR, m)
#define HWIO_NAND_ADDR4_OUT(v)      \
        out_dword(HWIO_NAND_ADDR4_ADDR,v)
#define HWIO_NAND_ADDR4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_ADDR4_ADDR,m,v,HWIO_NAND_ADDR4_IN)
#define HWIO_NAND_ADDR4_DEV_ADDR4_BMSK                                                 0xffffffff
#define HWIO_NAND_ADDR4_DEV_ADDR4_SHFT                                                        0x0

#define HWIO_NAND_ADDR5_ADDR                                                      (EBI2ND_REG_BASE      + 0x000000cc)
#define HWIO_NAND_ADDR5_PHYS                                                      (EBI2ND_REG_BASE_PHYS + 0x000000cc)
#define HWIO_NAND_ADDR5_OFFS                                                      (EBI2ND_REG_BASE_OFFS + 0x000000cc)
#define HWIO_NAND_ADDR5_RMSK                                                      0xffffffff
#define HWIO_NAND_ADDR5_IN          \
        in_dword_masked(HWIO_NAND_ADDR5_ADDR, HWIO_NAND_ADDR5_RMSK)
#define HWIO_NAND_ADDR5_INM(m)      \
        in_dword_masked(HWIO_NAND_ADDR5_ADDR, m)
#define HWIO_NAND_ADDR5_OUT(v)      \
        out_dword(HWIO_NAND_ADDR5_ADDR,v)
#define HWIO_NAND_ADDR5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_ADDR5_ADDR,m,v,HWIO_NAND_ADDR5_IN)
#define HWIO_NAND_ADDR5_DEV_ADDR5_BMSK                                                 0xffffffff
#define HWIO_NAND_ADDR5_DEV_ADDR5_SHFT                                                        0x0

#define HWIO_FLASH_DEV_CMD3_ADDR                                                  (EBI2ND_REG_BASE      + 0x000000d0)
#define HWIO_FLASH_DEV_CMD3_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x000000d0)
#define HWIO_FLASH_DEV_CMD3_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x000000d0)
#define HWIO_FLASH_DEV_CMD3_RMSK                                                  0xffff00ff
#define HWIO_FLASH_DEV_CMD3_IN          \
        in_dword_masked(HWIO_FLASH_DEV_CMD3_ADDR, HWIO_FLASH_DEV_CMD3_RMSK)
#define HWIO_FLASH_DEV_CMD3_INM(m)      \
        in_dword_masked(HWIO_FLASH_DEV_CMD3_ADDR, m)
#define HWIO_FLASH_DEV_CMD3_OUT(v)      \
        out_dword(HWIO_FLASH_DEV_CMD3_ADDR,v)
#define HWIO_FLASH_DEV_CMD3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_DEV_CMD3_ADDR,m,v,HWIO_FLASH_DEV_CMD3_IN)
#define HWIO_FLASH_DEV_CMD3_READ_CACHE_LAST_BMSK                                       0xff000000
#define HWIO_FLASH_DEV_CMD3_READ_CACHE_LAST_SHFT                                             0x18
#define HWIO_FLASH_DEV_CMD3_READ_CACHE_SEQ_BMSK                                          0xff0000
#define HWIO_FLASH_DEV_CMD3_READ_CACHE_SEQ_SHFT                                              0x10
#define HWIO_FLASH_DEV_CMD3_WRITE_START_CACHE_BMSK                                           0xff
#define HWIO_FLASH_DEV_CMD3_WRITE_START_CACHE_SHFT                                            0x0

#define HWIO_FLASH_DEV_CMD4_ADDR                                                  (EBI2ND_REG_BASE      + 0x000000d4)
#define HWIO_FLASH_DEV_CMD4_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x000000d4)
#define HWIO_FLASH_DEV_CMD4_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x000000d4)
#define HWIO_FLASH_DEV_CMD4_RMSK                                                  0xffffffff
#define HWIO_FLASH_DEV_CMD4_IN          \
        in_dword_masked(HWIO_FLASH_DEV_CMD4_ADDR, HWIO_FLASH_DEV_CMD4_RMSK)
#define HWIO_FLASH_DEV_CMD4_INM(m)      \
        in_dword_masked(HWIO_FLASH_DEV_CMD4_ADDR, m)
#define HWIO_FLASH_DEV_CMD4_OUT(v)      \
        out_dword(HWIO_FLASH_DEV_CMD4_ADDR,v)
#define HWIO_FLASH_DEV_CMD4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_DEV_CMD4_ADDR,m,v,HWIO_FLASH_DEV_CMD4_IN)
#define HWIO_FLASH_DEV_CMD4_GP_CMD4_BMSK                                               0xffff0000
#define HWIO_FLASH_DEV_CMD4_GP_CMD4_SHFT                                                     0x10
#define HWIO_FLASH_DEV_CMD4_GP_CMD3_BMSK                                                   0xffff
#define HWIO_FLASH_DEV_CMD4_GP_CMD3_SHFT                                                      0x0

#define HWIO_FLASH_DEV_CMD5_ADDR                                                  (EBI2ND_REG_BASE      + 0x000000d8)
#define HWIO_FLASH_DEV_CMD5_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x000000d8)
#define HWIO_FLASH_DEV_CMD5_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x000000d8)
#define HWIO_FLASH_DEV_CMD5_RMSK                                                  0xffffffff
#define HWIO_FLASH_DEV_CMD5_IN          \
        in_dword_masked(HWIO_FLASH_DEV_CMD5_ADDR, HWIO_FLASH_DEV_CMD5_RMSK)
#define HWIO_FLASH_DEV_CMD5_INM(m)      \
        in_dword_masked(HWIO_FLASH_DEV_CMD5_ADDR, m)
#define HWIO_FLASH_DEV_CMD5_OUT(v)      \
        out_dword(HWIO_FLASH_DEV_CMD5_ADDR,v)
#define HWIO_FLASH_DEV_CMD5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_DEV_CMD5_ADDR,m,v,HWIO_FLASH_DEV_CMD5_IN)
#define HWIO_FLASH_DEV_CMD5_GP_CMD6_BMSK                                               0xffff0000
#define HWIO_FLASH_DEV_CMD5_GP_CMD6_SHFT                                                     0x10
#define HWIO_FLASH_DEV_CMD5_GP_CMD5_BMSK                                                   0xffff
#define HWIO_FLASH_DEV_CMD5_GP_CMD5_SHFT                                                      0x0

#define HWIO_FLASH_DEV_CMD6_ADDR                                                  (EBI2ND_REG_BASE      + 0x000000dc)
#define HWIO_FLASH_DEV_CMD6_PHYS                                                  (EBI2ND_REG_BASE_PHYS + 0x000000dc)
#define HWIO_FLASH_DEV_CMD6_OFFS                                                  (EBI2ND_REG_BASE_OFFS + 0x000000dc)
#define HWIO_FLASH_DEV_CMD6_RMSK                                                  0xffffffff
#define HWIO_FLASH_DEV_CMD6_IN          \
        in_dword_masked(HWIO_FLASH_DEV_CMD6_ADDR, HWIO_FLASH_DEV_CMD6_RMSK)
#define HWIO_FLASH_DEV_CMD6_INM(m)      \
        in_dword_masked(HWIO_FLASH_DEV_CMD6_ADDR, m)
#define HWIO_FLASH_DEV_CMD6_OUT(v)      \
        out_dword(HWIO_FLASH_DEV_CMD6_ADDR,v)
#define HWIO_FLASH_DEV_CMD6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FLASH_DEV_CMD6_ADDR,m,v,HWIO_FLASH_DEV_CMD6_IN)
#define HWIO_FLASH_DEV_CMD6_GP_CMD8_BMSK                                               0xffff0000
#define HWIO_FLASH_DEV_CMD6_GP_CMD8_SHFT                                                     0x10
#define HWIO_FLASH_DEV_CMD6_GP_CMD7_BMSK                                                   0xffff
#define HWIO_FLASH_DEV_CMD6_GP_CMD7_SHFT                                                      0x0

#define HWIO_NAND_ADDR6_ADDR                                                      (EBI2ND_REG_BASE      + 0x000000e4)
#define HWIO_NAND_ADDR6_PHYS                                                      (EBI2ND_REG_BASE_PHYS + 0x000000e4)
#define HWIO_NAND_ADDR6_OFFS                                                      (EBI2ND_REG_BASE_OFFS + 0x000000e4)
#define HWIO_NAND_ADDR6_RMSK                                                      0xffffffff
#define HWIO_NAND_ADDR6_IN          \
        in_dword_masked(HWIO_NAND_ADDR6_ADDR, HWIO_NAND_ADDR6_RMSK)
#define HWIO_NAND_ADDR6_INM(m)      \
        in_dword_masked(HWIO_NAND_ADDR6_ADDR, m)
#define HWIO_NAND_ADDR6_OUT(v)      \
        out_dword(HWIO_NAND_ADDR6_ADDR,v)
#define HWIO_NAND_ADDR6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_ADDR6_ADDR,m,v,HWIO_NAND_ADDR6_IN)
#define HWIO_NAND_ADDR6_DEV_ADDR6_BMSK                                                 0xffffffff
#define HWIO_NAND_ADDR6_DEV_ADDR6_SHFT                                                        0x0

#define HWIO_NAND_ERASED_CW_DETECT_CFG_ADDR                                       (EBI2ND_REG_BASE      + 0x000000e8)
#define HWIO_NAND_ERASED_CW_DETECT_CFG_PHYS                                       (EBI2ND_REG_BASE_PHYS + 0x000000e8)
#define HWIO_NAND_ERASED_CW_DETECT_CFG_OFFS                                       (EBI2ND_REG_BASE_OFFS + 0x000000e8)
#define HWIO_NAND_ERASED_CW_DETECT_CFG_RMSK                                              0x3
#define HWIO_NAND_ERASED_CW_DETECT_CFG_IN          \
        in_dword_masked(HWIO_NAND_ERASED_CW_DETECT_CFG_ADDR, HWIO_NAND_ERASED_CW_DETECT_CFG_RMSK)
#define HWIO_NAND_ERASED_CW_DETECT_CFG_INM(m)      \
        in_dword_masked(HWIO_NAND_ERASED_CW_DETECT_CFG_ADDR, m)
#define HWIO_NAND_ERASED_CW_DETECT_CFG_OUT(v)      \
        out_dword(HWIO_NAND_ERASED_CW_DETECT_CFG_ADDR,v)
#define HWIO_NAND_ERASED_CW_DETECT_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_ERASED_CW_DETECT_CFG_ADDR,m,v,HWIO_NAND_ERASED_CW_DETECT_CFG_IN)
#define HWIO_NAND_ERASED_CW_DETECT_CFG_ERASED_CW_ECC_MASK_BMSK                                0x2
#define HWIO_NAND_ERASED_CW_DETECT_CFG_ERASED_CW_ECC_MASK_SHFT                                0x1
#define HWIO_NAND_ERASED_CW_DETECT_CFG_ERASED_CW_ECC_MASK_NOT_MASKED_FVAL                     0x0
#define HWIO_NAND_ERASED_CW_DETECT_CFG_ERASED_CW_ECC_MASK_MASKED_FVAL                         0x1
#define HWIO_NAND_ERASED_CW_DETECT_CFG_AUTO_DETECT_RES_BMSK                                   0x1
#define HWIO_NAND_ERASED_CW_DETECT_CFG_AUTO_DETECT_RES_SHFT                                   0x0
#define HWIO_NAND_ERASED_CW_DETECT_CFG_AUTO_DETECT_RES_ACTIVE_FVAL                            0x0
#define HWIO_NAND_ERASED_CW_DETECT_CFG_AUTO_DETECT_RES_RESET_FVAL                             0x1

#define HWIO_NAND_ERASED_CW_DETECT_STATUS_ADDR                                    (EBI2ND_REG_BASE      + 0x000000ec)
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_PHYS                                    (EBI2ND_REG_BASE_PHYS + 0x000000ec)
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_OFFS                                    (EBI2ND_REG_BASE_OFFS + 0x000000ec)
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_RMSK                                          0xf3
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_IN          \
        in_dword_masked(HWIO_NAND_ERASED_CW_DETECT_STATUS_ADDR, HWIO_NAND_ERASED_CW_DETECT_STATUS_RMSK)
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_INM(m)      \
        in_dword_masked(HWIO_NAND_ERASED_CW_DETECT_STATUS_ADDR, m)
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_PAGE_ALL_ERASED_BMSK                               0x80
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_PAGE_ALL_ERASED_SHFT                                0x7
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_CODEWORD_ALL_ERASED_BMSK                           0x40
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_CODEWORD_ALL_ERASED_SHFT                            0x6
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_PAGE_ERASED_BMSK                                   0x20
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_PAGE_ERASED_SHFT                                    0x5
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_CODEWORD_ERASED_BMSK                               0x10
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_CODEWORD_ERASED_SHFT                                0x4
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_ERASED_CW_ECC_MASK_BMSK                             0x2
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_ERASED_CW_ECC_MASK_SHFT                             0x1
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_AUTO_DETECT_RES_BMSK                                0x1
#define HWIO_NAND_ERASED_CW_DETECT_STATUS_AUTO_DETECT_RES_SHFT                                0x0

#define HWIO_EBI2_ECC_BUF_CFG_ADDR                                                (EBI2ND_REG_BASE      + 0x000000f0)
#define HWIO_EBI2_ECC_BUF_CFG_PHYS                                                (EBI2ND_REG_BASE_PHYS + 0x000000f0)
#define HWIO_EBI2_ECC_BUF_CFG_OFFS                                                (EBI2ND_REG_BASE_OFFS + 0x000000f0)
#define HWIO_EBI2_ECC_BUF_CFG_RMSK                                                     0x3ff
#define HWIO_EBI2_ECC_BUF_CFG_IN          \
        in_dword_masked(HWIO_EBI2_ECC_BUF_CFG_ADDR, HWIO_EBI2_ECC_BUF_CFG_RMSK)
#define HWIO_EBI2_ECC_BUF_CFG_INM(m)      \
        in_dword_masked(HWIO_EBI2_ECC_BUF_CFG_ADDR, m)
#define HWIO_EBI2_ECC_BUF_CFG_OUT(v)      \
        out_dword(HWIO_EBI2_ECC_BUF_CFG_ADDR,v)
#define HWIO_EBI2_ECC_BUF_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_EBI2_ECC_BUF_CFG_ADDR,m,v,HWIO_EBI2_ECC_BUF_CFG_IN)
#define HWIO_EBI2_ECC_BUF_CFG_NUM_STEPS_BMSK                                                0x3ff
#define HWIO_EBI2_ECC_BUF_CFG_NUM_STEPS_SHFT                                                  0x0

#define HWIO_NANDC_DBG_CFG_ADDR                                                   (EBI2ND_REG_BASE      + 0x000000f4)
#define HWIO_NANDC_DBG_CFG_PHYS                                                   (EBI2ND_REG_BASE_PHYS + 0x000000f4)
#define HWIO_NANDC_DBG_CFG_OFFS                                                   (EBI2ND_REG_BASE_OFFS + 0x000000f4)
#define HWIO_NANDC_DBG_CFG_RMSK                                                      0xf07ff
#define HWIO_NANDC_DBG_CFG_IN          \
        in_dword_masked(HWIO_NANDC_DBG_CFG_ADDR, HWIO_NANDC_DBG_CFG_RMSK)
#define HWIO_NANDC_DBG_CFG_INM(m)      \
        in_dword_masked(HWIO_NANDC_DBG_CFG_ADDR, m)
#define HWIO_NANDC_DBG_CFG_OUT(v)      \
        out_dword(HWIO_NANDC_DBG_CFG_ADDR,v)
#define HWIO_NANDC_DBG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NANDC_DBG_CFG_ADDR,m,v,HWIO_NANDC_DBG_CFG_IN)
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_EXT_BMSK                                      0xf0000
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_EXT_SHFT                                         0x10
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_EXT_NANDC_TX0_LOW_FVAL                            0x0
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_EXT_NANDC_TX0_HIGH_FVAL                           0x1
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_EXT_NANDC_TX1_LOW_FVAL                            0x2
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_EXT_NANDC_TX1_HIGH_FVAL                           0x3
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_EXT_NANDC_RX_LOW_FVAL                             0x4
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_EXT_NANDC_RX_HIGH_FVAL                            0x5
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_EXT_NANDC_RAMB_LOW_FVAL                           0x6
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_EXT_NANDC_RAMB_HIGH_FVAL                          0x7
#define HWIO_NANDC_DBG_CFG_BCH_ECC_DBG_BUS_SEL_BMSK                                         0x700
#define HWIO_NANDC_DBG_CFG_BCH_ECC_DBG_BUS_SEL_SHFT                                           0x8
#define HWIO_NANDC_DBG_CFG_NAND_CTRL_DBG_BUS_D31_D16_EN_BMSK                                 0x80
#define HWIO_NANDC_DBG_CFG_NAND_CTRL_DBG_BUS_D31_D16_EN_SHFT                                  0x7
#define HWIO_NANDC_DBG_CFG_NAND_CTRL_DBG_BUS_D15_D0_EN_BMSK                                  0x40
#define HWIO_NANDC_DBG_CFG_NAND_CTRL_DBG_BUS_D15_D0_EN_SHFT                                   0x6
#define HWIO_NANDC_DBG_CFG_NAND_CTRL_DEBUG_SWAP_BMSK                                         0x20
#define HWIO_NANDC_DBG_CFG_NAND_CTRL_DEBUG_SWAP_SHFT                                          0x5
#define HWIO_NANDC_DBG_CFG_NAND_CTRL_DEBUG_SWAP_SWAP_DISABLED_FVAL                            0x0
#define HWIO_NANDC_DBG_CFG_NAND_CTRL_DEBUG_SWAP_SWAP_ENABLED_FVAL                             0x1
#define HWIO_NANDC_DBG_CFG_UPPER_WORD_SELECT_BMSK                                            0x10
#define HWIO_NANDC_DBG_CFG_UPPER_WORD_SELECT_SHFT                                             0x4
#define HWIO_NANDC_DBG_CFG_UPPER_WORD_SELECT_LOWER_WORD_IS_SELECTED_31_0_FVAL                 0x0
#define HWIO_NANDC_DBG_CFG_UPPER_WORD_SELECT_UPPER_WORD_IS_SELECTED_63_32_FVAL                0x1
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_BMSK                                              0xf
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_SHFT                                              0x0
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_IF0_FVAL                                    0x0
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_IF1_FVAL                                    0x1
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_IF2_FVAL                                    0x2
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_IF3_FVAL                                    0x3
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_FSM0_FVAL                                   0x4
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_FSM1_FVAL                                   0x5
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_SMD_DEC_FVAL                                0x6
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_DM_FVAL                                     0x7
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_SFLASHC_IF0_FVAL                                  0x8
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_SFLASHC_IF1_FVAL                                  0x9
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_HSDDR_DATAPATH_FVAL                         0xa
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_BCH_ECC_FVAL                                      0xb
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_AHBIF_FVAL                                  0xc
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_AHB_DEC_FVAL                                0xd
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_NANDC_REG_IF_FVAL                                 0xe
#define HWIO_NANDC_DBG_CFG_DEBUG_BUS_SELECT_DEBUG_BUS_EXTENSION_FVAL                          0xf

#define HWIO_NANDC_HW_PROFILE_CFG_ADDR                                            (EBI2ND_REG_BASE      + 0x000000f8)
#define HWIO_NANDC_HW_PROFILE_CFG_PHYS                                            (EBI2ND_REG_BASE_PHYS + 0x000000f8)
#define HWIO_NANDC_HW_PROFILE_CFG_OFFS                                            (EBI2ND_REG_BASE_OFFS + 0x000000f8)
#define HWIO_NANDC_HW_PROFILE_CFG_RMSK                                                0xffff
#define HWIO_NANDC_HW_PROFILE_CFG_IN          \
        in_dword_masked(HWIO_NANDC_HW_PROFILE_CFG_ADDR, HWIO_NANDC_HW_PROFILE_CFG_RMSK)
#define HWIO_NANDC_HW_PROFILE_CFG_INM(m)      \
        in_dword_masked(HWIO_NANDC_HW_PROFILE_CFG_ADDR, m)
#define HWIO_NANDC_HW_PROFILE_CFG_OUT(v)      \
        out_dword(HWIO_NANDC_HW_PROFILE_CFG_ADDR,v)
#define HWIO_NANDC_HW_PROFILE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NANDC_HW_PROFILE_CFG_ADDR,m,v,HWIO_NANDC_HW_PROFILE_CFG_IN)
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_BMSK                                0xf000
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_SHFT                                   0xc
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_TIME_NAND_FSM_IS_ACTIVE_FVAL        0x1
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_TIME_NAND_HW_IS_ACTIVE_FOR_A_READ_FVAL        0x2
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_TIME_NAND_HW_IS_ACTIVE_FOR_A_WRITE_FVAL        0x3
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_NUMBER_OF_READ_CODEWORDS_FVAL        0x4
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_NUMBER_OF_WRITE_CODEWORDS_FVAL        0x5
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_NUMBER_OF_ERASE_CODEWORDS_FVAL        0x6
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_READ_BUSY_TIME_FVAL                    0x7
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_WRITE_BUSY_TIME_FVAL                   0x8
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_BUSY_TIME_FVAL                   0x9
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_ECC_TRANSFER_TIME_FVAL           0xa
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_ECC_TRASFER_TIME_FOR_A_READ_FVAL        0xb
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_ECC_TRANSFER_TIME_FOR_A_WRITE_FVAL        0xc
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_TOTAL_TIME_SPENT_IN_ACCESSING_THE_AHB_IF_BUT_NOT_THE_NAND_HARDWARE_FVAL        0xd
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_NOTING_IS_PROFILED_FVAL                0xe
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_4N_NOTHING_IS_PROFILED_FVAL               0xf
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_BMSK                                 0xf00
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_SHFT                                   0x8
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_DEFAULT_FVAL                           0x0
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_FSM_ACT_TIME_FVAL                      0x1
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_HW_RD_ACT_TIME_FVAL                    0x2
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_HW_WR_ACT_TIME_TOTAL_TIME_NAND_HW_IS_ACTIVE_FOR_A_WRITE_FVAL        0x3
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_NUM_OF_RD_FVAL                         0x4
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_NUM_OF_WR_FVAL                         0x5
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_NUM_OF_ERASE_FVAL                      0x6
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_RD_BUSY_TIME_FVAL                      0x7
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_WR_BUSY_TIME_FVAL                      0x8
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_TOT_BUSY_TIME_FVAL                     0x9
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_TOT_ECC_TX_TIME_FVAL                   0xa
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_TOT_ECC_TX_RD_TIME_FVAL                0xb
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_TOT_ECC_TX_WR_TIME_FVAL                0xc
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_TOT_AHB_ACC_TIME_FVAL                  0xd
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_NOTHING0_FVAL                          0xe
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_3N_NOTHING1_FVAL                          0xf
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_BMSK                                  0xf0
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_SHFT                                   0x4
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_DEFAULT_FVAL                           0x0
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_FSM_ACT_TIME_FVAL                      0x1
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_HW_RD_ACT_TIME_FVAL                    0x2
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_HW_WR_ACT_TIME_FVAL                    0x3
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_NUM_OF_RD_FVAL                         0x4
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_NUM_OF_WR_FVAL                         0x5
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_NUM_OF_ERASE_FVAL                      0x6
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_RD_BUSY_TIME_FVAL                      0x7
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_WR_BUSY_TIME_FVAL                      0x8
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_TOT_BUSY_TIME_FVAL                     0x9
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_TOT_ECC_TX_TIME_FVAL                   0xa
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_TOT_ECC_TX_RD_TIME_FVAL                0xb
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_TOT_ECC_TX_WR_TIME_FVAL                0xc
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_TOT_AHB_ACC_TIME_FVAL                  0xd
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_NOTHING0_FVAL                          0xe
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_2N_NOTHING1_FVAL                          0xf
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_BMSK                                   0xf
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_SHFT                                   0x0
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_TIME_NAND_FSM_IS_ACTIVE_FVAL        0x1
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_TIME_NAND_HW_IS_ACTIVE_FOR_A_READ_FVAL        0x2
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_TIME_NAND_HW_IS_ACTIVE_FOR_A_WRITE_FVAL        0x3
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_NUMBER_OF_READ_CODEWORDS_FVAL        0x4
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_NUMBER_OF_WRITE_CODEWORDS_FVAL        0x5
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_NUMBER_OF_ERASE_CODEWORDS_FVAL        0x6
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_READ_BUSY_TIME_FVAL                    0x7
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_WRITE_BUSY_TIME_FVAL                   0x8
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_BUSY_TIME_FVAL                   0x9
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_ECC_TRANSFER_TIME_FVAL           0xa
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_ECC_TRASFER_TIME_FOR_A_READ_FVAL        0xb
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_ECC_TRANSFER_TIME_FOR_A_WRITE_FVAL        0xc
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_TOTAL_TIME_SPENT_IN_ACCESSING_THE_AHB_IF_BUT_NOT_THE_NAND_HARDWARE_FVAL        0xd
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_NOTING_IS_PROFILED_FVAL                0xe
#define HWIO_NANDC_HW_PROFILE_CFG_HW_COUNTER_SELECT_1N_NOTHING_IS_PROFILED_FVAL               0xf

#define HWIO_FLASH_BUFFn_ACC_ADDR(n)                                              (EBI2ND_REG_BASE      + 0x00000100 + 0x4 * (n))
#define HWIO_FLASH_BUFFn_ACC_PHYS(n)                                              (EBI2ND_REG_BASE_PHYS + 0x00000100 + 0x4 * (n))
#define HWIO_FLASH_BUFFn_ACC_OFFS(n)                                              (EBI2ND_REG_BASE_OFFS + 0x00000100 + 0x4 * (n))
#define HWIO_FLASH_BUFFn_ACC_RMSK                                                 0xffffffff
#define HWIO_FLASH_BUFFn_ACC_MAXn                                                        143
#define HWIO_FLASH_BUFFn_ACC_INI(n)        \
        in_dword_masked(HWIO_FLASH_BUFFn_ACC_ADDR(n), HWIO_FLASH_BUFFn_ACC_RMSK)
#define HWIO_FLASH_BUFFn_ACC_INMI(n,mask)    \
        in_dword_masked(HWIO_FLASH_BUFFn_ACC_ADDR(n), mask)
#define HWIO_FLASH_BUFFn_ACC_OUTI(n,val)    \
        out_dword(HWIO_FLASH_BUFFn_ACC_ADDR(n),val)
#define HWIO_FLASH_BUFFn_ACC_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_FLASH_BUFFn_ACC_ADDR(n),mask,val,HWIO_FLASH_BUFFn_ACC_INI(n))
#define HWIO_FLASH_BUFFn_ACC_BUFF_DATA_BMSK                                            0xffffffff
#define HWIO_FLASH_BUFFn_ACC_BUFF_DATA_SHFT                                                   0x0

/* NAND CTRL Register will always be accessed only through Direct Register access and not through BAM. So define the Memory mapped address always */
#define HWIO_NAND_CTRL_ADDR                                                       (EBI2ND_REG_BASE_DCPU      + 0x00000f00)
#define HWIO_NAND_CTRL_PHYS                                                       (EBI2ND_REG_BASE_PHYS_DCPU + 0x00000f00)
#define HWIO_NAND_CTRL_OFFS                                                       (EBI2ND_REG_BASE_OFFS_DCPU + 0x00000f00)
#define HWIO_NAND_CTRL_RMSK                                                              0x7ff
#define HWIO_NAND_CTRL_IN          \
        in_dword_masked(HWIO_NAND_CTRL_ADDR, HWIO_NAND_CTRL_RMSK)
#define HWIO_NAND_CTRL_INM(m)      \
        in_dword_masked(HWIO_NAND_CTRL_ADDR, m)
#define HWIO_NAND_CTRL_OUT(v)      \
        out_dword(HWIO_NAND_CTRL_ADDR,v)
#define HWIO_NAND_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_CTRL_ADDR,m,v,HWIO_NAND_CTRL_IN)
#define HWIO_NAND_CTRL_NANDC_READ_DELAY_COUNTER_VAL_BMSK                                    0x7fe
#define HWIO_NAND_CTRL_NANDC_READ_DELAY_COUNTER_VAL_SHFT                                      0x1
#define HWIO_NAND_CTRL_BAM_MODE_EN_BMSK                                                       0x1
#define HWIO_NAND_CTRL_BAM_MODE_EN_SHFT                                                       0x0
#define HWIO_NAND_CTRL_BAM_MODE_EN_CPU_MODE_FVAL                                              0x0
#define HWIO_NAND_CTRL_BAM_MODE_EN_BAM_MODE_FVAL                                              0x1

#define HWIO_NAND_STATUS_ADDR                                                     (EBI2ND_REG_BASE      + 0x00000f04)
#define HWIO_NAND_STATUS_PHYS                                                     (EBI2ND_REG_BASE_PHYS + 0x00000f04)
#define HWIO_NAND_STATUS_OFFS                                                     (EBI2ND_REG_BASE_OFFS + 0x00000f04)
#define HWIO_NAND_STATUS_RMSK                                                           0x33
#define HWIO_NAND_STATUS_IN          \
        in_dword_masked(HWIO_NAND_STATUS_ADDR, HWIO_NAND_STATUS_RMSK)
#define HWIO_NAND_STATUS_INM(m)      \
        in_dword_masked(HWIO_NAND_STATUS_ADDR, m)
#define HWIO_NAND_STATUS_RX_BUF_IN_USE_BMSK                                                  0x30
#define HWIO_NAND_STATUS_RX_BUF_IN_USE_SHFT                                                   0x4
#define HWIO_NAND_STATUS_RX_BUF_IN_USE_NONE_FVAL                                              0x0
#define HWIO_NAND_STATUS_RX_BUF_IN_USE_BUFFER_0_FVAL                                          0x1
#define HWIO_NAND_STATUS_RX_BUF_IN_USE_BUFFER_1_FVAL                                          0x2
#define HWIO_NAND_STATUS_TX_BUF_IN_USE_BMSK                                                   0x3
#define HWIO_NAND_STATUS_TX_BUF_IN_USE_SHFT                                                   0x0
#define HWIO_NAND_STATUS_TX_BUF_IN_USE_NONE_FVAL                                              0x0
#define HWIO_NAND_STATUS_TX_BUF_IN_USE_BUFFER_0_FVAL                                          0x1
#define HWIO_NAND_STATUS_TX_BUF_IN_USE_BUFFER_1_FVAL                                          0x2

#define HWIO_NAND_VERSION_ADDR                                                    (EBI2ND_REG_BASE      + 0x00000f08)
#define HWIO_NAND_VERSION_PHYS                                                    (EBI2ND_REG_BASE_PHYS + 0x00000f08)
#define HWIO_NAND_VERSION_OFFS                                                    (EBI2ND_REG_BASE_OFFS + 0x00000f08)
#define HWIO_NAND_VERSION_RMSK                                                    0xffffffff
#define HWIO_NAND_VERSION_IN          \
        in_dword_masked(HWIO_NAND_VERSION_ADDR, HWIO_NAND_VERSION_RMSK)
#define HWIO_NAND_VERSION_INM(m)      \
        in_dword_masked(HWIO_NAND_VERSION_ADDR, m)
#define HWIO_NAND_VERSION_MAJOR_BMSK                                                   0xf0000000
#define HWIO_NAND_VERSION_MAJOR_SHFT                                                         0x1c
#define HWIO_NAND_VERSION_MINOR_BMSK                                                    0xfff0000
#define HWIO_NAND_VERSION_MINOR_SHFT                                                         0x10
#define HWIO_NAND_VERSION_STEP_BMSK                                                        0xffff
#define HWIO_NAND_VERSION_STEP_SHFT                                                           0x0

#define HWIO_NAND_MPU_BYPASS_ADDR                                                 (EBI2ND_REG_BASE      + 0x00000f44)
#define HWIO_NAND_MPU_BYPASS_PHYS                                                 (EBI2ND_REG_BASE_PHYS + 0x00000f44)
#define HWIO_NAND_MPU_BYPASS_OFFS                                                 (EBI2ND_REG_BASE_OFFS + 0x00000f44)
#define HWIO_NAND_MPU_BYPASS_RMSK                                                        0x1
#define HWIO_NAND_MPU_BYPASS_IN          \
        in_dword_masked(HWIO_NAND_MPU_BYPASS_ADDR, HWIO_NAND_MPU_BYPASS_RMSK)
#define HWIO_NAND_MPU_BYPASS_INM(m)      \
        in_dword_masked(HWIO_NAND_MPU_BYPASS_ADDR, m)
#define HWIO_NAND_MPU_BYPASS_OUT(v)      \
        out_dword(HWIO_NAND_MPU_BYPASS_ADDR,v)
#define HWIO_NAND_MPU_BYPASS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_MPU_BYPASS_ADDR,m,v,HWIO_NAND_MPU_BYPASS_IN)
#define HWIO_NAND_MPU_BYPASS_MPU_BYPASS_BMSK                                                  0x1
#define HWIO_NAND_MPU_BYPASS_MPU_BYPASS_SHFT                                                  0x0
#define HWIO_NAND_MPU_BYPASS_MPU_BYPASS_DISABLE_FVAL                                          0x0
#define HWIO_NAND_MPU_BYPASS_MPU_BYPASS_ENABLE_FVAL                                           0x1

#define HWIO_NAND_DEBUG_ADDR                                                      (EBI2ND_REG_BASE      + 0x00000f0c)
#define HWIO_NAND_DEBUG_PHYS                                                      (EBI2ND_REG_BASE_PHYS + 0x00000f0c)
#define HWIO_NAND_DEBUG_OFFS                                                      (EBI2ND_REG_BASE_OFFS + 0x00000f0c)
#define HWIO_NAND_DEBUG_RMSK                                                      0xbfffffff
#define HWIO_NAND_DEBUG_IN          \
        in_dword_masked(HWIO_NAND_DEBUG_ADDR, HWIO_NAND_DEBUG_RMSK)
#define HWIO_NAND_DEBUG_INM(m)      \
        in_dword_masked(HWIO_NAND_DEBUG_ADDR, m)
#define HWIO_NAND_DEBUG_OUT(v)      \
        out_dword(HWIO_NAND_DEBUG_ADDR,v)
#define HWIO_NAND_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_DEBUG_ADDR,m,v,HWIO_NAND_DEBUG_IN)
#define HWIO_NAND_DEBUG_BAM_MODE_BIT_RESET_BMSK                                        0x80000000
#define HWIO_NAND_DEBUG_BAM_MODE_BIT_RESET_SHFT                                              0x1f
#define HWIO_NAND_DEBUG_BAM_MODE_BIT_RESET_NO_RESET_FVAL                                      0x0
#define HWIO_NAND_DEBUG_BAM_MODE_BIT_RESET_RESET_FVAL                                         0x1
#define HWIO_NAND_DEBUG_COMMAND_ELEMENT_REG_SECURITY_BMSK                              0x20000000
#define HWIO_NAND_DEBUG_COMMAND_ELEMENT_REG_SECURITY_SHFT                                    0x1d
#define HWIO_NAND_DEBUG_COMMAND_ELEMENT_REG_SECURITY_SECURITY_ENABLED_FVAL                    0x0
#define HWIO_NAND_DEBUG_COMMAND_ELEMENT_REG_SECURITY_SECURITY_DISABLED_FVAL                   0x1
#define HWIO_NAND_DEBUG_RX_CPU_DIS_BUFFER_SECURITY_BMSK                                0x10000000
#define HWIO_NAND_DEBUG_RX_CPU_DIS_BUFFER_SECURITY_SHFT                                      0x1c
#define HWIO_NAND_DEBUG_RX_CPU_DIS_BUFFER_SECURITY_SECURITY_ENABLED_FVAL                      0x0
#define HWIO_NAND_DEBUG_RX_CPU_DIS_BUFFER_SECURITY_SECURITY_DISABLED_FVAL                     0x1
#define HWIO_NAND_DEBUG_RX_BAM_DIS_BUFFER_SECURITY_BMSK                                 0x8000000
#define HWIO_NAND_DEBUG_RX_BAM_DIS_BUFFER_SECURITY_SHFT                                      0x1b
#define HWIO_NAND_DEBUG_RX_BAM_DIS_BUFFER_SECURITY_SECURITY_ENABLED_FVAL                      0x0
#define HWIO_NAND_DEBUG_RX_BAM_DIS_BUFFER_SECURITY_SECURITY_DISABLED_FVAL                     0x1
#define HWIO_NAND_DEBUG_DIS_RPP_MPU_BYPASS_BMSK                                         0x4000000
#define HWIO_NAND_DEBUG_DIS_RPP_MPU_BYPASS_SHFT                                              0x1a
#define HWIO_NAND_DEBUG_DIS_RPP_MPU_BYPASS_ENABLE_FVAL                                        0x0
#define HWIO_NAND_DEBUG_DIS_RPP_MPU_BYPASS_DISABLE_FVAL                                       0x1
#define HWIO_NAND_DEBUG_DIS_RID_MPU_BYPASS_BMSK                                         0x2000000
#define HWIO_NAND_DEBUG_DIS_RID_MPU_BYPASS_SHFT                                              0x19
#define HWIO_NAND_DEBUG_DIS_RID_MPU_BYPASS_ENABLE_FVAL                                        0x0
#define HWIO_NAND_DEBUG_DIS_RID_MPU_BYPASS_DISABLE_FVAL                                       0x1
#define HWIO_NAND_DEBUG_DIS_STTS_MPU_BYPASS_BMSK                                        0x1000000
#define HWIO_NAND_DEBUG_DIS_STTS_MPU_BYPASS_SHFT                                             0x18
#define HWIO_NAND_DEBUG_DIS_STTS_MPU_BYPASS_ENABLE_FVAL                                       0x0
#define HWIO_NAND_DEBUG_DIS_STTS_MPU_BYPASS_DISABLE_FVAL                                      0x1
#define HWIO_NAND_DEBUG_DIS_RSTD_MPU_BYPASS_BMSK                                         0x800000
#define HWIO_NAND_DEBUG_DIS_RSTD_MPU_BYPASS_SHFT                                             0x17
#define HWIO_NAND_DEBUG_DIS_RSTD_MPU_BYPASS_ENABLE_FVAL                                       0x0
#define HWIO_NAND_DEBUG_DIS_RSTD_MPU_BYPASS_DISABLE_FVAL                                      0x1
#define HWIO_NAND_DEBUG_BAM_MOD_SW_EXEC_EN_BMSK                                          0x400000
#define HWIO_NAND_DEBUG_BAM_MOD_SW_EXEC_EN_SHFT                                              0x16
#define HWIO_NAND_DEBUG_BAM_MOD_SW_EXEC_EN_DISABLE_FVAL                                       0x0
#define HWIO_NAND_DEBUG_BAM_MOD_SW_EXEC_EN_ENABLE_FVAL                                        0x1
#define HWIO_NAND_DEBUG_BAM_MOD_PIPE_NUMBER_BMSK                                         0x3e0000
#define HWIO_NAND_DEBUG_BAM_MOD_PIPE_NUMBER_SHFT                                             0x11
#define HWIO_NAND_DEBUG_RX_BAM_MOD_BYTES_FREE_BMSK                                        0x1c000
#define HWIO_NAND_DEBUG_RX_BAM_MOD_BYTES_FREE_SHFT                                            0xe
#define HWIO_NAND_DEBUG_RX_BAM_MOD_BYTES_FREE_FREE_32_BYTES_FVAL                              0x0
#define HWIO_NAND_DEBUG_RX_BAM_MOD_BYTES_FREE_FREE_64_BYTES_FVAL                              0x1
#define HWIO_NAND_DEBUG_RX_BAM_MOD_BYTES_FREE_FREE_128_BYTES_FVAL                             0x2
#define HWIO_NAND_DEBUG_RX_BAM_MOD_BYTES_FREE_FREE_256_BYTES_FVAL                             0x3
#define HWIO_NAND_DEBUG_RX_BAM_MOD_BYTES_FREE_FREE_512_BYTES_FVAL                             0x4
#define HWIO_NAND_DEBUG_RX_BAM_MOD_BYTES_FREE_FREE_PIPE_FVAL                                  0x5
#define HWIO_NAND_DEBUG_RX_BAM_MOD_SNGL_BUF_BMSK                                           0x2000
#define HWIO_NAND_DEBUG_RX_BAM_MOD_SNGL_BUF_SHFT                                              0xd
#define HWIO_NAND_DEBUG_RX_BAM_MOD_SNGL_BUF_USE_2_BUFFERS_FVAL                                0x0
#define HWIO_NAND_DEBUG_RX_BAM_MOD_SNGL_BUF_USE_1_BUFFER_FVAL                                 0x1
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_FREE_BMSK                                          0x1c00
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_FREE_SHFT                                             0xa
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_FREE_FREE_32_BYTES_FVAL                               0x0
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_FREE_FREE_64_BYTES_FVAL                               0x1
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_FREE_FREE_128_BYTES_FVAL                              0x2
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_FREE_FREE_256_BYTES_FVAL                              0x3
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_FREE_FREE_512_BYTES_FVAL                              0x4
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_FREE_FREE_BUFFER_FVAL                                 0x5
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_AVAIL_BMSK                                          0x380
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_AVAIL_SHFT                                            0x7
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_AVAIL_AVAIL_32_BYTES_FVAL                             0x0
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_AVAIL_AVAIL_64_BYTES_FVAL                             0x1
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_AVAIL_AVAIL_128_BYTES_FVAL                            0x2
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_AVAIL_AVAIL_256_BYTES_FVAL                            0x3
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_AVAIL_AVAIL_512_BYTES_FVAL                            0x4
#define HWIO_NAND_DEBUG_TX_BAM_MOD_PREF_AVAIL_AVAIL_CODEWORD_FVAL                             0x5
#define HWIO_NAND_DEBUG_EN_BAM_MOD_ACTV_FRST_BMSK                                            0x40
#define HWIO_NAND_DEBUG_EN_BAM_MOD_ACTV_FRST_SHFT                                             0x6
#define HWIO_NAND_DEBUG_EN_BAM_MOD_ACTV_FRST_DISABLE_FVAL                                     0x0
#define HWIO_NAND_DEBUG_EN_BAM_MOD_ACTV_FRST_ENABLE_FVAL                                      0x1
#define HWIO_NAND_DEBUG_DIS_BAM_MOD_BAM_CONC_BMSK                                            0x20
#define HWIO_NAND_DEBUG_DIS_BAM_MOD_BAM_CONC_SHFT                                             0x5
#define HWIO_NAND_DEBUG_DIS_BAM_MOD_BAM_CONC_ENABLED_FVAL                                     0x0
#define HWIO_NAND_DEBUG_DIS_BAM_MOD_BAM_CONC_DISABLED_FVAL                                    0x1
#define HWIO_NAND_DEBUG_DIS_BAM_MOD_TX_CONC_BMSK                                             0x10
#define HWIO_NAND_DEBUG_DIS_BAM_MOD_TX_CONC_SHFT                                              0x4
#define HWIO_NAND_DEBUG_DIS_BAM_MOD_TX_CONC_ENABLED_FVAL                                      0x0
#define HWIO_NAND_DEBUG_DIS_BAM_MOD_TX_CONC_DISABLED_FVAL                                     0x1
#define HWIO_NAND_DEBUG_EN_BAM_MOD_BUF_ACC_BMSK                                               0x8
#define HWIO_NAND_DEBUG_EN_BAM_MOD_BUF_ACC_SHFT                                               0x3
#define HWIO_NAND_DEBUG_EN_BAM_MOD_BUF_ACC_DISABLE_FVAL                                       0x0
#define HWIO_NAND_DEBUG_EN_BAM_MOD_BUF_ACC_ENABLE_FVAL                                        0x1
#define HWIO_NAND_DEBUG_EN_CPU_MOD_BUF_ACC_BMSK                                               0x4
#define HWIO_NAND_DEBUG_EN_CPU_MOD_BUF_ACC_SHFT                                               0x2
#define HWIO_NAND_DEBUG_EN_CPU_MOD_BUF_ACC_DISABLE_FVAL                                       0x0
#define HWIO_NAND_DEBUG_EN_CPU_MOD_BUF_ACC_ENABLE_FVAL                                        0x1
#define HWIO_NAND_DEBUG_EN_BAM_MOD_REG_ACC_BMSK                                               0x2
#define HWIO_NAND_DEBUG_EN_BAM_MOD_REG_ACC_SHFT                                               0x1
#define HWIO_NAND_DEBUG_EN_BAM_MOD_REG_ACC_DISABLE_FVAL                                       0x0
#define HWIO_NAND_DEBUG_EN_BAM_MOD_REG_ACC_ENABLE_FVAL                                        0x1
#define HWIO_NAND_DEBUG_EN_CPU_MOD_REG_ACC_BMSK                                               0x1
#define HWIO_NAND_DEBUG_EN_CPU_MOD_REG_ACC_SHFT                                               0x0
#define HWIO_NAND_DEBUG_EN_CPU_MOD_REG_ACC_DISABLE_FVAL                                       0x0
#define HWIO_NAND_DEBUG_EN_CPU_MOD_REG_ACC_ENABLE_FVAL                                        0x1

#define HWIO_NAND_IRQ_STTS_ADDR                                                   (EBI2ND_REG_BASE      + 0x00000f10)
#define HWIO_NAND_IRQ_STTS_PHYS                                                   (EBI2ND_REG_BASE_PHYS + 0x00000f10)
#define HWIO_NAND_IRQ_STTS_OFFS                                                   (EBI2ND_REG_BASE_OFFS + 0x00000f10)
#define HWIO_NAND_IRQ_STTS_RMSK                                                          0x7
#define HWIO_NAND_IRQ_STTS_IN          \
        in_dword_masked(HWIO_NAND_IRQ_STTS_ADDR, HWIO_NAND_IRQ_STTS_RMSK)
#define HWIO_NAND_IRQ_STTS_INM(m)      \
        in_dword_masked(HWIO_NAND_IRQ_STTS_ADDR, m)
#define HWIO_NAND_IRQ_STTS_ERROR_BMSK                                                         0x4
#define HWIO_NAND_IRQ_STTS_ERROR_SHFT                                                         0x2
#define HWIO_NAND_IRQ_STTS_WRITE_ERASE_DONE_BMSK                                              0x2
#define HWIO_NAND_IRQ_STTS_WRITE_ERASE_DONE_SHFT                                              0x1
#define HWIO_NAND_IRQ_STTS_OPERATION_DONE_BMSK                                                0x1
#define HWIO_NAND_IRQ_STTS_OPERATION_DONE_SHFT                                                0x0

#define HWIO_NAND_IRQ_CLR_ADDR                                                    (EBI2ND_REG_BASE      + 0x00000f14)
#define HWIO_NAND_IRQ_CLR_PHYS                                                    (EBI2ND_REG_BASE_PHYS + 0x00000f14)
#define HWIO_NAND_IRQ_CLR_OFFS                                                    (EBI2ND_REG_BASE_OFFS + 0x00000f14)
#define HWIO_NAND_IRQ_CLR_RMSK                                                           0x7
#define HWIO_NAND_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_NAND_IRQ_CLR_ADDR,v)
#define HWIO_NAND_IRQ_CLR_ERROR_BMSK                                                          0x4
#define HWIO_NAND_IRQ_CLR_ERROR_SHFT                                                          0x2
#define HWIO_NAND_IRQ_CLR_ERROR_NO_CHANGE_FVAL                                                0x0
#define HWIO_NAND_IRQ_CLR_ERROR_CLEAR_FVAL                                                    0x1
#define HWIO_NAND_IRQ_CLR_WRITE_ERASE_DONE_BMSK                                               0x2
#define HWIO_NAND_IRQ_CLR_WRITE_ERASE_DONE_SHFT                                               0x1
#define HWIO_NAND_IRQ_CLR_WRITE_ERASE_DONE_NO_CHANGE_FVAL                                     0x0
#define HWIO_NAND_IRQ_CLR_WRITE_ERASE_DONE_CLEAR_FVAL                                         0x1
#define HWIO_NAND_IRQ_CLR_OPERATION_DONE_BMSK                                                 0x1
#define HWIO_NAND_IRQ_CLR_OPERATION_DONE_SHFT                                                 0x0
#define HWIO_NAND_IRQ_CLR_OPERATION_DONE_NO_CHANGE_FVAL                                       0x0
#define HWIO_NAND_IRQ_CLR_OPERATION_DONE_CLEAR_FVAL                                           0x1

#define HWIO_NAND_IRQ_EN_ADDR                                                     (EBI2ND_REG_BASE      + 0x00000f18)
#define HWIO_NAND_IRQ_EN_PHYS                                                     (EBI2ND_REG_BASE_PHYS + 0x00000f18)
#define HWIO_NAND_IRQ_EN_OFFS                                                     (EBI2ND_REG_BASE_OFFS + 0x00000f18)
#define HWIO_NAND_IRQ_EN_RMSK                                                            0x7
#define HWIO_NAND_IRQ_EN_IN          \
        in_dword_masked(HWIO_NAND_IRQ_EN_ADDR, HWIO_NAND_IRQ_EN_RMSK)
#define HWIO_NAND_IRQ_EN_INM(m)      \
        in_dword_masked(HWIO_NAND_IRQ_EN_ADDR, m)
#define HWIO_NAND_IRQ_EN_OUT(v)      \
        out_dword(HWIO_NAND_IRQ_EN_ADDR,v)
#define HWIO_NAND_IRQ_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_IRQ_EN_ADDR,m,v,HWIO_NAND_IRQ_EN_IN)
#define HWIO_NAND_IRQ_EN_ERROR_BMSK                                                           0x4
#define HWIO_NAND_IRQ_EN_ERROR_SHFT                                                           0x2
#define HWIO_NAND_IRQ_EN_ERROR_DISABLE_FVAL                                                   0x0
#define HWIO_NAND_IRQ_EN_ERROR_ENABLE_FVAL                                                    0x1
#define HWIO_NAND_IRQ_EN_WRITE_ERASE_DONE_BMSK                                                0x2
#define HWIO_NAND_IRQ_EN_WRITE_ERASE_DONE_SHFT                                                0x1
#define HWIO_NAND_IRQ_EN_WRITE_ERASE_DONE_DISABLE_FVAL                                        0x0
#define HWIO_NAND_IRQ_EN_WRITE_ERASE_DONE_ENABLE_FVAL                                         0x1
#define HWIO_NAND_IRQ_EN_OPERATION_DONE_BMSK                                                  0x1
#define HWIO_NAND_IRQ_EN_OPERATION_DONE_SHFT                                                  0x0
#define HWIO_NAND_IRQ_EN_OPERATION_DONE_DISABLE_FVAL                                          0x0
#define HWIO_NAND_IRQ_EN_OPERATION_DONE_ENABLE_FVAL                                           0x1

#define HWIO_NAND_MUTEX_ADDR                                                      (EBI2ND_REG_BASE      + 0x00000f1c)
#define HWIO_NAND_MUTEX_PHYS                                                      (EBI2ND_REG_BASE_PHYS + 0x00000f1c)
#define HWIO_NAND_MUTEX_OFFS                                                      (EBI2ND_REG_BASE_OFFS + 0x00000f1c)
#define HWIO_NAND_MUTEX_RMSK                                                             0x1
#define HWIO_NAND_MUTEX_IN          \
        in_dword_masked(HWIO_NAND_MUTEX_ADDR, HWIO_NAND_MUTEX_RMSK)
#define HWIO_NAND_MUTEX_INM(m)      \
        in_dword_masked(HWIO_NAND_MUTEX_ADDR, m)
#define HWIO_NAND_MUTEX_OUT(v)      \
        out_dword(HWIO_NAND_MUTEX_ADDR,v)
#define HWIO_NAND_MUTEX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_MUTEX_ADDR,m,v,HWIO_NAND_MUTEX_IN)
#define HWIO_NAND_MUTEX_MUTEX_LOCK_BMSK                                                       0x1
#define HWIO_NAND_MUTEX_MUTEX_LOCK_SHFT                                                       0x0
#define HWIO_NAND_MUTEX_MUTEX_LOCK_UNLOCKED_FVAL                                              0x0
#define HWIO_NAND_MUTEX_MUTEX_LOCK_LOCKED_FVAL                                                0x1

#define HWIO_NAND_CONFIG_BITS_ADDR                                                (EBI2ND_REG_BASE      + 0x00000f40)
#define HWIO_NAND_CONFIG_BITS_PHYS                                                (EBI2ND_REG_BASE_PHYS + 0x00000f40)
#define HWIO_NAND_CONFIG_BITS_OFFS                                                (EBI2ND_REG_BASE_OFFS + 0x00000f40)
#define HWIO_NAND_CONFIG_BITS_RMSK                                                0xffffffff
#define HWIO_NAND_CONFIG_BITS_IN          \
        in_dword_masked(HWIO_NAND_CONFIG_BITS_ADDR, HWIO_NAND_CONFIG_BITS_RMSK)
#define HWIO_NAND_CONFIG_BITS_INM(m)      \
        in_dword_masked(HWIO_NAND_CONFIG_BITS_ADDR, m)
#define HWIO_NAND_CONFIG_BITS_OUT(v)      \
        out_dword(HWIO_NAND_CONFIG_BITS_ADDR,v)
#define HWIO_NAND_CONFIG_BITS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_CONFIG_BITS_ADDR,m,v,HWIO_NAND_CONFIG_BITS_IN)
#define HWIO_NAND_CONFIG_BITS_BITS_1_BMSK                                              0xffff0000
#define HWIO_NAND_CONFIG_BITS_BITS_1_SHFT                                                    0x10
#define HWIO_NAND_CONFIG_BITS_FF_USED_FOR_ECO_BMSK                                         0x8000
#define HWIO_NAND_CONFIG_BITS_FF_USED_FOR_ECO_SHFT                                            0xf
#define HWIO_NAND_CONFIG_BITS_BITS_0_BMSK                                                  0x7ff0
#define HWIO_NAND_CONFIG_BITS_BITS_0_SHFT                                                     0x4
#define HWIO_NAND_CONFIG_BITS_BIT_BMSK                                                        0x8
#define HWIO_NAND_CONFIG_BITS_BIT_SHFT                                                        0x3
#define HWIO_NAND_CONFIG_BITS_NANDC_FSM_DIS_READ_0_LOOP_FIX_BMSK                              0x4
#define HWIO_NAND_CONFIG_BITS_NANDC_FSM_DIS_READ_0_LOOP_FIX_SHFT                              0x2
#define HWIO_NAND_CONFIG_BITS_NANDC_FSM_DIS_READ_0_LOOP_FIX_ENABLE_FVAL                       0x0
#define HWIO_NAND_CONFIG_BITS_NANDC_FSM_DIS_READ_0_LOOP_FIX_DISABLE_FVAL                      0x1
#define HWIO_NAND_CONFIG_BITS_NANDC_AHB_IF_HRDY_FIX2_BMSK                                     0x2
#define HWIO_NAND_CONFIG_BITS_NANDC_AHB_IF_HRDY_FIX2_SHFT                                     0x1
#define HWIO_NAND_CONFIG_BITS_NANDC_AHB_IF_HRDY_FIX2_ENABLE_FVAL                              0x0
#define HWIO_NAND_CONFIG_BITS_NANDC_AHB_IF_HRDY_FIX2_DISABLE_FVAL                             0x1
#define HWIO_NAND_CONFIG_BITS_NANDC_AHB_IF_HRDY_FIX1_BMSK                                     0x1
#define HWIO_NAND_CONFIG_BITS_NANDC_AHB_IF_HRDY_FIX1_SHFT                                     0x0
#define HWIO_NAND_CONFIG_BITS_NANDC_AHB_IF_HRDY_FIX1_ENABLE_FVAL                              0x0
#define HWIO_NAND_CONFIG_BITS_NANDC_AHB_IF_HRDY_FIX1_DISABLE_FVAL                             0x1

#define HWIO_NAND_READ_LOCATION_n_ADDR(n)                                         (EBI2ND_REG_BASE      + 0x00000f20 + 0x4 * (n))
#define HWIO_NAND_READ_LOCATION_n_PHYS(n)                                         (EBI2ND_REG_BASE_PHYS + 0x00000f20 + 0x4 * (n))
#define HWIO_NAND_READ_LOCATION_n_OFFS(n)                                         (EBI2ND_REG_BASE_OFFS + 0x00000f20 + 0x4 * (n))
#define HWIO_NAND_READ_LOCATION_n_RMSK                                            0x83ff03ff
#define HWIO_NAND_READ_LOCATION_n_MAXn                                                     4
#define HWIO_NAND_READ_LOCATION_n_INI(n)        \
        in_dword_masked(HWIO_NAND_READ_LOCATION_n_ADDR(n), HWIO_NAND_READ_LOCATION_n_RMSK)
#define HWIO_NAND_READ_LOCATION_n_INMI(n,mask)    \
        in_dword_masked(HWIO_NAND_READ_LOCATION_n_ADDR(n), mask)
#define HWIO_NAND_READ_LOCATION_n_OUTI(n,val)    \
        out_dword(HWIO_NAND_READ_LOCATION_n_ADDR(n),val)
#define HWIO_NAND_READ_LOCATION_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_NAND_READ_LOCATION_n_ADDR(n),mask,val,HWIO_NAND_READ_LOCATION_n_INI(n))
#define HWIO_NAND_READ_LOCATION_n_LAST_BMSK                                            0x80000000
#define HWIO_NAND_READ_LOCATION_n_LAST_SHFT                                                  0x1f
#define HWIO_NAND_READ_LOCATION_n_SIZE_BMSK                                             0x3ff0000
#define HWIO_NAND_READ_LOCATION_n_SIZE_SHFT                                                  0x10
#define HWIO_NAND_READ_LOCATION_n_OFFSET_BMSK                                               0x3ff
#define HWIO_NAND_READ_LOCATION_n_OFFSET_SHFT                                                 0x0

#define HWIO_NAND_TEST_BUS_LSB_ADDR                                               (EBI2ND_REG_BASE      + 0x00000f48)
#define HWIO_NAND_TEST_BUS_LSB_PHYS                                               (EBI2ND_REG_BASE_PHYS + 0x00000f48)
#define HWIO_NAND_TEST_BUS_LSB_OFFS                                               (EBI2ND_REG_BASE_OFFS + 0x00000f48)
#define HWIO_NAND_TEST_BUS_LSB_RMSK                                               0xffffffff
#define HWIO_NAND_TEST_BUS_LSB_IN          \
        in_dword_masked(HWIO_NAND_TEST_BUS_LSB_ADDR, HWIO_NAND_TEST_BUS_LSB_RMSK)
#define HWIO_NAND_TEST_BUS_LSB_INM(m)      \
        in_dword_masked(HWIO_NAND_TEST_BUS_LSB_ADDR, m)
#define HWIO_NAND_TEST_BUS_LSB_TEST_BUS_BMSK                                           0xffffffff
#define HWIO_NAND_TEST_BUS_LSB_TEST_BUS_SHFT                                                  0x0

#define HWIO_NAND_TEST_BUS_MSB_ADDR                                               (EBI2ND_REG_BASE      + 0x00000f4c)
#define HWIO_NAND_TEST_BUS_MSB_PHYS                                               (EBI2ND_REG_BASE_PHYS + 0x00000f4c)
#define HWIO_NAND_TEST_BUS_MSB_OFFS                                               (EBI2ND_REG_BASE_OFFS + 0x00000f4c)
#define HWIO_NAND_TEST_BUS_MSB_RMSK                                               0xffffffff
#define HWIO_NAND_TEST_BUS_MSB_IN          \
        in_dword_masked(HWIO_NAND_TEST_BUS_MSB_ADDR, HWIO_NAND_TEST_BUS_MSB_RMSK)
#define HWIO_NAND_TEST_BUS_MSB_INM(m)      \
        in_dword_masked(HWIO_NAND_TEST_BUS_MSB_ADDR, m)
#define HWIO_NAND_TEST_BUS_MSB_TEST_BUS_BMSK                                           0xffffffff
#define HWIO_NAND_TEST_BUS_MSB_TEST_BUS_SHFT                                                  0x0

#define HWIO_NAND_OP_DONE_CNT_ADDR                                                (EBI2ND_REG_BASE      + 0x00000f50)
#define HWIO_NAND_OP_DONE_CNT_PHYS                                                (EBI2ND_REG_BASE_PHYS + 0x00000f50)
#define HWIO_NAND_OP_DONE_CNT_OFFS                                                (EBI2ND_REG_BASE_OFFS + 0x00000f50)
#define HWIO_NAND_OP_DONE_CNT_RMSK                                                    0xffff
#define HWIO_NAND_OP_DONE_CNT_IN          \
        in_dword_masked(HWIO_NAND_OP_DONE_CNT_ADDR, HWIO_NAND_OP_DONE_CNT_RMSK)
#define HWIO_NAND_OP_DONE_CNT_INM(m)      \
        in_dword_masked(HWIO_NAND_OP_DONE_CNT_ADDR, m)
#define HWIO_NAND_OP_DONE_CNT_OUT(v)      \
        out_dword(HWIO_NAND_OP_DONE_CNT_ADDR,v)
#define HWIO_NAND_OP_DONE_CNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_OP_DONE_CNT_ADDR,m,v,HWIO_NAND_OP_DONE_CNT_IN)
#define HWIO_NAND_OP_DONE_CNT_OP_DONE_DELAY_BMSK                                           0xffff
#define HWIO_NAND_OP_DONE_CNT_OP_DONE_DELAY_SHFT                                              0x0

#define HWIO_NAND_ERR_RESP_TIME_OUT_ADDR                                          (EBI2ND_REG_BASE      + 0x00000f54)
#define HWIO_NAND_ERR_RESP_TIME_OUT_PHYS                                          (EBI2ND_REG_BASE_PHYS + 0x00000f54)
#define HWIO_NAND_ERR_RESP_TIME_OUT_OFFS                                          (EBI2ND_REG_BASE_OFFS + 0x00000f54)
#define HWIO_NAND_ERR_RESP_TIME_OUT_RMSK                                              0xffff
#define HWIO_NAND_ERR_RESP_TIME_OUT_IN          \
        in_dword_masked(HWIO_NAND_ERR_RESP_TIME_OUT_ADDR, HWIO_NAND_ERR_RESP_TIME_OUT_RMSK)
#define HWIO_NAND_ERR_RESP_TIME_OUT_INM(m)      \
        in_dword_masked(HWIO_NAND_ERR_RESP_TIME_OUT_ADDR, m)
#define HWIO_NAND_ERR_RESP_TIME_OUT_OUT(v)      \
        out_dword(HWIO_NAND_ERR_RESP_TIME_OUT_ADDR,v)
#define HWIO_NAND_ERR_RESP_TIME_OUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NAND_ERR_RESP_TIME_OUT_ADDR,m,v,HWIO_NAND_ERR_RESP_TIME_OUT_IN)
#define HWIO_NAND_ERR_RESP_TIME_OUT_ERR_RESP_TIME_OUT_VAL_BMSK                             0xffff
#define HWIO_NAND_ERR_RESP_TIME_OUT_ERR_RESP_TIME_OUT_VAL_SHFT                                0x0


#endif /* __FLASH_HAL_HWIO_8X09_H__ */
