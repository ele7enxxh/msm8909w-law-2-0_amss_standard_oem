#ifndef __QUP_HWIO__
#define __QUP_HWIO__
/*
===========================================================================
*/
/**
  @file qup_hwio
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    PERIPH_SS_BLSP1_BLSP_QUP0

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/qup/hw/qup_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: QUP0
 *--------------------------------------------------------------------------*/

#define QUP0_REG_BASE                                                                                (PERIPH_SS_BASE      + 0x00123000)
#define QUP0_REG_BASE_PHYS                                                                           (PERIPH_SS_BASE_PHYS + 0x00123000)
#define QUP0_REG_BASE_OFFS                                                                           0x00123000

#define HWIO_QUP_CONFIG_ADDR(x)                                                                      ((x) + 0x00000000)
#define HWIO_QUP_CONFIG_PHYS(x)                                                                      ((x) + 0x00000000)
#define HWIO_QUP_CONFIG_OFFS                                                                         (0x00000000)
#define HWIO_QUP_CONFIG_RMSK                                                                            0x17fdf
#define HWIO_QUP_CONFIG_IN(x)      \
        in_dword_masked(HWIO_QUP_CONFIG_ADDR(x), HWIO_QUP_CONFIG_RMSK)
#define HWIO_QUP_CONFIG_INM(x, m)      \
        in_dword_masked(HWIO_QUP_CONFIG_ADDR(x), m)
#define HWIO_QUP_CONFIG_OUT(x, v)      \
        out_dword(HWIO_QUP_CONFIG_ADDR(x),v)
#define HWIO_QUP_CONFIG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_CONFIG_ADDR(x),m,v,HWIO_QUP_CONFIG_IN(x))
#define HWIO_QUP_CONFIG_EN_EXT_OUT_FLAG_BMSK                                                            0x10000
#define HWIO_QUP_CONFIG_EN_EXT_OUT_FLAG_SHFT                                                               0x10
#define HWIO_QUP_CONFIG_FIFO_CLK_GATE_EN_BMSK                                                            0x4000
#define HWIO_QUP_CONFIG_FIFO_CLK_GATE_EN_SHFT                                                               0xe
#define HWIO_QUP_CONFIG_CORE_CLK_ON_EN_BMSK                                                              0x2000
#define HWIO_QUP_CONFIG_CORE_CLK_ON_EN_SHFT                                                                 0xd
#define HWIO_QUP_CONFIG_APP_CLK_ON_EN_BMSK                                                               0x1000
#define HWIO_QUP_CONFIG_APP_CLK_ON_EN_SHFT                                                                  0xc
#define HWIO_QUP_CONFIG_MINI_CORE_BMSK                                                                    0xf00
#define HWIO_QUP_CONFIG_MINI_CORE_SHFT                                                                      0x8
#define HWIO_QUP_CONFIG_NO_INPUT_BMSK                                                                      0x80
#define HWIO_QUP_CONFIG_NO_INPUT_SHFT                                                                       0x7
#define HWIO_QUP_CONFIG_NO_OUTPUT_BMSK                                                                     0x40
#define HWIO_QUP_CONFIG_NO_OUTPUT_SHFT                                                                      0x6
#define HWIO_QUP_CONFIG_N_BMSK                                                                             0x1f
#define HWIO_QUP_CONFIG_N_SHFT                                                                              0x0

#define HWIO_QUP_STATE_ADDR(x)                                                                       ((x) + 0x00000004)
#define HWIO_QUP_STATE_PHYS(x)                                                                       ((x) + 0x00000004)
#define HWIO_QUP_STATE_OFFS                                                                          (0x00000004)
#define HWIO_QUP_STATE_RMSK                                                                              0xffff
#define HWIO_QUP_STATE_IN(x)      \
        in_dword_masked(HWIO_QUP_STATE_ADDR(x), HWIO_QUP_STATE_RMSK)
#define HWIO_QUP_STATE_INM(x, m)      \
        in_dword_masked(HWIO_QUP_STATE_ADDR(x), m)
#define HWIO_QUP_STATE_OUT(x, v)      \
        out_dword(HWIO_QUP_STATE_ADDR(x),v)
#define HWIO_QUP_STATE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_STATE_ADDR(x),m,v,HWIO_QUP_STATE_IN(x))
#define HWIO_QUP_STATE_TRANSFER_CANCEL_ID_BMSK                                                           0xff00
#define HWIO_QUP_STATE_TRANSFER_CANCEL_ID_SHFT                                                              0x8
#define HWIO_QUP_STATE_TRANSFER_CANCEL_BMSK                                                                0x80
#define HWIO_QUP_STATE_TRANSFER_CANCEL_SHFT                                                                 0x7
#define HWIO_QUP_STATE_I2C_FLUSH_BMSK                                                                      0x40
#define HWIO_QUP_STATE_I2C_FLUSH_SHFT                                                                       0x6
#define HWIO_QUP_STATE_WAIT_FOR_EOT_BMSK                                                                   0x20
#define HWIO_QUP_STATE_WAIT_FOR_EOT_SHFT                                                                    0x5
#define HWIO_QUP_STATE_I2C_MAST_GEN_BMSK                                                                   0x10
#define HWIO_QUP_STATE_I2C_MAST_GEN_SHFT                                                                    0x4
#define HWIO_QUP_STATE_SPI_GEN_BMSK                                                                         0x8
#define HWIO_QUP_STATE_SPI_GEN_SHFT                                                                         0x3
#define HWIO_QUP_STATE_STATE_VALID_BMSK                                                                     0x4
#define HWIO_QUP_STATE_STATE_VALID_SHFT                                                                     0x2
#define HWIO_QUP_STATE_STATE_BMSK                                                                           0x3
#define HWIO_QUP_STATE_STATE_SHFT                                                                           0x0

#define HWIO_QUP_IO_MODES_ADDR(x)                                                                    ((x) + 0x00000008)
#define HWIO_QUP_IO_MODES_PHYS(x)                                                                    ((x) + 0x00000008)
#define HWIO_QUP_IO_MODES_OFFS                                                                       (0x00000008)
#define HWIO_QUP_IO_MODES_RMSK                                                                          0x1ffff
#define HWIO_QUP_IO_MODES_IN(x)      \
        in_dword_masked(HWIO_QUP_IO_MODES_ADDR(x), HWIO_QUP_IO_MODES_RMSK)
#define HWIO_QUP_IO_MODES_INM(x, m)      \
        in_dword_masked(HWIO_QUP_IO_MODES_ADDR(x), m)
#define HWIO_QUP_IO_MODES_OUT(x, v)      \
        out_dword(HWIO_QUP_IO_MODES_ADDR(x),v)
#define HWIO_QUP_IO_MODES_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_IO_MODES_ADDR(x),m,v,HWIO_QUP_IO_MODES_IN(x))
#define HWIO_QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN_BMSK                                                      0x10000
#define HWIO_QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN_SHFT                                                         0x10
#define HWIO_QUP_IO_MODES_PACK_EN_BMSK                                                                   0x8000
#define HWIO_QUP_IO_MODES_PACK_EN_SHFT                                                                      0xf
#define HWIO_QUP_IO_MODES_UNPACK_EN_BMSK                                                                 0x4000
#define HWIO_QUP_IO_MODES_UNPACK_EN_SHFT                                                                    0xe
#define HWIO_QUP_IO_MODES_INPUT_MODE_BMSK                                                                0x3000
#define HWIO_QUP_IO_MODES_INPUT_MODE_SHFT                                                                   0xc
#define HWIO_QUP_IO_MODES_OUTPUT_MODE_BMSK                                                                0xc00
#define HWIO_QUP_IO_MODES_OUTPUT_MODE_SHFT                                                                  0xa
#define HWIO_QUP_IO_MODES_INPUT_FIFO_SIZE_BMSK                                                            0x380
#define HWIO_QUP_IO_MODES_INPUT_FIFO_SIZE_SHFT                                                              0x7
#define HWIO_QUP_IO_MODES_INPUT_BLOCK_SIZE_BMSK                                                            0x60
#define HWIO_QUP_IO_MODES_INPUT_BLOCK_SIZE_SHFT                                                             0x5
#define HWIO_QUP_IO_MODES_OUTPUT_FIFO_SIZE_BMSK                                                            0x1c
#define HWIO_QUP_IO_MODES_OUTPUT_FIFO_SIZE_SHFT                                                             0x2
#define HWIO_QUP_IO_MODES_OUTPUT_BLOCK_SIZE_BMSK                                                            0x3
#define HWIO_QUP_IO_MODES_OUTPUT_BLOCK_SIZE_SHFT                                                            0x0

#define HWIO_QUP_SW_RESET_ADDR(x)                                                                    ((x) + 0x0000000c)
#define HWIO_QUP_SW_RESET_PHYS(x)                                                                    ((x) + 0x0000000c)
#define HWIO_QUP_SW_RESET_OFFS                                                                       (0x0000000c)
#define HWIO_QUP_SW_RESET_RMSK                                                                       0xffffffff
#define HWIO_QUP_SW_RESET_OUT(x, v)      \
        out_dword(HWIO_QUP_SW_RESET_ADDR(x),v)
#define HWIO_QUP_SW_RESET_QUP_SW_RESET_BMSK                                                          0xffffffff
#define HWIO_QUP_SW_RESET_QUP_SW_RESET_SHFT                                                                 0x0

#define HWIO_QUP_OPERATIONAL_ADDR(x)                                                                 ((x) + 0x00000018)
#define HWIO_QUP_OPERATIONAL_PHYS(x)                                                                 ((x) + 0x00000018)
#define HWIO_QUP_OPERATIONAL_OFFS                                                                    (0x00000018)
#define HWIO_QUP_OPERATIONAL_RMSK                                                                        0xfff0
#define HWIO_QUP_OPERATIONAL_IN(x)      \
        in_dword_masked(HWIO_QUP_OPERATIONAL_ADDR(x), HWIO_QUP_OPERATIONAL_RMSK)
#define HWIO_QUP_OPERATIONAL_INM(x, m)      \
        in_dword_masked(HWIO_QUP_OPERATIONAL_ADDR(x), m)
#define HWIO_QUP_OPERATIONAL_OUT(x, v)      \
        out_dword(HWIO_QUP_OPERATIONAL_ADDR(x),v)
#define HWIO_QUP_OPERATIONAL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_OPERATIONAL_ADDR(x),m,v,HWIO_QUP_OPERATIONAL_IN(x))
#define HWIO_QUP_OPERATIONAL_NWD_BMSK                                                                    0x8000
#define HWIO_QUP_OPERATIONAL_NWD_SHFT                                                                       0xf
#define HWIO_QUP_OPERATIONAL_DONE_TOGGLE_BMSK                                                            0x4000
#define HWIO_QUP_OPERATIONAL_DONE_TOGGLE_SHFT                                                               0xe
#define HWIO_QUP_OPERATIONAL_IN_BLOCK_READ_REQ_BMSK                                                      0x2000
#define HWIO_QUP_OPERATIONAL_IN_BLOCK_READ_REQ_SHFT                                                         0xd
#define HWIO_QUP_OPERATIONAL_OUT_BLOCK_WRITE_REQ_BMSK                                                    0x1000
#define HWIO_QUP_OPERATIONAL_OUT_BLOCK_WRITE_REQ_SHFT                                                       0xc
#define HWIO_QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_BMSK                                                     0x800
#define HWIO_QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_SHFT                                                       0xb
#define HWIO_QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_BMSK                                                    0x400
#define HWIO_QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_SHFT                                                      0xa
#define HWIO_QUP_OPERATIONAL_INPUT_SERVICE_FLAG_BMSK                                                      0x200
#define HWIO_QUP_OPERATIONAL_INPUT_SERVICE_FLAG_SHFT                                                        0x9
#define HWIO_QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_BMSK                                                     0x100
#define HWIO_QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_SHFT                                                       0x8
#define HWIO_QUP_OPERATIONAL_INPUT_FIFO_FULL_BMSK                                                          0x80
#define HWIO_QUP_OPERATIONAL_INPUT_FIFO_FULL_SHFT                                                           0x7
#define HWIO_QUP_OPERATIONAL_OUTPUT_FIFO_FULL_BMSK                                                         0x40
#define HWIO_QUP_OPERATIONAL_OUTPUT_FIFO_FULL_SHFT                                                          0x6
#define HWIO_QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_BMSK                                                     0x20
#define HWIO_QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_SHFT                                                      0x5
#define HWIO_QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_BMSK                                                    0x10
#define HWIO_QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_SHFT                                                     0x4

#define HWIO_QUP_ERROR_FLAGS_ADDR(x)                                                                 ((x) + 0x0000001c)
#define HWIO_QUP_ERROR_FLAGS_PHYS(x)                                                                 ((x) + 0x0000001c)
#define HWIO_QUP_ERROR_FLAGS_OFFS                                                                    (0x0000001c)
#define HWIO_QUP_ERROR_FLAGS_RMSK                                                                          0x3c
#define HWIO_QUP_ERROR_FLAGS_IN(x)      \
        in_dword_masked(HWIO_QUP_ERROR_FLAGS_ADDR(x), HWIO_QUP_ERROR_FLAGS_RMSK)
#define HWIO_QUP_ERROR_FLAGS_INM(x, m)      \
        in_dword_masked(HWIO_QUP_ERROR_FLAGS_ADDR(x), m)
#define HWIO_QUP_ERROR_FLAGS_OUT(x, v)      \
        out_dword(HWIO_QUP_ERROR_FLAGS_ADDR(x),v)
#define HWIO_QUP_ERROR_FLAGS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_ERROR_FLAGS_ADDR(x),m,v,HWIO_QUP_ERROR_FLAGS_IN(x))
#define HWIO_QUP_ERROR_FLAGS_OUTPUT_OVER_RUN_ERR_BMSK                                                      0x20
#define HWIO_QUP_ERROR_FLAGS_OUTPUT_OVER_RUN_ERR_SHFT                                                       0x5
#define HWIO_QUP_ERROR_FLAGS_INPUT_UNDER_RUN_ERR_BMSK                                                      0x10
#define HWIO_QUP_ERROR_FLAGS_INPUT_UNDER_RUN_ERR_SHFT                                                       0x4
#define HWIO_QUP_ERROR_FLAGS_OUTPUT_UNDER_RUN_ERR_BMSK                                                      0x8
#define HWIO_QUP_ERROR_FLAGS_OUTPUT_UNDER_RUN_ERR_SHFT                                                      0x3
#define HWIO_QUP_ERROR_FLAGS_INPUT_OVER_RUN_ERR_BMSK                                                        0x4
#define HWIO_QUP_ERROR_FLAGS_INPUT_OVER_RUN_ERR_SHFT                                                        0x2

#define HWIO_QUP_ERROR_FLAGS_EN_ADDR(x)                                                              ((x) + 0x00000020)
#define HWIO_QUP_ERROR_FLAGS_EN_PHYS(x)                                                              ((x) + 0x00000020)
#define HWIO_QUP_ERROR_FLAGS_EN_OFFS                                                                 (0x00000020)
#define HWIO_QUP_ERROR_FLAGS_EN_RMSK                                                                       0x3c
#define HWIO_QUP_ERROR_FLAGS_EN_IN(x)      \
        in_dword_masked(HWIO_QUP_ERROR_FLAGS_EN_ADDR(x), HWIO_QUP_ERROR_FLAGS_EN_RMSK)
#define HWIO_QUP_ERROR_FLAGS_EN_INM(x, m)      \
        in_dword_masked(HWIO_QUP_ERROR_FLAGS_EN_ADDR(x), m)
#define HWIO_QUP_ERROR_FLAGS_EN_OUT(x, v)      \
        out_dword(HWIO_QUP_ERROR_FLAGS_EN_ADDR(x),v)
#define HWIO_QUP_ERROR_FLAGS_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_ERROR_FLAGS_EN_ADDR(x),m,v,HWIO_QUP_ERROR_FLAGS_EN_IN(x))
#define HWIO_QUP_ERROR_FLAGS_EN_OUTPUT_OVER_RUN_ERR_EN_BMSK                                                0x20
#define HWIO_QUP_ERROR_FLAGS_EN_OUTPUT_OVER_RUN_ERR_EN_SHFT                                                 0x5
#define HWIO_QUP_ERROR_FLAGS_EN_INPUT_UNDER_RUN_ERR_EN_BMSK                                                0x10
#define HWIO_QUP_ERROR_FLAGS_EN_INPUT_UNDER_RUN_ERR_EN_SHFT                                                 0x4
#define HWIO_QUP_ERROR_FLAGS_EN_OUTPUT_UNDER_RUN_ERR_EN_BMSK                                                0x8
#define HWIO_QUP_ERROR_FLAGS_EN_OUTPUT_UNDER_RUN_ERR_EN_SHFT                                                0x3
#define HWIO_QUP_ERROR_FLAGS_EN_INPUT_OVER_RUN_ERR_EN_BMSK                                                  0x4
#define HWIO_QUP_ERROR_FLAGS_EN_INPUT_OVER_RUN_ERR_EN_SHFT                                                  0x2

#define HWIO_QUP_TEST_CTRL_ADDR(x)                                                                   ((x) + 0x00000024)
#define HWIO_QUP_TEST_CTRL_PHYS(x)                                                                   ((x) + 0x00000024)
#define HWIO_QUP_TEST_CTRL_OFFS                                                                      (0x00000024)
#define HWIO_QUP_TEST_CTRL_RMSK                                                                             0x1
#define HWIO_QUP_TEST_CTRL_IN(x)      \
        in_dword_masked(HWIO_QUP_TEST_CTRL_ADDR(x), HWIO_QUP_TEST_CTRL_RMSK)
#define HWIO_QUP_TEST_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_QUP_TEST_CTRL_ADDR(x), m)
#define HWIO_QUP_TEST_CTRL_OUT(x, v)      \
        out_dword(HWIO_QUP_TEST_CTRL_ADDR(x),v)
#define HWIO_QUP_TEST_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_TEST_CTRL_ADDR(x),m,v,HWIO_QUP_TEST_CTRL_IN(x))
#define HWIO_QUP_TEST_CTRL_QUP_TEST_BUS_EN_BMSK                                                             0x1
#define HWIO_QUP_TEST_CTRL_QUP_TEST_BUS_EN_SHFT                                                             0x0

#define HWIO_QUP_OPERATIONAL_MASK_ADDR(x)                                                            ((x) + 0x00000028)
#define HWIO_QUP_OPERATIONAL_MASK_PHYS(x)                                                            ((x) + 0x00000028)
#define HWIO_QUP_OPERATIONAL_MASK_OFFS                                                               (0x00000028)
#define HWIO_QUP_OPERATIONAL_MASK_RMSK                                                                    0x300
#define HWIO_QUP_OPERATIONAL_MASK_IN(x)      \
        in_dword_masked(HWIO_QUP_OPERATIONAL_MASK_ADDR(x), HWIO_QUP_OPERATIONAL_MASK_RMSK)
#define HWIO_QUP_OPERATIONAL_MASK_INM(x, m)      \
        in_dword_masked(HWIO_QUP_OPERATIONAL_MASK_ADDR(x), m)
#define HWIO_QUP_OPERATIONAL_MASK_OUT(x, v)      \
        out_dword(HWIO_QUP_OPERATIONAL_MASK_ADDR(x),v)
#define HWIO_QUP_OPERATIONAL_MASK_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_OPERATIONAL_MASK_ADDR(x),m,v,HWIO_QUP_OPERATIONAL_MASK_IN(x))
#define HWIO_QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_BMSK                                                 0x200
#define HWIO_QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_SHFT                                                   0x9
#define HWIO_QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_BMSK                                                0x100
#define HWIO_QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_SHFT                                                  0x8

#define HWIO_QUP_HW_VERSION_ADDR(x)                                                                  ((x) + 0x00000030)
#define HWIO_QUP_HW_VERSION_PHYS(x)                                                                  ((x) + 0x00000030)
#define HWIO_QUP_HW_VERSION_OFFS                                                                     (0x00000030)
#define HWIO_QUP_HW_VERSION_RMSK                                                                     0xffffffff
#define HWIO_QUP_HW_VERSION_IN(x)      \
        in_dword_masked(HWIO_QUP_HW_VERSION_ADDR(x), HWIO_QUP_HW_VERSION_RMSK)
#define HWIO_QUP_HW_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_QUP_HW_VERSION_ADDR(x), m)
#define HWIO_QUP_HW_VERSION_MAJOR_BMSK                                                               0xf0000000
#define HWIO_QUP_HW_VERSION_MAJOR_SHFT                                                                     0x1c
#define HWIO_QUP_HW_VERSION_MINOR_BMSK                                                                0xfff0000
#define HWIO_QUP_HW_VERSION_MINOR_SHFT                                                                     0x10
#define HWIO_QUP_HW_VERSION_STEP_BMSK                                                                    0xffff
#define HWIO_QUP_HW_VERSION_STEP_SHFT                                                                       0x0

#define HWIO_QUP_MX_OUTPUT_COUNT_ADDR(x)                                                             ((x) + 0x00000100)
#define HWIO_QUP_MX_OUTPUT_COUNT_PHYS(x)                                                             ((x) + 0x00000100)
#define HWIO_QUP_MX_OUTPUT_COUNT_OFFS                                                                (0x00000100)
#define HWIO_QUP_MX_OUTPUT_COUNT_RMSK                                                                0x8000ffff
#define HWIO_QUP_MX_OUTPUT_COUNT_IN(x)      \
        in_dword_masked(HWIO_QUP_MX_OUTPUT_COUNT_ADDR(x), HWIO_QUP_MX_OUTPUT_COUNT_RMSK)
#define HWIO_QUP_MX_OUTPUT_COUNT_INM(x, m)      \
        in_dword_masked(HWIO_QUP_MX_OUTPUT_COUNT_ADDR(x), m)
#define HWIO_QUP_MX_OUTPUT_COUNT_OUT(x, v)      \
        out_dword(HWIO_QUP_MX_OUTPUT_COUNT_ADDR(x),v)
#define HWIO_QUP_MX_OUTPUT_COUNT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_MX_OUTPUT_COUNT_ADDR(x),m,v,HWIO_QUP_MX_OUTPUT_COUNT_IN(x))
#define HWIO_QUP_MX_OUTPUT_COUNT_MX_CONFIG_DURING_RUN_BMSK                                           0x80000000
#define HWIO_QUP_MX_OUTPUT_COUNT_MX_CONFIG_DURING_RUN_SHFT                                                 0x1f
#define HWIO_QUP_MX_OUTPUT_COUNT_MX_OUTPUT_COUNT_BMSK                                                    0xffff
#define HWIO_QUP_MX_OUTPUT_COUNT_MX_OUTPUT_COUNT_SHFT                                                       0x0

#define HWIO_QUP_MX_OUTPUT_CNT_CURRENT_ADDR(x)                                                       ((x) + 0x00000104)
#define HWIO_QUP_MX_OUTPUT_CNT_CURRENT_PHYS(x)                                                       ((x) + 0x00000104)
#define HWIO_QUP_MX_OUTPUT_CNT_CURRENT_OFFS                                                          (0x00000104)
#define HWIO_QUP_MX_OUTPUT_CNT_CURRENT_RMSK                                                              0xffff
#define HWIO_QUP_MX_OUTPUT_CNT_CURRENT_IN(x)      \
        in_dword_masked(HWIO_QUP_MX_OUTPUT_CNT_CURRENT_ADDR(x), HWIO_QUP_MX_OUTPUT_CNT_CURRENT_RMSK)
#define HWIO_QUP_MX_OUTPUT_CNT_CURRENT_INM(x, m)      \
        in_dword_masked(HWIO_QUP_MX_OUTPUT_CNT_CURRENT_ADDR(x), m)
#define HWIO_QUP_MX_OUTPUT_CNT_CURRENT_MX_OUTPUT_CNT_CURRENT_BMSK                                        0xffff
#define HWIO_QUP_MX_OUTPUT_CNT_CURRENT_MX_OUTPUT_CNT_CURRENT_SHFT                                           0x0

#define HWIO_QUP_OUTPUT_DEBUG_ADDR(x)                                                                ((x) + 0x00000108)
#define HWIO_QUP_OUTPUT_DEBUG_PHYS(x)                                                                ((x) + 0x00000108)
#define HWIO_QUP_OUTPUT_DEBUG_OFFS                                                                   (0x00000108)
#define HWIO_QUP_OUTPUT_DEBUG_RMSK                                                                   0xffffffff
#define HWIO_QUP_OUTPUT_DEBUG_IN(x)      \
        in_dword_masked(HWIO_QUP_OUTPUT_DEBUG_ADDR(x), HWIO_QUP_OUTPUT_DEBUG_RMSK)
#define HWIO_QUP_OUTPUT_DEBUG_INM(x, m)      \
        in_dword_masked(HWIO_QUP_OUTPUT_DEBUG_ADDR(x), m)
#define HWIO_QUP_OUTPUT_DEBUG_OUTPUT_DEBUG_DATA_BMSK                                                 0xffffffff
#define HWIO_QUP_OUTPUT_DEBUG_OUTPUT_DEBUG_DATA_SHFT                                                        0x0

#define HWIO_QUP_OUTPUT_FIFO_WORD_CNT_ADDR(x)                                                        ((x) + 0x0000010c)
#define HWIO_QUP_OUTPUT_FIFO_WORD_CNT_PHYS(x)                                                        ((x) + 0x0000010c)
#define HWIO_QUP_OUTPUT_FIFO_WORD_CNT_OFFS                                                           (0x0000010c)
#define HWIO_QUP_OUTPUT_FIFO_WORD_CNT_RMSK                                                                0x1ff
#define HWIO_QUP_OUTPUT_FIFO_WORD_CNT_IN(x)      \
        in_dword_masked(HWIO_QUP_OUTPUT_FIFO_WORD_CNT_ADDR(x), HWIO_QUP_OUTPUT_FIFO_WORD_CNT_RMSK)
#define HWIO_QUP_OUTPUT_FIFO_WORD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_QUP_OUTPUT_FIFO_WORD_CNT_ADDR(x), m)
#define HWIO_QUP_OUTPUT_FIFO_WORD_CNT_OUTPUT_FIFO_WORD_CNT_BMSK                                           0x1ff
#define HWIO_QUP_OUTPUT_FIFO_WORD_CNT_OUTPUT_FIFO_WORD_CNT_SHFT                                             0x0

#define HWIO_QUP_OUTPUT_FIFOc_ADDR(base,c)                                                           ((base) + 0x00000110 + 0x4 * (c))
#define HWIO_QUP_OUTPUT_FIFOc_PHYS(base,c)                                                           ((base) + 0x00000110 + 0x4 * (c))
#define HWIO_QUP_OUTPUT_FIFOc_OFFS(base,c)                                                           (0x00000110 + 0x4 * (c))
#define HWIO_QUP_OUTPUT_FIFOc_RMSK                                                                   0xffffffff
#define HWIO_QUP_OUTPUT_FIFOc_MAXc                                                                           15
#define HWIO_QUP_OUTPUT_FIFOc_OUTI(base,c,val)    \
        out_dword(HWIO_QUP_OUTPUT_FIFOc_ADDR(base,c),val)
#define HWIO_QUP_OUTPUT_FIFOc_OUTPUT_BMSK                                                            0xffffffff
#define HWIO_QUP_OUTPUT_FIFOc_OUTPUT_SHFT                                                                   0x0

#define HWIO_QUP_MX_WRITE_COUNT_ADDR(x)                                                              ((x) + 0x00000150)
#define HWIO_QUP_MX_WRITE_COUNT_PHYS(x)                                                              ((x) + 0x00000150)
#define HWIO_QUP_MX_WRITE_COUNT_OFFS                                                                 (0x00000150)
#define HWIO_QUP_MX_WRITE_COUNT_RMSK                                                                 0x8000ffff
#define HWIO_QUP_MX_WRITE_COUNT_IN(x)      \
        in_dword_masked(HWIO_QUP_MX_WRITE_COUNT_ADDR(x), HWIO_QUP_MX_WRITE_COUNT_RMSK)
#define HWIO_QUP_MX_WRITE_COUNT_INM(x, m)      \
        in_dword_masked(HWIO_QUP_MX_WRITE_COUNT_ADDR(x), m)
#define HWIO_QUP_MX_WRITE_COUNT_OUT(x, v)      \
        out_dword(HWIO_QUP_MX_WRITE_COUNT_ADDR(x),v)
#define HWIO_QUP_MX_WRITE_COUNT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_MX_WRITE_COUNT_ADDR(x),m,v,HWIO_QUP_MX_WRITE_COUNT_IN(x))
#define HWIO_QUP_MX_WRITE_COUNT_MX_CONFIG_DURING_RUN_BMSK                                            0x80000000
#define HWIO_QUP_MX_WRITE_COUNT_MX_CONFIG_DURING_RUN_SHFT                                                  0x1f
#define HWIO_QUP_MX_WRITE_COUNT_MX_WRITE_COUNT_BMSK                                                      0xffff
#define HWIO_QUP_MX_WRITE_COUNT_MX_WRITE_COUNT_SHFT                                                         0x0

#define HWIO_QUP_MX_WRITE_CNT_CURRENT_ADDR(x)                                                        ((x) + 0x00000154)
#define HWIO_QUP_MX_WRITE_CNT_CURRENT_PHYS(x)                                                        ((x) + 0x00000154)
#define HWIO_QUP_MX_WRITE_CNT_CURRENT_OFFS                                                           (0x00000154)
#define HWIO_QUP_MX_WRITE_CNT_CURRENT_RMSK                                                               0xffff
#define HWIO_QUP_MX_WRITE_CNT_CURRENT_IN(x)      \
        in_dword_masked(HWIO_QUP_MX_WRITE_CNT_CURRENT_ADDR(x), HWIO_QUP_MX_WRITE_CNT_CURRENT_RMSK)
#define HWIO_QUP_MX_WRITE_CNT_CURRENT_INM(x, m)      \
        in_dword_masked(HWIO_QUP_MX_WRITE_CNT_CURRENT_ADDR(x), m)
#define HWIO_QUP_MX_WRITE_CNT_CURRENT_MX_WRITE_CNT_CURRENT_BMSK                                          0xffff
#define HWIO_QUP_MX_WRITE_CNT_CURRENT_MX_WRITE_CNT_CURRENT_SHFT                                             0x0

#define HWIO_QUP_MX_INPUT_COUNT_ADDR(x)                                                              ((x) + 0x00000200)
#define HWIO_QUP_MX_INPUT_COUNT_PHYS(x)                                                              ((x) + 0x00000200)
#define HWIO_QUP_MX_INPUT_COUNT_OFFS                                                                 (0x00000200)
#define HWIO_QUP_MX_INPUT_COUNT_RMSK                                                                 0x8000ffff
#define HWIO_QUP_MX_INPUT_COUNT_IN(x)      \
        in_dword_masked(HWIO_QUP_MX_INPUT_COUNT_ADDR(x), HWIO_QUP_MX_INPUT_COUNT_RMSK)
#define HWIO_QUP_MX_INPUT_COUNT_INM(x, m)      \
        in_dword_masked(HWIO_QUP_MX_INPUT_COUNT_ADDR(x), m)
#define HWIO_QUP_MX_INPUT_COUNT_OUT(x, v)      \
        out_dword(HWIO_QUP_MX_INPUT_COUNT_ADDR(x),v)
#define HWIO_QUP_MX_INPUT_COUNT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_MX_INPUT_COUNT_ADDR(x),m,v,HWIO_QUP_MX_INPUT_COUNT_IN(x))
#define HWIO_QUP_MX_INPUT_COUNT_MX_CONFIG_DURING_RUN_BMSK                                            0x80000000
#define HWIO_QUP_MX_INPUT_COUNT_MX_CONFIG_DURING_RUN_SHFT                                                  0x1f
#define HWIO_QUP_MX_INPUT_COUNT_MX_INPUT_COUNT_BMSK                                                      0xffff
#define HWIO_QUP_MX_INPUT_COUNT_MX_INPUT_COUNT_SHFT                                                         0x0

#define HWIO_QUP_MX_INPUT_CNT_CURRENT_ADDR(x)                                                        ((x) + 0x00000204)
#define HWIO_QUP_MX_INPUT_CNT_CURRENT_PHYS(x)                                                        ((x) + 0x00000204)
#define HWIO_QUP_MX_INPUT_CNT_CURRENT_OFFS                                                           (0x00000204)
#define HWIO_QUP_MX_INPUT_CNT_CURRENT_RMSK                                                               0xffff
#define HWIO_QUP_MX_INPUT_CNT_CURRENT_IN(x)      \
        in_dword_masked(HWIO_QUP_MX_INPUT_CNT_CURRENT_ADDR(x), HWIO_QUP_MX_INPUT_CNT_CURRENT_RMSK)
#define HWIO_QUP_MX_INPUT_CNT_CURRENT_INM(x, m)      \
        in_dword_masked(HWIO_QUP_MX_INPUT_CNT_CURRENT_ADDR(x), m)
#define HWIO_QUP_MX_INPUT_CNT_CURRENT_MX_INPUT_CNT_CURRENT_BMSK                                          0xffff
#define HWIO_QUP_MX_INPUT_CNT_CURRENT_MX_INPUT_CNT_CURRENT_SHFT                                             0x0

#define HWIO_QUP_MX_READ_COUNT_ADDR(x)                                                               ((x) + 0x00000208)
#define HWIO_QUP_MX_READ_COUNT_PHYS(x)                                                               ((x) + 0x00000208)
#define HWIO_QUP_MX_READ_COUNT_OFFS                                                                  (0x00000208)
#define HWIO_QUP_MX_READ_COUNT_RMSK                                                                  0x8000ffff
#define HWIO_QUP_MX_READ_COUNT_IN(x)      \
        in_dword_masked(HWIO_QUP_MX_READ_COUNT_ADDR(x), HWIO_QUP_MX_READ_COUNT_RMSK)
#define HWIO_QUP_MX_READ_COUNT_INM(x, m)      \
        in_dword_masked(HWIO_QUP_MX_READ_COUNT_ADDR(x), m)
#define HWIO_QUP_MX_READ_COUNT_OUT(x, v)      \
        out_dword(HWIO_QUP_MX_READ_COUNT_ADDR(x),v)
#define HWIO_QUP_MX_READ_COUNT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QUP_MX_READ_COUNT_ADDR(x),m,v,HWIO_QUP_MX_READ_COUNT_IN(x))
#define HWIO_QUP_MX_READ_COUNT_MX_CONFIG_DURING_RUN_BMSK                                             0x80000000
#define HWIO_QUP_MX_READ_COUNT_MX_CONFIG_DURING_RUN_SHFT                                                   0x1f
#define HWIO_QUP_MX_READ_COUNT_MX_READ_COUNT_BMSK                                                        0xffff
#define HWIO_QUP_MX_READ_COUNT_MX_READ_COUNT_SHFT                                                           0x0

#define HWIO_QUP_MX_READ_CNT_CURRENT_ADDR(x)                                                         ((x) + 0x0000020c)
#define HWIO_QUP_MX_READ_CNT_CURRENT_PHYS(x)                                                         ((x) + 0x0000020c)
#define HWIO_QUP_MX_READ_CNT_CURRENT_OFFS                                                            (0x0000020c)
#define HWIO_QUP_MX_READ_CNT_CURRENT_RMSK                                                                0xffff
#define HWIO_QUP_MX_READ_CNT_CURRENT_IN(x)      \
        in_dword_masked(HWIO_QUP_MX_READ_CNT_CURRENT_ADDR(x), HWIO_QUP_MX_READ_CNT_CURRENT_RMSK)
#define HWIO_QUP_MX_READ_CNT_CURRENT_INM(x, m)      \
        in_dword_masked(HWIO_QUP_MX_READ_CNT_CURRENT_ADDR(x), m)
#define HWIO_QUP_MX_READ_CNT_CURRENT_MX_READ_CNT_CURRENT_BMSK                                            0xffff
#define HWIO_QUP_MX_READ_CNT_CURRENT_MX_READ_CNT_CURRENT_SHFT                                               0x0

#define HWIO_QUP_INPUT_DEBUG_ADDR(x)                                                                 ((x) + 0x00000210)
#define HWIO_QUP_INPUT_DEBUG_PHYS(x)                                                                 ((x) + 0x00000210)
#define HWIO_QUP_INPUT_DEBUG_OFFS                                                                    (0x00000210)
#define HWIO_QUP_INPUT_DEBUG_RMSK                                                                    0xffffffff
#define HWIO_QUP_INPUT_DEBUG_IN(x)      \
        in_dword_masked(HWIO_QUP_INPUT_DEBUG_ADDR(x), HWIO_QUP_INPUT_DEBUG_RMSK)
#define HWIO_QUP_INPUT_DEBUG_INM(x, m)      \
        in_dword_masked(HWIO_QUP_INPUT_DEBUG_ADDR(x), m)
#define HWIO_QUP_INPUT_DEBUG_INPUT_DEBUG_DATA_BMSK                                                   0xffffffff
#define HWIO_QUP_INPUT_DEBUG_INPUT_DEBUG_DATA_SHFT                                                          0x0

#define HWIO_QUP_INPUT_FIFO_WORD_CNT_ADDR(x)                                                         ((x) + 0x00000214)
#define HWIO_QUP_INPUT_FIFO_WORD_CNT_PHYS(x)                                                         ((x) + 0x00000214)
#define HWIO_QUP_INPUT_FIFO_WORD_CNT_OFFS                                                            (0x00000214)
#define HWIO_QUP_INPUT_FIFO_WORD_CNT_RMSK                                                                 0x1ff
#define HWIO_QUP_INPUT_FIFO_WORD_CNT_IN(x)      \
        in_dword_masked(HWIO_QUP_INPUT_FIFO_WORD_CNT_ADDR(x), HWIO_QUP_INPUT_FIFO_WORD_CNT_RMSK)
#define HWIO_QUP_INPUT_FIFO_WORD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_QUP_INPUT_FIFO_WORD_CNT_ADDR(x), m)
#define HWIO_QUP_INPUT_FIFO_WORD_CNT_INPUT_FIFO_WORD_CNT_BMSK                                             0x1ff
#define HWIO_QUP_INPUT_FIFO_WORD_CNT_INPUT_FIFO_WORD_CNT_SHFT                                               0x0

#define HWIO_QUP_INPUT_FIFOc_ADDR(base,c)                                                            ((base) + 0x00000218 + 0x4 * (c))
#define HWIO_QUP_INPUT_FIFOc_PHYS(base,c)                                                            ((base) + 0x00000218 + 0x4 * (c))
#define HWIO_QUP_INPUT_FIFOc_OFFS(base,c)                                                            (0x00000218 + 0x4 * (c))
#define HWIO_QUP_INPUT_FIFOc_RMSK                                                                    0xffffffff
#define HWIO_QUP_INPUT_FIFOc_MAXc                                                                            15
#define HWIO_QUP_INPUT_FIFOc_INI(base,c)        \
        in_dword_masked(HWIO_QUP_INPUT_FIFOc_ADDR(base,c), HWIO_QUP_INPUT_FIFOc_RMSK)
#define HWIO_QUP_INPUT_FIFOc_INMI(base,c,mask)    \
        in_dword_masked(HWIO_QUP_INPUT_FIFOc_ADDR(base,c), mask)
#define HWIO_QUP_INPUT_FIFOc_INPUT_BMSK                                                              0xffffffff
#define HWIO_QUP_INPUT_FIFOc_INPUT_SHFT                                                                     0x0

#define HWIO_SPI_CONFIG_ADDR(x)                                                                      ((x) + 0x00000300)
#define HWIO_SPI_CONFIG_PHYS(x)                                                                      ((x) + 0x00000300)
#define HWIO_SPI_CONFIG_OFFS                                                                         (0x00000300)
#define HWIO_SPI_CONFIG_RMSK                                                                              0x720
#define HWIO_SPI_CONFIG_IN(x)      \
        in_dword_masked(HWIO_SPI_CONFIG_ADDR(x), HWIO_SPI_CONFIG_RMSK)
#define HWIO_SPI_CONFIG_INM(x, m)      \
        in_dword_masked(HWIO_SPI_CONFIG_ADDR(x), m)
#define HWIO_SPI_CONFIG_OUT(x, v)      \
        out_dword(HWIO_SPI_CONFIG_ADDR(x),v)
#define HWIO_SPI_CONFIG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_CONFIG_ADDR(x),m,v,HWIO_SPI_CONFIG_IN(x))
#define HWIO_SPI_CONFIG_HS_MODE_BMSK                                                                      0x400
#define HWIO_SPI_CONFIG_HS_MODE_SHFT                                                                        0xa
#define HWIO_SPI_CONFIG_INPUT_FIRST_BMSK                                                                  0x200
#define HWIO_SPI_CONFIG_INPUT_FIRST_SHFT                                                                    0x9
#define HWIO_SPI_CONFIG_LOOP_BACK_BMSK                                                                    0x100
#define HWIO_SPI_CONFIG_LOOP_BACK_SHFT                                                                      0x8
#define HWIO_SPI_CONFIG_SLAVE_OPERATION_BMSK                                                               0x20
#define HWIO_SPI_CONFIG_SLAVE_OPERATION_SHFT                                                                0x5

#define HWIO_SPI_IO_CONTROL_ADDR(x)                                                                  ((x) + 0x00000304)
#define HWIO_SPI_IO_CONTROL_PHYS(x)                                                                  ((x) + 0x00000304)
#define HWIO_SPI_IO_CONTROL_OFFS                                                                     (0x00000304)
#define HWIO_SPI_IO_CONTROL_RMSK                                                                          0xfff
#define HWIO_SPI_IO_CONTROL_IN(x)      \
        in_dword_masked(HWIO_SPI_IO_CONTROL_ADDR(x), HWIO_SPI_IO_CONTROL_RMSK)
#define HWIO_SPI_IO_CONTROL_INM(x, m)      \
        in_dword_masked(HWIO_SPI_IO_CONTROL_ADDR(x), m)
#define HWIO_SPI_IO_CONTROL_OUT(x, v)      \
        out_dword(HWIO_SPI_IO_CONTROL_ADDR(x),v)
#define HWIO_SPI_IO_CONTROL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_IO_CONTROL_ADDR(x),m,v,HWIO_SPI_IO_CONTROL_IN(x))
#define HWIO_SPI_IO_CONTROL_FORCE_CS_BMSK                                                                 0x800
#define HWIO_SPI_IO_CONTROL_FORCE_CS_SHFT                                                                   0xb
#define HWIO_SPI_IO_CONTROL_CLK_IDLE_HIGH_BMSK                                                            0x400
#define HWIO_SPI_IO_CONTROL_CLK_IDLE_HIGH_SHFT                                                              0xa
#define HWIO_SPI_IO_CONTROL_CLK_ALWAYS_ON_BMSK                                                            0x200
#define HWIO_SPI_IO_CONTROL_CLK_ALWAYS_ON_SHFT                                                              0x9
#define HWIO_SPI_IO_CONTROL_MX_CS_MODE_BMSK                                                               0x100
#define HWIO_SPI_IO_CONTROL_MX_CS_MODE_SHFT                                                                 0x8
#define HWIO_SPI_IO_CONTROL_CS_N_POLARITY_BMSK                                                             0xf0
#define HWIO_SPI_IO_CONTROL_CS_N_POLARITY_SHFT                                                              0x4
#define HWIO_SPI_IO_CONTROL_CS_SELECT_BMSK                                                                  0xc
#define HWIO_SPI_IO_CONTROL_CS_SELECT_SHFT                                                                  0x2
#define HWIO_SPI_IO_CONTROL_TRISTATE_CS_BMSK                                                                0x2
#define HWIO_SPI_IO_CONTROL_TRISTATE_CS_SHFT                                                                0x1
#define HWIO_SPI_IO_CONTROL_NO_TRI_STATE_BMSK                                                               0x1
#define HWIO_SPI_IO_CONTROL_NO_TRI_STATE_SHFT                                                               0x0

#define HWIO_SPI_ERROR_FLAGS_ADDR(x)                                                                 ((x) + 0x00000308)
#define HWIO_SPI_ERROR_FLAGS_PHYS(x)                                                                 ((x) + 0x00000308)
#define HWIO_SPI_ERROR_FLAGS_OFFS                                                                    (0x00000308)
#define HWIO_SPI_ERROR_FLAGS_RMSK                                                                           0x3
#define HWIO_SPI_ERROR_FLAGS_IN(x)      \
        in_dword_masked(HWIO_SPI_ERROR_FLAGS_ADDR(x), HWIO_SPI_ERROR_FLAGS_RMSK)
#define HWIO_SPI_ERROR_FLAGS_INM(x, m)      \
        in_dword_masked(HWIO_SPI_ERROR_FLAGS_ADDR(x), m)
#define HWIO_SPI_ERROR_FLAGS_OUT(x, v)      \
        out_dword(HWIO_SPI_ERROR_FLAGS_ADDR(x),v)
#define HWIO_SPI_ERROR_FLAGS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_ERROR_FLAGS_ADDR(x),m,v,HWIO_SPI_ERROR_FLAGS_IN(x))
#define HWIO_SPI_ERROR_FLAGS_SPI_SLV_CLK_OVER_RUN_ERR_BMSK                                                  0x2
#define HWIO_SPI_ERROR_FLAGS_SPI_SLV_CLK_OVER_RUN_ERR_SHFT                                                  0x1
#define HWIO_SPI_ERROR_FLAGS_SPI_SLV_CLK_UNDER_RUN_ERR_BMSK                                                 0x1
#define HWIO_SPI_ERROR_FLAGS_SPI_SLV_CLK_UNDER_RUN_ERR_SHFT                                                 0x0

#define HWIO_SPI_ERROR_FLAGS_EN_ADDR(x)                                                              ((x) + 0x0000030c)
#define HWIO_SPI_ERROR_FLAGS_EN_PHYS(x)                                                              ((x) + 0x0000030c)
#define HWIO_SPI_ERROR_FLAGS_EN_OFFS                                                                 (0x0000030c)
#define HWIO_SPI_ERROR_FLAGS_EN_RMSK                                                                        0x3
#define HWIO_SPI_ERROR_FLAGS_EN_IN(x)      \
        in_dword_masked(HWIO_SPI_ERROR_FLAGS_EN_ADDR(x), HWIO_SPI_ERROR_FLAGS_EN_RMSK)
#define HWIO_SPI_ERROR_FLAGS_EN_INM(x, m)      \
        in_dword_masked(HWIO_SPI_ERROR_FLAGS_EN_ADDR(x), m)
#define HWIO_SPI_ERROR_FLAGS_EN_OUT(x, v)      \
        out_dword(HWIO_SPI_ERROR_FLAGS_EN_ADDR(x),v)
#define HWIO_SPI_ERROR_FLAGS_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_ERROR_FLAGS_EN_ADDR(x),m,v,HWIO_SPI_ERROR_FLAGS_EN_IN(x))
#define HWIO_SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_OVER_RUN_ERR_EN_BMSK                                            0x2
#define HWIO_SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_OVER_RUN_ERR_EN_SHFT                                            0x1
#define HWIO_SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_UNDER_RUN_ERR_EN_BMSK                                           0x1
#define HWIO_SPI_ERROR_FLAGS_EN_SPI_SLV_CLK_UNDER_RUN_ERR_EN_SHFT                                           0x0

#define HWIO_SPI_DEASSERT_WAIT_ADDR(x)                                                               ((x) + 0x00000310)
#define HWIO_SPI_DEASSERT_WAIT_PHYS(x)                                                               ((x) + 0x00000310)
#define HWIO_SPI_DEASSERT_WAIT_OFFS                                                                  (0x00000310)
#define HWIO_SPI_DEASSERT_WAIT_RMSK                                                                        0x3f
#define HWIO_SPI_DEASSERT_WAIT_IN(x)      \
        in_dword_masked(HWIO_SPI_DEASSERT_WAIT_ADDR(x), HWIO_SPI_DEASSERT_WAIT_RMSK)
#define HWIO_SPI_DEASSERT_WAIT_INM(x, m)      \
        in_dword_masked(HWIO_SPI_DEASSERT_WAIT_ADDR(x), m)
#define HWIO_SPI_DEASSERT_WAIT_OUT(x, v)      \
        out_dword(HWIO_SPI_DEASSERT_WAIT_ADDR(x),v)
#define HWIO_SPI_DEASSERT_WAIT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_DEASSERT_WAIT_ADDR(x),m,v,HWIO_SPI_DEASSERT_WAIT_IN(x))
#define HWIO_SPI_DEASSERT_WAIT_DEASSERT_WAIT_BMSK                                                          0x3f
#define HWIO_SPI_DEASSERT_WAIT_DEASSERT_WAIT_SHFT                                                           0x0

#define HWIO_SPI_CHAR_CFG_ADDR(x)                                                                    ((x) + 0x00000320)
#define HWIO_SPI_CHAR_CFG_PHYS(x)                                                                    ((x) + 0x00000320)
#define HWIO_SPI_CHAR_CFG_OFFS                                                                       (0x00000320)
#define HWIO_SPI_CHAR_CFG_RMSK                                                                            0xf11
#define HWIO_SPI_CHAR_CFG_IN(x)      \
        in_dword_masked(HWIO_SPI_CHAR_CFG_ADDR(x), HWIO_SPI_CHAR_CFG_RMSK)
#define HWIO_SPI_CHAR_CFG_INM(x, m)      \
        in_dword_masked(HWIO_SPI_CHAR_CFG_ADDR(x), m)
#define HWIO_SPI_CHAR_CFG_OUT(x, v)      \
        out_dword(HWIO_SPI_CHAR_CFG_ADDR(x),v)
#define HWIO_SPI_CHAR_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_CHAR_CFG_ADDR(x),m,v,HWIO_SPI_CHAR_CFG_IN(x))
#define HWIO_SPI_CHAR_CFG_CHAR_STATUS_BMSK                                                                0xf00
#define HWIO_SPI_CHAR_CFG_CHAR_STATUS_SHFT                                                                  0x8
#define HWIO_SPI_CHAR_CFG_DIRECTION_BMSK                                                                   0x10
#define HWIO_SPI_CHAR_CFG_DIRECTION_SHFT                                                                    0x4
#define HWIO_SPI_CHAR_CFG_ENABLE_BMSK                                                                       0x1
#define HWIO_SPI_CHAR_CFG_ENABLE_SHFT                                                                       0x0

#define HWIO_SPI_CHAR_DATA_SPI_CS_ADDR(x)                                                            ((x) + 0x00000324)
#define HWIO_SPI_CHAR_DATA_SPI_CS_PHYS(x)                                                            ((x) + 0x00000324)
#define HWIO_SPI_CHAR_DATA_SPI_CS_OFFS                                                               (0x00000324)
#define HWIO_SPI_CHAR_DATA_SPI_CS_RMSK                                                                    0x3ff
#define HWIO_SPI_CHAR_DATA_SPI_CS_IN(x)      \
        in_dword_masked(HWIO_SPI_CHAR_DATA_SPI_CS_ADDR(x), HWIO_SPI_CHAR_DATA_SPI_CS_RMSK)
#define HWIO_SPI_CHAR_DATA_SPI_CS_INM(x, m)      \
        in_dword_masked(HWIO_SPI_CHAR_DATA_SPI_CS_ADDR(x), m)
#define HWIO_SPI_CHAR_DATA_SPI_CS_OUT(x, v)      \
        out_dword(HWIO_SPI_CHAR_DATA_SPI_CS_ADDR(x),v)
#define HWIO_SPI_CHAR_DATA_SPI_CS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_CHAR_DATA_SPI_CS_ADDR(x),m,v,HWIO_SPI_CHAR_DATA_SPI_CS_IN(x))
#define HWIO_SPI_CHAR_DATA_SPI_CS_CHAR_MODE_BMSK                                                          0x300
#define HWIO_SPI_CHAR_DATA_SPI_CS_CHAR_MODE_SHFT                                                            0x8
#define HWIO_SPI_CHAR_DATA_SPI_CS_DOUT_DATA_DIN_EXP_BMSK                                                   0xff
#define HWIO_SPI_CHAR_DATA_SPI_CS_DOUT_DATA_DIN_EXP_SHFT                                                    0x0

#define HWIO_SPI_CHAR_DATA_SPI_MOSI_ADDR(x)                                                          ((x) + 0x00000328)
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_PHYS(x)                                                          ((x) + 0x00000328)
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_OFFS                                                             (0x00000328)
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_RMSK                                                                  0x3ff
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_IN(x)      \
        in_dword_masked(HWIO_SPI_CHAR_DATA_SPI_MOSI_ADDR(x), HWIO_SPI_CHAR_DATA_SPI_MOSI_RMSK)
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_INM(x, m)      \
        in_dword_masked(HWIO_SPI_CHAR_DATA_SPI_MOSI_ADDR(x), m)
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_OUT(x, v)      \
        out_dword(HWIO_SPI_CHAR_DATA_SPI_MOSI_ADDR(x),v)
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_CHAR_DATA_SPI_MOSI_ADDR(x),m,v,HWIO_SPI_CHAR_DATA_SPI_MOSI_IN(x))
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_CHAR_MODE_BMSK                                                        0x300
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_CHAR_MODE_SHFT                                                          0x8
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_DOUT_DATA_DIN_EXP_BMSK                                                 0xff
#define HWIO_SPI_CHAR_DATA_SPI_MOSI_DOUT_DATA_DIN_EXP_SHFT                                                  0x0

#define HWIO_SPI_CHAR_DATA_SPI_MISO_ADDR(x)                                                          ((x) + 0x0000032c)
#define HWIO_SPI_CHAR_DATA_SPI_MISO_PHYS(x)                                                          ((x) + 0x0000032c)
#define HWIO_SPI_CHAR_DATA_SPI_MISO_OFFS                                                             (0x0000032c)
#define HWIO_SPI_CHAR_DATA_SPI_MISO_RMSK                                                                  0x7ff
#define HWIO_SPI_CHAR_DATA_SPI_MISO_IN(x)      \
        in_dword_masked(HWIO_SPI_CHAR_DATA_SPI_MISO_ADDR(x), HWIO_SPI_CHAR_DATA_SPI_MISO_RMSK)
#define HWIO_SPI_CHAR_DATA_SPI_MISO_INM(x, m)      \
        in_dword_masked(HWIO_SPI_CHAR_DATA_SPI_MISO_ADDR(x), m)
#define HWIO_SPI_CHAR_DATA_SPI_MISO_OUT(x, v)      \
        out_dword(HWIO_SPI_CHAR_DATA_SPI_MISO_ADDR(x),v)
#define HWIO_SPI_CHAR_DATA_SPI_MISO_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SPI_CHAR_DATA_SPI_MISO_ADDR(x),m,v,HWIO_SPI_CHAR_DATA_SPI_MISO_IN(x))
#define HWIO_SPI_CHAR_DATA_SPI_MISO_CHAR_MODE_BMSK                                                        0x700
#define HWIO_SPI_CHAR_DATA_SPI_MISO_CHAR_MODE_SHFT                                                          0x8
#define HWIO_SPI_CHAR_DATA_SPI_MISO_DOUT_DATA_DIN_EXP_BMSK                                                 0xff
#define HWIO_SPI_CHAR_DATA_SPI_MISO_DOUT_DATA_DIN_EXP_SHFT                                                  0x0

#define HWIO_I2C_MASTER_CLK_CTL_ADDR(x)                                                              ((x) + 0x00000400)
#define HWIO_I2C_MASTER_CLK_CTL_PHYS(x)                                                              ((x) + 0x00000400)
#define HWIO_I2C_MASTER_CLK_CTL_OFFS                                                                 (0x00000400)
#define HWIO_I2C_MASTER_CLK_CTL_RMSK                                                                 0x1fff07ff
#define HWIO_I2C_MASTER_CLK_CTL_IN(x)      \
        in_dword_masked(HWIO_I2C_MASTER_CLK_CTL_ADDR(x), HWIO_I2C_MASTER_CLK_CTL_RMSK)
#define HWIO_I2C_MASTER_CLK_CTL_INM(x, m)      \
        in_dword_masked(HWIO_I2C_MASTER_CLK_CTL_ADDR(x), m)
#define HWIO_I2C_MASTER_CLK_CTL_OUT(x, v)      \
        out_dword(HWIO_I2C_MASTER_CLK_CTL_ADDR(x),v)
#define HWIO_I2C_MASTER_CLK_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I2C_MASTER_CLK_CTL_ADDR(x),m,v,HWIO_I2C_MASTER_CLK_CTL_IN(x))
#define HWIO_I2C_MASTER_CLK_CTL_SCL_EXT_FORCE_LOW_BMSK                                               0x10000000
#define HWIO_I2C_MASTER_CLK_CTL_SCL_EXT_FORCE_LOW_SHFT                                                     0x1c
#define HWIO_I2C_MASTER_CLK_CTL_SDA_NOISE_REJECTION_BMSK                                              0xc000000
#define HWIO_I2C_MASTER_CLK_CTL_SDA_NOISE_REJECTION_SHFT                                                   0x1a
#define HWIO_I2C_MASTER_CLK_CTL_SCL_NOISE_REJECTION_BMSK                                              0x3000000
#define HWIO_I2C_MASTER_CLK_CTL_SCL_NOISE_REJECTION_SHFT                                                   0x18
#define HWIO_I2C_MASTER_CLK_CTL_HIGH_TIME_DIVIDER_VALUE_BMSK                                           0xff0000
#define HWIO_I2C_MASTER_CLK_CTL_HIGH_TIME_DIVIDER_VALUE_SHFT                                               0x10
#define HWIO_I2C_MASTER_CLK_CTL_HS_DIVIDER_VALUE_BMSK                                                     0x700
#define HWIO_I2C_MASTER_CLK_CTL_HS_DIVIDER_VALUE_SHFT                                                       0x8
#define HWIO_I2C_MASTER_CLK_CTL_FS_DIVIDER_VALUE_BMSK                                                      0xff
#define HWIO_I2C_MASTER_CLK_CTL_FS_DIVIDER_VALUE_SHFT                                                       0x0

#define HWIO_I2C_MASTER_STATUS_ADDR(x)                                                               ((x) + 0x00000404)
#define HWIO_I2C_MASTER_STATUS_PHYS(x)                                                               ((x) + 0x00000404)
#define HWIO_I2C_MASTER_STATUS_OFFS                                                                  (0x00000404)
#define HWIO_I2C_MASTER_STATUS_RMSK                                                                  0x3ffffffc
#define HWIO_I2C_MASTER_STATUS_IN(x)      \
        in_dword_masked(HWIO_I2C_MASTER_STATUS_ADDR(x), HWIO_I2C_MASTER_STATUS_RMSK)
#define HWIO_I2C_MASTER_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_I2C_MASTER_STATUS_ADDR(x), m)
#define HWIO_I2C_MASTER_STATUS_OUT(x, v)      \
        out_dword(HWIO_I2C_MASTER_STATUS_ADDR(x),v)
#define HWIO_I2C_MASTER_STATUS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I2C_MASTER_STATUS_ADDR(x),m,v,HWIO_I2C_MASTER_STATUS_IN(x))
#define HWIO_I2C_MASTER_STATUS_TRANSFER_CANCEL_DONE_BMSK                                             0x20000000
#define HWIO_I2C_MASTER_STATUS_TRANSFER_CANCEL_DONE_SHFT                                                   0x1d
#define HWIO_I2C_MASTER_STATUS_TRANSFER_CANCEL_ID_MATCH_BMSK                                         0x10000000
#define HWIO_I2C_MASTER_STATUS_TRANSFER_CANCEL_ID_MATCH_SHFT                                               0x1c
#define HWIO_I2C_MASTER_STATUS_I2C_SCL_BMSK                                                           0x8000000
#define HWIO_I2C_MASTER_STATUS_I2C_SCL_SHFT                                                                0x1b
#define HWIO_I2C_MASTER_STATUS_I2C_SDA_BMSK                                                           0x4000000
#define HWIO_I2C_MASTER_STATUS_I2C_SDA_SHFT                                                                0x1a
#define HWIO_I2C_MASTER_STATUS_INVALID_READ_SEQ_BMSK                                                  0x2000000
#define HWIO_I2C_MASTER_STATUS_INVALID_READ_SEQ_SHFT                                                       0x19
#define HWIO_I2C_MASTER_STATUS_INVALID_READ_ADDR_BMSK                                                 0x1000000
#define HWIO_I2C_MASTER_STATUS_INVALID_READ_ADDR_SHFT                                                      0x18
#define HWIO_I2C_MASTER_STATUS_INVALID_TAG_BMSK                                                        0x800000
#define HWIO_I2C_MASTER_STATUS_INVALID_TAG_SHFT                                                            0x17
#define HWIO_I2C_MASTER_STATUS_INPUT_FSM_STATE_BMSK                                                    0x700000
#define HWIO_I2C_MASTER_STATUS_INPUT_FSM_STATE_SHFT                                                        0x14
#define HWIO_I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_RESET_STATE_FVAL                                       0x0
#define HWIO_I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_IDLE_STATE_FVAL                                        0x1
#define HWIO_I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_READ_LAST_BYTE_STATE_FVAL                              0x2
#define HWIO_I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_MI_REC_STATE_FVAL                                      0x3
#define HWIO_I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_DEC_STATE_FVAL                                         0x4
#define HWIO_I2C_MASTER_STATUS_INPUT_FSM_STATE_V1_V2_STORE_STATE_FVAL                                       0x5
#define HWIO_I2C_MASTER_STATUS_INPUT_FSM_STATE_V2_WR_TAG_STATE_FVAL                                         0x6
#define HWIO_I2C_MASTER_STATUS_INPUT_FSM_STATE_V2_WAIT_TAG_STATE_FVAL                                       0x7
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_BMSK                                                    0xf0000
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_SHFT                                                       0x10
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_RESET_STATE_FVAL                                      0x0
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_IDLE_STATE_FVAL                                       0x1
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_DECODE_STATE_FVAL                                     0x2
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_SEND_STATE_FVAL                                       0x3
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_MI_REC_STATE_FVAL                                     0x4
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_NOP_STATE_FVAL                                        0x5
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_INVALID_STATE_FVAL                                    0x6
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_PEEK_STATE_FVAL                                          0x7
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_SEND_R_STATE_FVAL                                        0x8
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V2_GET_BYTE_STATE_FVAL                                      0xb
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V2_GET_WRITE_BYTE_STATE_FVAL                                0xc
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V2_SPECIALITY_STATE_FVAL                                    0xd
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V2_WAIT_FLUSH_STOP_STATE_FVAL                               0xe
#define HWIO_I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V2_STOP_STATE_FVAL                                          0xf
#define HWIO_I2C_MASTER_STATUS_CLK_STATE_BMSK                                                            0xe000
#define HWIO_I2C_MASTER_STATUS_CLK_STATE_SHFT                                                               0xd
#define HWIO_I2C_MASTER_STATUS_CLK_STATE_RESET_BUSIDLE_STATE_FVAL                                           0x0
#define HWIO_I2C_MASTER_STATUS_CLK_STATE_NOT_MASTER_STATE_FVAL                                              0x1
#define HWIO_I2C_MASTER_STATUS_CLK_STATE_HIGH_STATE_FVAL                                                    0x2
#define HWIO_I2C_MASTER_STATUS_CLK_STATE_LOW_STATE_FVAL                                                     0x3
#define HWIO_I2C_MASTER_STATUS_CLK_STATE_HIGH_WAIT_STATE_FVAL                                               0x4
#define HWIO_I2C_MASTER_STATUS_CLK_STATE_FORCED_LOW_STATE_FVAL                                              0x5
#define HWIO_I2C_MASTER_STATUS_CLK_STATE_HS_ADDR_LOW_STATE_FVAL                                             0x6
#define HWIO_I2C_MASTER_STATUS_CLK_STATE_DOUBLE_BUFFER_WAIT_STATE_FVAL                                      0x7
#define HWIO_I2C_MASTER_STATUS_DATA_STATE_BMSK                                                           0x1c00
#define HWIO_I2C_MASTER_STATUS_DATA_STATE_SHFT                                                              0xa
#define HWIO_I2C_MASTER_STATUS_DATA_STATE_RESET_WAIT_STATE_FVAL                                             0x0
#define HWIO_I2C_MASTER_STATUS_DATA_STATE_TX_ADDR_STATE_FVAL                                                0x1
#define HWIO_I2C_MASTER_STATUS_DATA_STATE_TX_DATA_STATE_FVAL                                                0x2
#define HWIO_I2C_MASTER_STATUS_DATA_STATE_TX_HS_ADDR_STATE_FVAL                                             0x3
#define HWIO_I2C_MASTER_STATUS_DATA_STATE_TX_10_BIT_ADDR_STATE_FVAL                                         0x4
#define HWIO_I2C_MASTER_STATUS_DATA_STATE_TX_2ND_BYTE_STATE_FVAL                                            0x5
#define HWIO_I2C_MASTER_STATUS_DATA_STATE_RX_DATA_STATE_FVAL                                                0x6
#define HWIO_I2C_MASTER_STATUS_DATA_STATE_RX_NACK_HOLD_STATE_FVAL                                           0x7
#define HWIO_I2C_MASTER_STATUS_BUS_MASTER_BMSK                                                            0x200
#define HWIO_I2C_MASTER_STATUS_BUS_MASTER_SHFT                                                              0x9
#define HWIO_I2C_MASTER_STATUS_BUS_ACTIVE_BMSK                                                            0x100
#define HWIO_I2C_MASTER_STATUS_BUS_ACTIVE_SHFT                                                              0x8
#define HWIO_I2C_MASTER_STATUS_FAILED_BMSK                                                                 0xc0
#define HWIO_I2C_MASTER_STATUS_FAILED_SHFT                                                                  0x6
#define HWIO_I2C_MASTER_STATUS_INVALID_WRITE_BMSK                                                          0x20
#define HWIO_I2C_MASTER_STATUS_INVALID_WRITE_SHFT                                                           0x5
#define HWIO_I2C_MASTER_STATUS_ARB_LOST_BMSK                                                               0x10
#define HWIO_I2C_MASTER_STATUS_ARB_LOST_SHFT                                                                0x4
#define HWIO_I2C_MASTER_STATUS_PACKET_NACKED_BMSK                                                           0x8
#define HWIO_I2C_MASTER_STATUS_PACKET_NACKED_SHFT                                                           0x3
#define HWIO_I2C_MASTER_STATUS_BUS_ERROR_BMSK                                                               0x4
#define HWIO_I2C_MASTER_STATUS_BUS_ERROR_SHFT                                                               0x2

#define HWIO_I2C_MASTER_CONFIG_ADDR(x)                                                               ((x) + 0x00000408)
#define HWIO_I2C_MASTER_CONFIG_PHYS(x)                                                               ((x) + 0x00000408)
#define HWIO_I2C_MASTER_CONFIG_OFFS                                                                  (0x00000408)
#define HWIO_I2C_MASTER_CONFIG_RMSK                                                                         0x1
#define HWIO_I2C_MASTER_CONFIG_IN(x)      \
        in_dword_masked(HWIO_I2C_MASTER_CONFIG_ADDR(x), HWIO_I2C_MASTER_CONFIG_RMSK)
#define HWIO_I2C_MASTER_CONFIG_INM(x, m)      \
        in_dword_masked(HWIO_I2C_MASTER_CONFIG_ADDR(x), m)
#define HWIO_I2C_MASTER_CONFIG_OUT(x, v)      \
        out_dword(HWIO_I2C_MASTER_CONFIG_ADDR(x),v)
#define HWIO_I2C_MASTER_CONFIG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_I2C_MASTER_CONFIG_ADDR(x),m,v,HWIO_I2C_MASTER_CONFIG_IN(x))
#define HWIO_I2C_MASTER_CONFIG_EN_VERSION_TWO_TAG_BMSK                                                      0x1
#define HWIO_I2C_MASTER_CONFIG_EN_VERSION_TWO_TAG_SHFT                                                      0x0

#define HWIO_I2C_MASTER_BUS_CLEAR_ADDR(x)                                                            ((x) + 0x0000040c)
#define HWIO_I2C_MASTER_BUS_CLEAR_PHYS(x)                                                            ((x) + 0x0000040c)
#define HWIO_I2C_MASTER_BUS_CLEAR_OFFS                                                               (0x0000040c)
#define HWIO_I2C_MASTER_BUS_CLEAR_RMSK                                                                      0x1
#define HWIO_I2C_MASTER_BUS_CLEAR_OUT(x, v)      \
        out_dword(HWIO_I2C_MASTER_BUS_CLEAR_ADDR(x),v)
#define HWIO_I2C_MASTER_BUS_CLEAR_CLEAR_BMSK                                                                0x1
#define HWIO_I2C_MASTER_BUS_CLEAR_CLEAR_SHFT                                                                0x0

#define HWIO_I2C_MASTER_LOCAL_ID_ADDR(x)                                                             ((x) + 0x00000410)
#define HWIO_I2C_MASTER_LOCAL_ID_PHYS(x)                                                             ((x) + 0x00000410)
#define HWIO_I2C_MASTER_LOCAL_ID_OFFS                                                                (0x00000410)
#define HWIO_I2C_MASTER_LOCAL_ID_RMSK                                                                      0xff
#define HWIO_I2C_MASTER_LOCAL_ID_IN(x)      \
        in_dword_masked(HWIO_I2C_MASTER_LOCAL_ID_ADDR(x), HWIO_I2C_MASTER_LOCAL_ID_RMSK)
#define HWIO_I2C_MASTER_LOCAL_ID_INM(x, m)      \
        in_dword_masked(HWIO_I2C_MASTER_LOCAL_ID_ADDR(x), m)
#define HWIO_I2C_MASTER_LOCAL_ID_LOCAL_ID_BMSK                                                             0xff
#define HWIO_I2C_MASTER_LOCAL_ID_LOCAL_ID_SHFT                                                              0x0


#endif /* __QUP_HWIO__ */
