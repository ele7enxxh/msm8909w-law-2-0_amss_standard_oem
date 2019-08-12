#ifndef HALSDCCI_H
#define HALSDCCI_H

/**********************************************************************
 * HALsdcci.h
 *
 * This is a PRIVATE header file for internal use of sdcc HAL and is
 * not intended to be included by any other module other than sdcc
 * HAL.
 *
 * This header file declares internal functions common to all SDCC2
 * HAL implementations.
 *
 * Copyright (c) 2008-2014
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/

/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/hal/HALsdcc_sdcc4.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     ---------------------------------------------------
2014-09-24   sb      Switch the Voltage PADs to 1.8V mode (mode18) for 9x45
2014-03-11   pa      Code cleanup
2013-09-03   bn      Add SDHCi, DDR support
2013-06-13   bn      Added MCLK_REG_WR_ACTIVE check
2011-11-29   rh      Remove the use of START_BIT_ERR, not used on SDCC4Add DATA_END interrupt support
2010-07-21   rh      Add DATA_END interrupt support
2009-07-15   vin     Using SDIO interrupt bit SDIO_INTR_OPER
2009-05-07   vin     HAL_sdcc_DataTimeout simplified to just program MCI 
                     register
2009-02-12   vj      OS Abstraction Layer
2008-11-14   sr      HAL Adaptation for WM6.
2008-06-02   umr     Initial version.
=======================================================================*/
#include SDCC_OSAL_H

/* Define the required HWIO bit field information */
#define HWIO_MCI_POWER_RMSK                               0x41
#define HWIO_MCI_POWER_OPEN_DRAIN_BMSK                    0x40
#define HWIO_MCI_POWER_OPEN_DRAIN_SHFT                     0x6
#define HWIO_MCI_POWER_CONTROL_BMSK                        0x1
#define HWIO_MCI_POWER_CONTROL_SHFT                          0

#define HWIO_MCI_CLK_RMSK                              0x1ff00
#define HWIO_MCI_CLK_HCLKON_SW_EN_BMSK                 0x10000
#define HWIO_MCI_CLK_HCLKON_SW_EN_SHFT                    0x10
#define HWIO_MCI_CLK_SELECT_IN_BMSK                     0xc000
#define HWIO_MCI_CLK_SELECT_IN_SHFT                        0xe
#define HWIO_MCI_CLK_INVERT_OUT_BMSK                    0x2000
#define HWIO_MCI_CLK_INVERT_OUT_SHFT                       0xd
#define HWIO_MCI_CLK_FLOW_ENA_BMSK                      0x1000
#define HWIO_MCI_CLK_FLOW_ENA_SHFT                         0xc
#define HWIO_MCI_CLK_WIDEBUS_BMSK                        0xc00
#define HWIO_MCI_CLK_WIDEBUS_SHFT                          0xa
#define HWIO_MCI_CLK_PWRSAVE_BMSK                        0x200
#define HWIO_MCI_CLK_PWRSAVE_SHFT                          0x9
#define HWIO_MCI_CLK_ENABLE_BMSK                         0x100
#define HWIO_MCI_CLK_ENABLE_SHFT                           0x8

#define HWIO_MCI_CMD_RMSK                               0xffff
#define HWIO_MCI_CMD_CCS_DISABLE_BMSK                   0x8000
#define HWIO_MCI_CMD_CCS_DISABLE_SHFT                      0xf
#define HWIO_MCI_CMD_CCS_ENABLE_BMSK                    0x4000
#define HWIO_MCI_CMD_CCS_ENABLE_SHFT                       0xe
#define HWIO_MCI_CMD_MCIABORT_BMSK                      0x2000
#define HWIO_MCI_CMD_MCIABORT_SHFT                         0xd
#define HWIO_MCI_CMD_DAT_CMD_BMSK                       0x1000
#define HWIO_MCI_CMD_DAT_CMD_SHFT                          0xc
#define HWIO_MCI_CMD_PROG_ENA_BMSK                       0x800
#define HWIO_MCI_CMD_PROG_ENA_SHFT                         0xb
#define HWIO_MCI_CMD_ENABLE_BMSK                         0x400
#define HWIO_MCI_CMD_ENABLE_SHFT                           0xa
#define HWIO_MCI_CMD_PENDING_BMSK                        0x200
#define HWIO_MCI_CMD_PENDING_SHFT                          0x9
#define HWIO_MCI_CMD_INTERRUPT_BMSK                      0x100
#define HWIO_MCI_CMD_INTERRUPT_SHFT                        0x8
#define HWIO_MCI_CMD_LONGRSP_BMSK                         0x80
#define HWIO_MCI_CMD_LONGRSP_SHFT                          0x7
#define HWIO_MCI_CMD_RESPONSE_BMSK                        0x40
#define HWIO_MCI_CMD_RESPONSE_SHFT                         0x6
#define HWIO_MCI_CMD_CMD_INDEX_BMSK                       0x3f
#define HWIO_MCI_CMD_CMD_INDEX_SHFT                          0

#define HWIO_MCI_DATA_CTL_RMSK                         0x3ffff
#define HWIO_MCI_DATA_CTL_DATA_PEND_BMSK               0x20000
#define HWIO_MCI_DATA_CTL_DATA_PEND_SHFT                  0x11
#define HWIO_MCI_DATA_CTL_BLOCKSIZE_BMSK               0x1fff0
#define HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT                   0x4
#define HWIO_MCI_DATA_CTL_DM_ENABLE_BMSK                   0x8
#define HWIO_MCI_DATA_CTL_DM_ENABLE_SHFT                   0x3
#define HWIO_MCI_DATA_CTL_MODE_BMSK                        0x4
#define HWIO_MCI_DATA_CTL_MODE_SHFT                        0x2
#define HWIO_MCI_DATA_CTL_DIRECTION_BMSK                   0x2
#define HWIO_MCI_DATA_CTL_DIRECTION_SHFT                   0x1
#define HWIO_MCI_DATA_CTL_ENABLE_BMSK                      0x1
#define HWIO_MCI_DATA_CTL_ENABLE_SHFT                        0

#define HWIO_MCI_STATUS_RMSK                        0x7ffffdff
#define HWIO_MCI_STATUS_AUTO_CMD19_TIMEOUT_BMSK     0x40000000
#define HWIO_MCI_STATUS_AUTO_CMD19_TIMEOUT_SHFT           0x1e
#define HWIO_MCI_STATUS_BOOT_TIMEOUT_BMSK           0x20000000
#define HWIO_MCI_STATUS_BOOT_TIMEOUT_SHFT                 0x1d
#define HWIO_MCI_STATUS_BOOT_ACK_ERR_BMSK           0x10000000
#define HWIO_MCI_STATUS_BOOT_ACK_ERR_SHFT                 0x1c
#define HWIO_MCI_STATUS_BOOT_ACK_REC_BMSK            0x8000000
#define HWIO_MCI_STATUS_BOOT_ACK_REC_SHFT                 0x1b
#define HWIO_MCI_STATUS_CCS_TIMEOUT_BMSK             0x4000000
#define HWIO_MCI_STATUS_CCS_TIMEOUT_SHFT                  0x1a
#define HWIO_MCI_STATUS_SDIO_INTR_OPER_BMSK          0x2000000
#define HWIO_MCI_STATUS_SDIO_INTR_OPER_SHFT               0x19
#define HWIO_MCI_STATUS_ATA_CMD_COMPL_BMSK           0x1000000
#define HWIO_MCI_STATUS_ATA_CMD_COMPL_SHFT                0x18
#define HWIO_MCI_STATUS_PROG_DONE_BMSK                0x800000
#define HWIO_MCI_STATUS_PROG_DONE_SHFT                    0x17
#define HWIO_MCI_STATUS_SDIO_INTR_BMSK                0x400000
#define HWIO_MCI_STATUS_SDIO_INTR_SHFT                    0x16
#define HWIO_MCI_STATUS_RXDATA_AVLBL_BMSK             0x200000
#define HWIO_MCI_STATUS_RXDATA_AVLBL_SHFT                 0x15
#define HWIO_MCI_STATUS_TXDATA_AVLBL_BMSK             0x100000
#define HWIO_MCI_STATUS_TXDATA_AVLBL_SHFT                 0x14
#define HWIO_MCI_STATUS_RXFIFO_EMPTY_BMSK              0x80000
#define HWIO_MCI_STATUS_RXFIFO_EMPTY_SHFT                 0x13
#define HWIO_MCI_STATUS_TXFIFO_EMPTY_BMSK              0x40000
#define HWIO_MCI_STATUS_TXFIFO_EMPTY_SHFT                 0x12
#define HWIO_MCI_STATUS_RXFIFO_FULL_BMSK               0x20000
#define HWIO_MCI_STATUS_RXFIFO_FULL_SHFT                  0x11
#define HWIO_MCI_STATUS_TXFIFO_FULL_BMSK               0x10000
#define HWIO_MCI_STATUS_TXFIFO_FULL_SHFT                  0x10
#define HWIO_MCI_STATUS_RXFIFO_HALF_FULL_BMSK           0x8000
#define HWIO_MCI_STATUS_RXFIFO_HALF_FULL_SHFT              0xf
#define HWIO_MCI_STATUS_TXFIFO_HALF_FULL_BMSK           0x4000
#define HWIO_MCI_STATUS_TXFIFO_HALF_FULL_SHFT              0xe
#define HWIO_MCI_STATUS_RXACTIVE_BMSK                   0x2000
#define HWIO_MCI_STATUS_RXACTIVE_SHFT                      0xd
#define HWIO_MCI_STATUS_TXACTIVE_BMSK                   0x1000
#define HWIO_MCI_STATUS_TXACTIVE_SHFT                      0xc
#define HWIO_MCI_STATUS_CMD_ACTIVE_BMSK                  0x800
#define HWIO_MCI_STATUS_CMD_ACTIVE_SHFT                    0xb
#define HWIO_MCI_STATUS_DATA_BLK_END_BMSK                0x400
#define HWIO_MCI_STATUS_DATA_BLK_END_SHFT                  0xa
#define HWIO_MCI_STATUS_DATAEND_BMSK                     0x100
#define HWIO_MCI_STATUS_DATAEND_SHFT                       0x8
#define HWIO_MCI_STATUS_CMD_SENT_BMSK                     0x80
#define HWIO_MCI_STATUS_CMD_SENT_SHFT                      0x7
#define HWIO_MCI_STATUS_CMD_RESPONSE_END_BMSK             0x40
#define HWIO_MCI_STATUS_CMD_RESPONSE_END_SHFT              0x6
#define HWIO_MCI_STATUS_RX_OVERRUN_BMSK                   0x20
#define HWIO_MCI_STATUS_RX_OVERRUN_SHFT                    0x5
#define HWIO_MCI_STATUS_TX_UNDERRUN_BMSK                  0x10
#define HWIO_MCI_STATUS_TX_UNDERRUN_SHFT                   0x4
#define HWIO_MCI_STATUS_DATA_TIMEOUT_BMSK                  0x8
#define HWIO_MCI_STATUS_DATA_TIMEOUT_SHFT                  0x3
#define HWIO_MCI_STATUS_CMD_TIMEOUT_BMSK                   0x4
#define HWIO_MCI_STATUS_CMD_TIMEOUT_SHFT                   0x2
#define HWIO_MCI_STATUS_DATA_CRC_FAIL_BMSK                 0x2
#define HWIO_MCI_STATUS_DATA_CRC_FAIL_SHFT                 0x1
#define HWIO_MCI_STATUS_CMD_CRC_FAIL_BMSK                  0x1
#define HWIO_MCI_STATUS_CMD_CRC_FAIL_SHFT                    0

#define HWIO_MCI_CLEAR_RMSK                         0x1dc007ff
#define HWIO_MCI_CLEAR_CCS_TIMEOUT_BMSK              0x4000000
#define HWIO_MCI_CLEAR_CCS_TIMEOUT_SHFT                   0x1a
#define HWIO_MCI_CLEAR_ATA_CMD_COMPL_BMSK            0x1000000
#define HWIO_MCI_CLEAR_ATA_CMD_COMPL_SHFT                 0x18
#define HWIO_MCI_CLEAR_PROG_DONE_CLR_BMSK             0x800000
#define HWIO_MCI_CLEAR_PROG_DONE_CLR_SHFT                 0x17
#define HWIO_MCI_CLEAR_SDIO_INTR_CLR_BMSK             0x400000
#define HWIO_MCI_CLEAR_SDIO_INTR_CLR_SHFT                 0x16
#define HWIO_MCI_CLEAR_DATA_BLK_END_CLR_BMSK             0x400
#define HWIO_MCI_CLEAR_DATA_BLK_END_CLR_SHFT               0xa
#define HWIO_MCI_CLEAR_START_BIT_ERR_CLR_BMSK            0x200
#define HWIO_MCI_CLEAR_START_BIT_ERR_CLR_SHFT              0x9
#define HWIO_MCI_CLEAR_DATA_END_CLR_BMSK                 0x100
#define HWIO_MCI_CLEAR_DATA_END_CLR_SHFT                   0x8
#define HWIO_MCI_CLEAR_CMD_SENT_CLR_BMSK                  0x80
#define HWIO_MCI_CLEAR_CMD_SENT_CLR_SHFT                   0x7
#define HWIO_MCI_CLEAR_CMD_RESP_END_CLT_BMSK              0x40
#define HWIO_MCI_CLEAR_CMD_RESP_END_CLT_SHFT               0x6
#define HWIO_MCI_CLEAR_RX_OVERRUN_CLR_BMSK                0x20
#define HWIO_MCI_CLEAR_RX_OVERRUN_CLR_SHFT                 0x5
#define HWIO_MCI_CLEAR_TX_UNDERRUN_CLR_BMSK               0x10
#define HWIO_MCI_CLEAR_TX_UNDERRUN_CLR_SHFT                0x4
#define HWIO_MCI_CLEAR_DATA_TIMEOUT_CLR_BMSK               0x8
#define HWIO_MCI_CLEAR_DATA_TIMEOUT_CLR_SHFT               0x3
#define HWIO_MCI_CLEAR_CMD_TIMOUT_CLR_BMSK                 0x4
#define HWIO_MCI_CLEAR_CMD_TIMOUT_CLR_SHFT                 0x2
#define HWIO_MCI_CLEAR_DATA_CRC_FAIL_CLR_BMSK              0x2
#define HWIO_MCI_CLEAR_DATA_CRC_FAIL_CLR_SHFT              0x1
#define HWIO_MCI_CLEAR_CMD_CRC_FAIL_CLR_BMSK               0x1
#define HWIO_MCI_CLEAR_CMD_CRC_FAIL_CLR_SHFT                 0

#define HWIO_MCI_INT_MASK0_RMSK                     0x1fffffff
#define HWIO_MCI_INT_MASK1_RMSK                     0x1fffffff
#define HWIO_MCI_RESPn_RMSK                         0xffffffff
#define HWIO_MCI_FIFO_RMSK                          0xffffffff

#define HWIO_MCI_STATUS2_MCLK_REG_WR_ACTIVE_BMSK           0x1
#define HWIO_MCI_STATUS2_MCLK_REG_WR_ACTIVE_SHFT             0
#define HWIO_SDC_MCI_STATUS2_REG_IDX                  (0x006C)


/*
 * Register values specific to SDCC2 core
 */
#define MCI_CLK_SET_FLOW            (1UL << HWIO_MCI_CLK_FLOW_ENA_SHFT)

#define MCI_DATA_DMA_ENABLED        (1UL << HWIO_MCI_DATA_CTL_DM_ENABLE_SHFT)
#define MCI_DATA_DMA_DISABLED       (0UL << HWIO_MCI_DATA_CTL_DM_ENABLE_SHFT)

/* Field enumeration for MCI_DATA_CTL */
#define MCI_DATA_DPSM_ENABLED       (1UL << HWIO_MCI_DATA_CTL_ENABLE_SHFT)
#define MCI_DATA_DPSM_DISABLED      (0UL << HWIO_MCI_DATA_CTL_ENABLE_SHFT)
#define MCI_DATA_READ               (1UL << HWIO_MCI_DATA_CTL_DIRECTION_SHFT)
#define MCI_DATA_WRITE              (0UL << HWIO_MCI_DATA_CTL_DIRECTION_SHFT)
#define MCI_DATA_BLOCK_MODE         (0UL << HWIO_MCI_DATA_CTL_MODE_SHFT )
#define MCI_DATA_STREAM_MODE        (1UL << HWIO_MCI_DATA_CTL_MODE_SHFT )

/* Field enumeration for MCI_POWER */
#define MCI_POWER_CTRL_OFF          0x0
#define MCI_POWER_CTRL_UP           0x2
#define MCI_POWER_CTRL_ON           0x3

/* Field enumeration for MCI_CLK */
#define MCI_CLK_SEL_IN_FALLING_EDGE (0UL << HWIO_MCI_CLK_SELECT_IN_SHFT)
#define MCI_CLK_SEL_IN_RISING_EDGE  (1UL << HWIO_MCI_CLK_SELECT_IN_SHFT)
#define MCI_CLK_SEL_IN_FEEDBACK     (2UL << HWIO_MCI_CLK_SELECT_IN_SHFT)
#define MCI_CLK_SEL_IN_LOOPBACK     (3UL << HWIO_MCI_CLK_SELECT_IN_SHFT)
#define MCI_CLK_SEL_IN_DDR          (3UL << HWIO_MCI_CLK_SELECT_IN_SHFT)

#define MCI_CLK_INVERT_IN           (1UL << HWIO_MCI_CLK_INVERT_IN_SHFT)
#define MCI_CLK_INVERT_OUT          (1UL << HWIO_MCI_CLK_INVERT_OUT_SHFT)

#define MCI_CLK_PWRSAVE_ENABLED     (1UL << HWIO_MCI_CLK_PWRSAVE_SHFT)
#define MCI_CLK_PWRSAVE_DISABLED    (0UL << HWIO_MCI_CLK_PWRSAVE_SHFT)

#define MCI_CLK_ENABLED             (1UL << HWIO_MCI_CLK_ENABLE_SHFT)
#define MCI_CLK_DISABLED            (0UL << HWIO_MCI_CLK_ENABLE_SHFT)

/* Field enumeration for MCI_INT_MASK0  */
#define MCI_INT_MASK_PROG_DONE      HWIO_MCI_STATUS_PROG_DONE_BMSK
#define MCI_INT_MASK_DATA_TIMEOUT   HWIO_MCI_STATUS_DATA_TIMEOUT_BMSK
#define MCI_INT_MASK_SDIO_INTR_OPER HWIO_MCI_STATUS_SDIO_INTR_OPER_BMSK
#define MCI_INT_MASK_DMA_DONE       HWIO_MCI_STATUS_DMA_DONE_BMSK
#define MCI_INT_MASK_DATAEND        HWIO_MCI_STATUS_DATAEND_BMSK

#define SDCC_CMD_INTERRUPTS   ( HWIO_FMSK( MCI_STATUS, CMD_RESPONSE_END)  | \
                                HWIO_FMSK( MCI_STATUS, CMD_SENT)          | \
                                HWIO_FMSK( MCI_STATUS, CMD_TIMEOUT)       | \
                                HWIO_FMSK( MCI_STATUS, CMD_CRC_FAIL) )

#define MCI_BLOCK_LENGTH            0x0200
#define MCI_ARGU_NULL               0x0000

#define MCI_FIFO_SIZE               16  /* FIFO SIZE = 16 * 32bit */

/* Field enumeration for MCI_CMD  */
#define MCI_CMD_MCIABORT            (1UL << HWIO_MCI_CMD_MCIABORT_SHFT)
#define MCI_CMD_DAT_CMD             (1UL << HWIO_MCI_CMD_DAT_CMD_SHFT)
#define MCI_CMD_RESPONSE            (1UL << HWIO_MCI_CMD_RESPONSE_SHFT)
#define MCI_CMD_LONG_RESP           (1UL << HWIO_MCI_CMD_LONGRSP_SHFT)
#define MCI_CMD_ENABLED             (1UL << HWIO_MCI_CMD_ENABLE_SHFT)
#define MCI_CMD_PROG_ENABLED        (1UL << HWIO_MCI_CMD_PROG_ENA_SHFT)

/** MCI_CLK BUSWIDE Register Values */
#define MCI_CLK_SET_1BIT_WIDE          (0UL << HWIO_MCI_CLK_WIDEBUS_SHFT)
#define MCI_CLK_SET_4BIT_WIDE          (2UL << HWIO_MCI_CLK_WIDEBUS_SHFT)
#define MCI_CLK_SET_8BIT_WIDE          (3UL << HWIO_MCI_CLK_WIDEBUS_SHFT)

/* MCI_PWRCTL_CLEAR_REG */
#define MCI_PWRCTL_CLR_BUS_OFF_BMSK     0x1 /* Bit 0 */
#define MCI_PWRCTL_CLR_BUS_ON_BMSK      0x2 /* Bit 1 */
#define MCI_PWRCTL_CLR_IO_LOW_V_BMSK    0x4 /* Bit 2 */
#define MCI_PWRCTL_CLR_IO_HIGH_V_BMSK   0x8 /* Bit 3 */

/* MCI_PWRCTL_CTL_REG */
#define MCI_PWRCTL_CTL_BUS_ON_OFF_SUCCESS_BMSK    0x1 /* Bit 0 */
#define MCI_PWRCTL_CTL_BUS_ON_OFF_FAIL_BMSK       0x2 /* Bit 1 */
#define MCI_PWRCTL_CTL_IO_SIG_SWITCH_SUCCESS_BMSK 0x4 /* Bit 2 */
#define MCI_PWRCTL_CTL_IO_SIG_SWITCH_FAIL_BMSK    0x8 /* Bit 3 */

/*********************************************************************/
/*                           MACROS                                  */
/*********************************************************************/
/* the static (clearable) bits */
#define MCI_STATUS_STATIC_MASK         0x018007FF

#define HAL_SDCC_DIR_READ_ERROR  ( HWIO_FMSK( MCI_STATUS, DATA_TIMEOUT )   | \
                               HWIO_FMSK( MCI_STATUS, RX_OVERRUN)      | \
                               HWIO_FMSK( MCI_STATUS, DATA_CRC_FAIL) )

#define HAL_SDCC_DIR_WRITE_ERROR ( HWIO_FMSK( MCI_STATUS, DATA_TIMEOUT )  | \
                               HWIO_FMSK( MCI_STATUS, TX_UNDERRUN)    | \
                               HWIO_FMSK( MCI_STATUS, DATA_CRC_FAIL) )

#define HAL_SDCC_DMA_ERROR   ( HWIO_FMSK( MCI_STATUS, DATA_TIMEOUT )  | \
                               HWIO_FMSK( MCI_STATUS, TX_UNDERRUN)    | \
                               HWIO_FMSK( MCI_STATUS, RX_OVERRUN)     | \
                               HWIO_FMSK( MCI_STATUS, DATA_CRC_FAIL) )

#define HAL_SDCC_DATA_ERROR  ( HWIO_FMSK( MCI_STATUS, CMD_CRC_FAIL )  | \
                               HWIO_FMSK( MCI_STATUS, DATA_CRC_FAIL)  | \
                               HWIO_FMSK( MCI_STATUS, CMD_TIMEOUT)    | \
                               HWIO_FMSK( MCI_STATUS, DATA_TIMEOUT) )

#define HAL_SDCC_STATUS_CMD_CRC_FAIL     (HWIO_FMSK (MCI_STATUS, CMD_CRC_FAIL))
#define HAL_SDCC_STATUS_CMD_TIMEOUT      (HWIO_FMSK (MCI_STATUS, CMD_TIMEOUT))
#define HAL_SDCC_STATUS_DATA_TIMEOUT     (HWIO_FMSK (MCI_STATUS, DATA_TIMEOUT))
#define HAL_SDCC_STATUS_TX_UNDERRUN      (HWIO_FMSK (MCI_STATUS, TX_UNDERRUN))
#define HAL_SDCC_STATUS_RX_OVERRUN       (HWIO_FMSK (MCI_STATUS, RX_OVERRUN))
#define HAL_SDCC_STATUS_CMD_RESPONSE_END (HWIO_FMSK (MCI_STATUS, CMD_RESPONSE_END))
#define HAL_SDCC_STATUS_CMD_SENT         (HWIO_FMSK (MCI_STATUS, CMD_SENT))
#define HAL_SDCC_STATUS_START_BIT_ERR    (0)
#define HAL_SDCC_STATUS_TXFIFO_FULL      (HWIO_FMSK (MCI_STATUS, TXFIFO_FULL))
#define HAL_SDCC_STATUS_RXDATA_AVLBL     (HWIO_FMSK (MCI_STATUS, RXDATA_AVLBL))
#define HAL_SDCC_STATUS_RXFIFO_HALF_FULL (HWIO_FMSK (MCI_STATUS, RXFIFO_HALF_FULL))
#define HAL_SDCC_STATUS_TXFIFO_HALF_FULL (HWIO_FMSK (MCI_STATUS, TXFIFO_HALF_FULL))
#define HAL_SDCC_STATUS_TXACTIVE         (HWIO_FMSK (MCI_STATUS, TXACTIVE))
#define HAL_SDCC_STATUS_SDIO_INTR        (HWIO_FMSK (MCI_STATUS, SDIO_INTR))
#define HAL_SDCC_STATUS_DATA_END         (HWIO_FMSK (MCI_STATUS, DATAEND))
#define HAL_SDCC_STATUS_PROG_DONE        (HWIO_FMSK (MCI_STATUS, PROG_DONE))
#define HAL_SDCC_STATUS_SDIO_INTR_OPER   (HWIO_FMSK (MCI_STATUS, SDIO_INTR_OPER))
#define HAL_SDCC_STATUS_READ_ERROR       (HAL_SDCC_DIR_READ_ERROR)
#define HAL_SDCC_STATUS_WRITE_ERROR      (HAL_SDCC_DIR_WRITE_ERROR)
#define HAL_SDCC_STATUS_DMA_ERROR        (HAL_SDCC_DMA_ERROR)
#define HAL_SDCC_STATUS_DATA_ERROR       (HAL_SDCC_DATA_ERROR)
#define HAL_SDCC_STATUS_ANY_STATIC       (MCI_STATUS_STATIC_MASK)

/* Define the offset of each of the register from base */
#define HWIO_SDC_MCI_POWER_REG_IDX          (0x0000)
#define HWIO_SDC_MCI_CLK_REG_IDX            (0x0004)
#define HWIO_SDC_MCI_ARGUMENT_REG_IDX       (0x0008)
#define HWIO_SDC_MCI_CMD_REG_IDX            (0x000C)
#define HWIO_SDC_MCI_RESPn_REG_IDX          (0x0014)
#define HWIO_SDC_MCI_DATA_TIMER_REG_IDX     (0x0024)
#define HWIO_SDC_MCI_DATA_LENGTH_REG_IDX    (0x0028)
#define HWIO_SDC_MCI_DATA_CTL_REG_IDX       (0x002C)
#define HWIO_SDC_MCI_STATUS_REG_IDX         (0x0034)
#define HWIO_SDC_MCI_CLEAR_REG_IDX          (0x0038)
#define HWIO_SDC_MCI_INT_MASK0_REG_IDX      (0x003C)
#define HWIO_SDC_MCI_INT_MASK1_REG_IDX      (0x0040)
#define HWIO_SDC_MCI_FIFO_REG_IDX           (0x0080)
#define HWIO_SDC_MCI_PWRCTL_STATUS_REG_IDX  (0x00DC)
#define HWIO_SDC_MCI_PWRCTL_CLEAR_REG_IDX   (0x00E4)
#define HWIO_SDC_MCI_PWRCTL_CTL_REG_IDX     (0x00E8)

/* Define the required HWIO input/out macro mapping */
#define HWIO_SDC_REG_IN(reg, b)                       \
        (in_dword((HWIO_SDC_##reg##_REG_IDX+b)))
#define HWIO_SDC_REG_IN_IDX(reg, b, idx)              \
        (in_dword((HWIO_SDC_##reg##_REG_IDX+b+(4*idx))))
#define HWIO_SDC_REG_OUT(reg, b, v)                    \
        (out_dword((HWIO_SDC_##reg##_REG_IDX+b),v))
#define HWIO_SDC_REG_OUTMI(reg, b, m, v) {             \
        HWIO_INTLOCK();                                \
        out_dword_masked_ns((HWIO_SDC_##reg##_REG_IDX+b), m, v,       \
                            (in_dword((HWIO_SDC_##reg##_REG_IDX+b)))); \
        HWIO_INTFREE(); }

/* HWIO_OUTMI requires this typedef */
typedef struct sdcc_reg_addr sdcc_access_type;

/* Table that stores the register location */
extern uint32 sdcc_reg_base[];
extern uint32 sdcc_reg_phys[];

void HAL_sdcc_WaitWrActive (const uint32 nController);

/* Inline function that implements the simple I/Os */

inline void
HAL_sdcc_Init (const uint32 nController, char **ppszVersion)
{
  uint32 reg_value = 0;
  
  HWIO_SDC_REG_OUT (MCI_CMD, sdcc_reg_base[nController], 0);
  HAL_sdcc_WaitWrActive (nController); 

  HWIO_SDC_REG_OUT (MCI_DATA_CTL, sdcc_reg_base[nController], 0);
  HAL_sdcc_WaitWrActive (nController); 

  HWIO_SDC_REG_OUT (MCI_CLEAR, sdcc_reg_base[nController], MCI_STATUS_STATIC_MASK);

  /* Switch the Voltage PADs to 1.8 V mode (mode18)*/
  reg_value = HWIO_SDC_REG_IN (MCI_CLK, sdcc_reg_base[nController]);
  reg_value |= 0x00200000;
  HWIO_SDC_REG_OUT (MCI_CLK, sdcc_reg_base[nController], reg_value);
  
  /* Widebus selection */
  HWIO_SDC_REG_OUTMI (MCI_CLK, sdcc_reg_base[nController], HWIO_FMSK (MCI_CLK, WIDEBUS), 0);
  
  HAL_sdcc_WaitWrActive (nController);

  HWIO_SDC_REG_OUT (MCI_CLEAR, sdcc_reg_base[nController], HWIO_MCI_CLEAR_SDIO_INTR_CLR_BMSK);
  HWIO_SDC_REG_OUT (MCI_INT_MASK0, sdcc_reg_base[nController], 0);
}

/* Number of items in the save/restore storage array */
#define SDCC_SAVE_RESTORE_ARRAY_SIZE         3

inline void
HAL_sdcc_Save (const uint32 nController, uint32 *state_array)
{
  /* Read and store register state */
  state_array[0] = HWIO_SDC_REG_IN (MCI_CLK, sdcc_reg_base[nController]);
  state_array[1] = HWIO_SDC_REG_IN (MCI_POWER, sdcc_reg_base[nController]);
  state_array[2] = HWIO_SDC_REG_IN (MCI_INT_MASK0, sdcc_reg_base[nController]);
}

inline void
HAL_sdcc_Restore (const uint32 nController, uint32 *state_array)
{
  /* Restore register state */
  HWIO_SDC_REG_OUT (MCI_CLK, sdcc_reg_base[nController], state_array[0]);
  HAL_sdcc_WaitWrActive (nController);  

  HWIO_SDC_REG_OUT (MCI_POWER, sdcc_reg_base[nController], state_array[1]);
  HAL_sdcc_WaitWrActive (nController); 

  HWIO_SDC_REG_OUT (MCI_INT_MASK0, sdcc_reg_base[nController], state_array[2]);
}

inline void
HAL_sdcc_SetPowerState (const uint32 nController, enum HAL_sdcc_PowerState state)
{
  if (state == HAL_SDCC_POWER_OFF) 
  {
      HWIO_SDC_REG_OUTMI (MCI_POWER, sdcc_reg_base[nController],
                  HWIO_FMSK (MCI_POWER, CONTROL), MCI_POWER_CTRL_OFF);
  }
  else
  {
      HWIO_SDC_REG_OUTMI (MCI_POWER, sdcc_reg_base[nController],
                  HWIO_FMSK (MCI_POWER, CONTROL), MCI_POWER_CTRL_ON);
  }

  HAL_sdcc_WaitWrActive (nController); 
}

inline void
HAL_sdcc_SetClockEdge (const uint32 nController,
                       enum HAL_sdcc_ClockEdge select_in)
{
  uint32 mci_select_in = 0;

  switch (select_in)
  {
    case HAL_SDCC_CLOCK_EDGE_FALLING:
      /* Latch data and command coming in on the FALLING edge of MCIClock */
      mci_select_in = MCI_CLK_SEL_IN_FALLING_EDGE;
      break;
    case HAL_SDCC_CLOCK_EDGE_RISING:
      /* Latch data and command coming in on the RISING edge of MCIClock */
      mci_select_in = MCI_CLK_SEL_IN_RISING_EDGE;
      break;
    case HAL_SDCC_CLOCK_EDGE_FEEDBACK:
      /* Latch data and command coming in using feedback clock */
      mci_select_in = MCI_CLK_SEL_IN_FEEDBACK;
      break;
    case HAL_SDCC_CLOCK_EDGE_LOOPBACK:
      /* Latch data and command coming in loopback mode */
      mci_select_in = MCI_CLK_SEL_IN_LOOPBACK;
      break;

    default:
      // ERROR
      return;
  }

  /* Program MCI_CLK SELECT_IN register of sdcc controller core */
  HWIO_SDC_REG_OUTMI (MCI_CLK, sdcc_reg_base[nController], 
                      HWIO_FMSK (MCI_CLK, SELECT_IN), mci_select_in);
  
  HAL_sdcc_WaitWrActive (nController);
}

inline void
HAL_sdcc_SetInvertOut (const uint32 nController,
                       enum HAL_sdcc_InvertOut nClockInvertOut)
{
  uint32 mci_invert_out = 0;

  switch (nClockInvertOut)
  {
    case HAL_SDCC_INVERT_OUT_CLEAR:
      /* Data and command go out on the falling edge of MCIClock */
      mci_invert_out = 0;
      break;
    case HAL_SDCC_INVERT_OUT_SET:
      /* Data and command go out on the rising edge of MCIClock */
      mci_invert_out = MCI_CLK_INVERT_OUT;
      break;
      /* other cases are undefined */
    default:
      // ERROR
      return;
  }
  /* Program MCI_CLK INVERT_OUT register of sdcc controller core */
  HWIO_SDC_REG_OUTMI (MCI_CLK, sdcc_reg_base[nController], 
                      HWIO_FMSK (MCI_CLK, INVERT_OUT), mci_invert_out);
  HAL_sdcc_WaitWrActive (nController);
}

inline void
HAL_sdcc_FlowControl (const uint32 nController, boolean flow_enable)
{
  if (flow_enable)
  {
    /* Enable Flow Control */
    HWIO_SDC_REG_OUTMI (MCI_CLK, sdcc_reg_base[nController], 
                        HWIO_FMSK (MCI_CLK, FLOW_ENA), MCI_CLK_SET_FLOW);
  }
  else
  {
    /* Disable Flow Control */
    HWIO_SDC_REG_OUTMI (MCI_CLK, sdcc_reg_base[nController], 
                        HWIO_FMSK (MCI_CLK, FLOW_ENA), 0);
  }
  HAL_sdcc_WaitWrActive (nController);
}

inline void
HAL_sdcc_PowerSave (const uint32 nController, boolean pwr_save)
{
  if (pwr_save)
  {
    /* Enable Power Save - Clock is enabled only when bus is active */
    HWIO_SDC_REG_OUTMI (MCI_CLK, sdcc_reg_base[nController],
                        HWIO_FMSK (MCI_CLK, PWRSAVE), MCI_CLK_PWRSAVE_ENABLED);
  }
  else
  {
    /* Disable Pwr Save - Clock is always enabled */
    HWIO_SDC_REG_OUTMI (MCI_CLK, sdcc_reg_base[nController],
                        HWIO_FMSK (MCI_CLK, PWRSAVE), MCI_CLK_PWRSAVE_DISABLED);
  }
  HAL_sdcc_WaitWrActive (nController);
}

inline void
HAL_sdcc_BusClockEnable (const uint32 nController, boolean clk_enable)
{
  if (clk_enable)
  {
    /* Enable Clock */
    HWIO_SDC_REG_OUTMI (MCI_CLK, sdcc_reg_base[nController], 
                        HWIO_FMSK (MCI_CLK, ENABLE), MCI_CLK_ENABLED);
  }
  else
  {
    /* Disable Clock */
    HWIO_SDC_REG_OUTMI (MCI_CLK, sdcc_reg_base[nController], 
                        HWIO_FMSK (MCI_CLK, ENABLE), MCI_CLK_DISABLED);
  }
  HAL_sdcc_WaitWrActive (nController);
}

inline void
HAL_sdcc_SdCmdAbort (const uint32 nController)
{
  HWIO_SDC_REG_OUT (MCI_CMD, sdcc_reg_base[nController], MCI_CMD_MCIABORT);
  HAL_sdcc_WaitWrActive (nController);
}

inline void
HAL_sdcc_GetResponse (const uint32 nController, uint32 * resp, uint32 idx)
{
  *resp = HWIO_SDC_REG_IN_IDX (MCI_RESPn, sdcc_reg_base[nController], idx);
}

inline void
HAL_sdcc_DataTimeout (const uint32 nController, uint32 data_timer)
{
  /* Set Data timeout period */
  HWIO_SDC_REG_OUT (MCI_DATA_TIMER, sdcc_reg_base[nController], data_timer);
}

inline void
HAL_sdcc_DataLength (const uint32 nController, uint32 data_length)
{
  HWIO_SDC_REG_OUT (MCI_DATA_LENGTH, sdcc_reg_base[nController], data_length);
}

inline void
HAL_sdcc_BlockLength (const uint32 nController, uint32 block_size)
{
  HWIO_SDC_REG_OUTMI (MCI_DATA_CTL, sdcc_reg_base[nController],
                      HWIO_FMSK (MCI_DATA_CTL, BLOCKSIZE),
                      block_size << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT);
  HAL_sdcc_WaitWrActive (nController);
}

inline void
HAL_sdcc_DataControlStopDPSM (const uint32 nController)
{
   /*stop the DPSM*/
   HWIO_SDC_REG_OUTMI (MCI_DATA_CTL, sdcc_reg_base[nController],
                       HWIO_FMSK(MCI_DATA_CTL, ENABLE), 0);
  HAL_sdcc_WaitWrActive (nController);
}

inline uint32
HAL_sdcc_StatusNow (const uint32 nController)
{
  return HWIO_SDC_REG_IN (MCI_STATUS, sdcc_reg_base[nController]);
}

inline uint32
HAL_sdcc_StatusCheckFlag (const uint32 nController,
                          const uint32 status,
                          const uint32 flag)
{
  return status & flag;
}

inline void
HAL_sdcc_StatusClear (const uint32 nController, const uint32 flag)
{
  HWIO_SDC_REG_OUT (MCI_CLEAR, sdcc_reg_base[nController], flag);
}

inline void
HAL_sdcc_StatusClearFlags (const uint32 nController, const uint32 u32Flag)
{
  /* the static (clearable) bits */
  HWIO_SDC_REG_OUT (MCI_CLEAR, sdcc_reg_base[nController], u32Flag);
}

inline void
HAL_sdcc_StatusClearStaticFlags (const uint32 nController)
{
  /* the static (clearable) bits */
  HWIO_SDC_REG_OUT (MCI_CLEAR, sdcc_reg_base[nController], MCI_STATUS_STATIC_MASK);
}

inline void
HAL_sdcc_FifoWrite (const uint32 nController, uint32 fifo_data)
{
  HWIO_SDC_REG_OUT (MCI_FIFO, sdcc_reg_base[nController], fifo_data);
}

inline uint32
HAL_sdcc_FifoRead (const uint32 nController)
{
  return HWIO_SDC_REG_IN (MCI_FIFO, sdcc_reg_base[nController]);
}

inline uint32
HAL_sdcc_FifoLogicalAddress (const uint32 nController)
{
  return (sdcc_reg_base[nController] + HWIO_SDC_MCI_FIFO_REG_IDX);
}

inline uint32
HAL_sdcc_FifoPhysicalAddress (const uint32 nController)
{
  return (sdcc_reg_phys[nController] + HWIO_SDC_MCI_FIFO_REG_IDX);
}

#endif /* !HALSDCCI_H */

