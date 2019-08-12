#ifndef FLASHI_HAL_NANDC_H
#define FLASHI_HAL_NANDC_H
/*=======================================================================
 * FILE:        flashi_hal_nandc.h
 *
 * SERVICES:    HAL internal header for QPIC NAND controller
 *
 * DESCRIPTION: HAL internal header for QPIC NAND controller
 *
 * Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/qpic/flashi_hal_nandc.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/14/15     sb      Set max num-of-zeros allowed for an erased page
 * 11/28/14     sb      Use 0x35 cmd for ONFI parameter page read
 * 06/13/14     sb      Update to support 8909
 * 09/03/13     eo      Include target specific HWIO register mapping
 * 11/06/12     sv      Update Flash Status check
 * 09/05/12     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include <HALhwio.h>
#include FLASH_HAL_HWIO_INCLUDE_H
#include "flash_hal_reg_list.h"


/* H/W I/O MACROS */
#define NANDC_OUT(reg, val)       FLASH_HAL_OUT(QPIC_NANDC, reg, val)
#define NANDC_IN(reg, dest)       FLASH_HAL_IN(QPIC_NANDC, reg, dest)

#define NANDC_INI(reg, cs, dest)  FLASH_HAL_INI(QPIC_NANDC, reg, cs, dest)
#define NANDC_OUTI(reg, cs, val)  FLASH_HAL_OUTI(QPIC_NANDC, reg, cs, val)

#define NANDC_INS(reg, dest, op)  FLASH_HAL_INS(QPIC_NANDC, reg, dest, op)
#define NANDC_OUTS(reg, val, op)  FLASH_HAL_OUTS(QPIC_NANDC, reg, val, op)

/* Memory Copy Macros */
#define NANDC_COPY_TO_DEVICE(reg, src, len) FLASH_HAL_COPY_TO_DEVICE(reg, src, len)
#define NANDC_COPY_FROM_DEVICE(dest, reg, len) FLASH_HAL_COPY_FROM_DEVICE(reg, dest, len)

#define NANDC_VADDR(reg)          FLASH_HAL_GET_VADDR(QPIC_NANDC,reg)
#define NANDC_PADDR(reg)          FLASH_HAL_GET_PADDR(QPIC_NANDC,reg)
#define NANDC_VADDRI(reg,cs)      FLASH_HAL_GET_VADDRI(QPIC_NANDC,reg,cs)
#define NANDC_PADDRI(reg,cs)      FLASH_HAL_GET_PADDRI(QPIC_NANDC,reg,cs)

/*------------------------ Common Defines -----------------*/

#define NAND_OPT_ON                      1UL      /* Turn on an option */
#define NAND_OPT_OFF                     0UL      /* Turn off an option */

/*------------------------ Memory Usage defines -----------------*/

/* To support 2048B page size */
#define FLASH_HAL_MEM_USAGE_1ST_2048PAGE   2048
#define FLASH_HAL_MEM_USAGE_2ND_2048PAGE   1668
#define FLASH_HAL_MEM_USAGE_NTH_2048PAGE    724

#define FLASH_HAL_MEM_USAGE_2048PAGE(n)    (((n)==1) ? \
  FLASH_HAL_MEM_USAGE_1ST_2048PAGE:(FLASH_HAL_MEM_USAGE_1ST_2048PAGE + \
  FLASH_HAL_MEM_USAGE_2ND_2048PAGE+FLASH_HAL_MEM_USAGE_NTH_2048PAGE*((n)-2)))

/* To support 4096B page size */
#define FLASH_HAL_MEM_USAGE_1ST_4096PAGE   3048
#define FLASH_HAL_MEM_USAGE_2ND_4096PAGE   3096
#define FLASH_HAL_MEM_USAGE_NTH_4096PAGE   1380

#define FLASH_HAL_MEM_USAGE_4096PAGE(n)    (((n)==1) ? \
  FLASH_HAL_MEM_USAGE_1ST_4096PAGE:(FLASH_HAL_MEM_USAGE_1ST_4096PAGE + \
  FLASH_HAL_MEM_USAGE_2ND_4096PAGE+FLASH_HAL_MEM_USAGE_NTH_4096PAGE*((n)-2)))

/*------------- Masks for operation completion ------------------------------*/

#define FLASH_NAND_STATUS_OP_DONE             0x20

#define FLASH_NAND_READ_STATUS_CLEAR          0xC0

#define FLASH_NAND_STATUS_OP_STATUS_MASK                        \
          HWIO_NAND_FLASH_STATUS_OPER_STATUS_BMSK

#define FLASH_NAND_STATUS_MPU_ERROR_MASK                       \
              HWIO_NAND_FLASH_STATUS_MPU_ERROR_BMSK

#define FLASH_NAND_STATUS_OP_ERR_MASK                       \
              HWIO_NAND_FLASH_STATUS_OP_ERR_BMSK

#define FLASH_NAND_STATUS_ANY_ERR_MASK                          \
          (HWIO_NAND_FLASH_STATUS_OP_ERR_BMSK |                \
           HWIO_NAND_FLASH_STATUS_MPU_ERROR_BMSK)

/* mask for ECC error */
#define FLASH_NAND_STATUS_ECC_ANY_ERR_MASK                      \
          (HWIO_NANDC_BUFFER_STATUS_UNCORRECTABLE_BMSK)

/* mask for OP_RESULT bit in the status register */
#define FLASH_NAND_STATUS_WR_ER_OP_RSLT_MASK                    \
          (HWIO_NAND_FLASH_STATUS_PROG_ERASE_OP_RESULT_BMSK)

#define FLASH_NAND_STATUS_OP_ANY_ERR_MASK                       \
          FLASH_NAND_STATUS_ANY_ERR_MASK

#define FLASH_NAND_PROG_ERASE_DONE_MASK                         \
          HWIO_NAND_FLASH_STATUS_PROG_ERASE_OP_RESULT_BMSK

#define FLASH_NANDC_EXEC_CMD                                    \
          HWIO_NANDC_EXEC_CMD_EXEC_CMD_BMSK

#define FLASH_NAND_STATUS_CW_CNTR_MASK                          \
          HWIO_NAND_FLASH_STATUS_CODEWORD_CNTR_BMSK

#define FLASH_NAND_STATUS_CW_CNTR_SHFT                          \
          HWIO_NAND_FLASH_STATUS_CODEWORD_CNTR_SHFT

#define FLASH_NAND_CTRL_BAM_MODE_EN \
          HWIO_NAND_CTRL_BAM_MODE_EN_BAM_MODE_FVAL

#define FLASH_NAND_DEV0_ECC_CFG_ECC_SW_RESET \
          (HWIO_NAND_DEV0_ECC_CFG_ECC_SW_RESET_RESET_FVAL << \
          HWIO_NAND_DEV0_ECC_CFG_ECC_SW_RESET_SHFT)

#define FLASH_NAND_READ_LOCATION_n_LAST \
          HWIO_NAND_READ_LOCATION_n_LAST_BMSK

#define FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT \
          HWIO_NAND_READ_LOCATION_n_SIZE_SHFT


/*------------------------ NAND Controller Address Defines -----------------*/

/* NANDC Physical addresses */
#define HAL_QPIC_NANDC_NAND_ADDR0_PADDR      HWIO_PHYS(NAND_ADDR0)
#define HAL_QPIC_NANDC_NAND_ADDR1_PADDR      HWIO_PHYS(NAND_ADDR1)
#define HAL_QPIC_NANDC_NANDC_EXEC_CMD_PADDR  HWIO_PHYS(NANDC_EXEC_CMD)
#define HAL_QPIC_NANDC_FLASH_DEV_CMD0_PADDR  HWIO_PHYS(FLASH_DEV_CMD0)
#define HAL_QPIC_NANDC_FLASH_DEV_CMD1_PADDR  HWIO_PHYS(FLASH_DEV_CMD1)
#define HAL_QPIC_NANDC_FLASH_DEV_CMD2_PADDR  HWIO_PHYS(FLASH_DEV_CMD2)
#define HAL_QPIC_NANDC_NAND_FLASH_CMD_PADDR  HWIO_PHYS(NAND_FLASH_CMD)

#define HAL_QPIC_NANDC_FLASH_DEV_CMD_VLD_PADDR                  \
                                          HWIO_PHYS(FLASH_DEV_CMD_VLD)
#define HAL_QPIC_NANDC_NANDC_BUFFER_PADDR                 \
                                          HWIO_PHYSI(FLASH_BUFFn_ACC,0)
#define HAL_QPIC_NANDC_NAND_FLASH_CHIP_SELECT_PADDR                  \
                                          HWIO_PHYS(NAND_FLASH_CHIP_SELECT)
#define HAL_QPIC_NANDC_EBI2_ECC_BUF_CFG_PADDR             \
                                          HWIO_PHYS(EBI2_ECC_BUF_CFG)

#define HAL_QPIC_NANDC_NAND_CTRL_PADDR    HWIO_PHYS(NAND_CTRL)

#define HAL_QPIC_NANDC_NAND_FLASH_READ_ID_PADDR                \
                                          HWIO_PHYS(NAND_FLASH_READ_ID)
#define HAL_QPIC_NANDC_NAND_FLASH_STATUS_PADDR                 \
                                          HWIO_PHYS(NAND_FLASH_STATUS)
#define HAL_QPIC_NANDC_NAND_FLASH_READ_STATUS_PADDR                 \
                                          HWIO_PHYS(NAND_FLASH_READ_STATUS)
#define HAL_QPIC_NANDC_NANDC_BUFFER_STATUS_PADDR          \
                                          HWIO_PHYS(NANDC_BUFFER_STATUS)
#define HAL_QPIC_NANDC_NAND_DEV0_CFG0_PADDR  \
                                          HWIO_PHYS(NAND_DEV0_CFG0)
#define HAL_QPIC_NANDC_NAND_DEVn_CFG1_PADDR(whichflash)  \
                                          HWIO_PHYSI(NAND_DEVn_CFG1,whichflash)
#define HAL_QPIC_NANDC_NAND_DEV0_ECC_CFG_PADDR \
                                          HWIO_PHYS(NAND_DEV0_ECC_CFG)
#define HAL_QPIC_NANDC_SFLASHC_BURST_CFG_PADDR    HWIO_SFLASHC_BURST_CFG_PHYS
#define HAL_QPIC_NANDC_NAND_ERASED_CW_DETECT_CFG_PADDR      \
                                          HWIO_PHYS(NAND_ERASED_CW_DETECT_CFG)
#define HAL_QPIC_NANDC_NAND_ERASED_CW_DETECT_STATUS_PADDR   \
                                          HWIO_PHYS(NAND_ERASED_CW_DETECT_STATUS)
#define HAL_QPIC_NANDC_NAND_READ_LOCATION_PADDR \
                                          HWIO_PHYSI(NAND_READ_LOCATION_n, 0)

/* NANDC Virtual address */
#define HAL_QPIC_NANDC_NAND_FLASH_READ_ID_VADDR   HWIO_NAND_FLASH_READ_ID_ADDR
#define HAL_QPIC_NANDC_NAND_FLASH_STATUS_VADDR    HWIO_NAND_FLASH_STATUS_ADDR
#define HAL_QPIC_NANDC_NAND_FLASH_READ_STATUS_VADDR  HWIO_NAND_FLASH_READ_STATUS_ADDR
#define HAL_QPIC_NANDC_NANDC_BUFFER_STATUS_VADDR  HWIO_NANDC_BUFFER_STATUS_ADDR
#define HAL_QPIC_NANDC_NANDC_BUFFER_VADDR         HWIO_FLASH_BUFFn_ACC_ADDR(0)
#define HAL_QPIC_NANDC_NAND_FLASH_CMD_VADDR       HWIO_NAND_FLASH_CMD_ADDR
#define HAL_QPIC_NANDC_FLASH_DEV_CMD_VLD_VADDR    HWIO_FLASH_DEV_CMD_VLD_ADDR
#define HAL_QPIC_NANDC_NAND_ADDR0_VADDR           HWIO_NAND_ADDR0_ADDR
#define HAL_QPIC_NANDC_NANDC_ADDR1_VADDR          HWIO_NAND_ADDR1_ADDR
#define HAL_QPIC_NANDC_NANDC_EXEC_CMD_VADDR       HWIO_NANDC_EXEC_CMD_ADDR
#define HAL_QPIC_NANDC_FLASH_DEV_CMD0_VADDR       HWIO_FLASH_DEV_CMD0_ADDR
#define HAL_QPIC_NANDC_FLASH_DEV_CMD1_VADDR       HWIO_FLASH_DEV_CMD1_ADDR
#define HAL_QPIC_NANDC_FLASH_DEV_CMD2_VADDR       HWIO_FLASH_DEV_CMD2_ADDR
#define HAL_QPIC_NANDC_EBI2_ECC_BUF_CFG_VADDR     HWIO_EBI2_ECC_BUF_CFG_ADDR
#define HAL_QPIC_NANDC_NAND_CTRL_VADDR            HWIO_NAND_CTRL_ADDR

#define HAL_QPIC_NANDC_NAND_ERASED_CW_DETECT_CFG_VADDR \
          HWIO_NAND_ERASED_CW_DETECT_CFG_ADDR

#define HAL_QPIC_NANDC_NAND_ERASED_CW_DETECT_STATUS_VADDR  \
          HWIO_NAND_ERASED_CW_DETECT_STATUS_ADDR

#define HAL_QPIC_NANDC_NANDC_DEV0_CFG0_VADDR(whichflash) \
          HWIO_NAND_DEV0_CFG0_ADDR(whichflash)

#define HAL_QPIC_NANDC_NANDC_DEVn_CFG1_VADDR(whichflash) \
          HWIO_NAND_DEVn_CFG1_ADDR(whichflash)

#define HAL_QPIC_NANDC_NAND_FLASH_CHIP_SELECT_VADDR  \
          HWIO_NAND_FLASH_CHIP_SELECT_ADDR

#define HAL_QPIC_NANDC_NAND_READ_LOCATION_VADDR   HWIO_NAND_READ_LOCATION_n_ADDR(0)

#define FLASH_NAND_DEV_STATUS_WP_MASK                         0x00800000
#define FLASH_NAND_DEV_STATUS_WP_ENABLED                      0x0

/* The nandc status mask includes the following bits from the Status Register
 * OP_ERR, MPU_ERROR, CODEWORD_CNTR, OPER_STATUS, READY_BSY_N
 * These bits are checked during the status check after any NAND operation.
 * Additionally the Write Protect bit which is DEV_STATUS[7] is checked for
 * Program and Erase operations.
 * Note: After any Status Read from NAND, OP_ERR bit in NAND_FLASH_STATUS 
 * register is updated with the value read in DEV_STATUS[0] bit.
 */
#define FLASH_NANDC_STATUS_MASK                                \
          FLASH_NAND_STATUS_CW_CNTR_MASK   |                   \
          FLASH_NAND_STATUS_ANY_ERR_MASK   |                   \
          FLASH_NAND_STATUS_OP_STATUS_MASK |                   \
          HWIO_NAND_FLASH_STATUS_READY_BSY_N_BMSK

#define FLASH_NANDC_IGNORE_MPU_STATUS_MASK                     \
          FLASH_NAND_STATUS_CW_CNTR_MASK   |                   \
          FLASH_NAND_STATUS_OP_ERR_MASK    |                   \
          FLASH_NAND_STATUS_OP_STATUS_MASK |                   \
          HWIO_NAND_FLASH_STATUS_READY_BSY_N_BMSK

#define FLASH_NANDC_BUFFER_STATUS_MASK                         \
          HWIO_NANDC_BUFFER_STATUS_UNCORRECTABLE_BMSK   |      \
          HWIO_NANDC_BUFFER_STATUS_NUM_ERRORS_BMSK

#define FLASH_NANDC_BUFFER_STATUS_ERASED_VAL                   \
          ((1 <<HWIO_NANDC_BUFFER_STATUS_UNCORRECTABLE_SHFT) | \
           (1 << HWIO_NANDC_BUFFER_STATUS_NUM_ERRORS_SHFT))

#define FLASH_NANDC_ERASED_CW_DET_STATUS_MASK                  \
          HWIO_NAND_ERASED_CW_DETECT_STATUS_CODEWORD_ALL_ERASED_BMSK

#define FLASH_QPIC_NANDC_ERASED_PAGE_DET_STATUS_MASK                  \
          HWIO_NAND_ERASED_CW_DETECT_STATUS_PAGE_ALL_ERASED_BMSK

/****************************************************************
 * Defines for NAND_DEVn_CFG base
 * We set up the base for these registers to be generic.
 ***************************************************************/

#define NAND_DEVn_CFG0_BASE                                                    \
        (NAND_OPT_ON << HWIO_SHFT(NAND_DEV0_CFG0, SET_RD_MODE_AFTER_STATUS) |  \
         0 << HWIO_SHFT(NAND_DEV0_CFG0, NUM_ADDR_CYCLES) |                     \
         0 << HWIO_SHFT(NAND_DEV0_CFG0, SPARE_SIZE_BYTES) |                    \
         NAND_ECC_SIZE << HWIO_SHFT(NAND_DEV0_CFG0, ECC_PARITY_SIZE_BYTES) |   \
         NAND_UD_BYTE_SIZE << HWIO_SHFT(NAND_DEV0_CFG0, UD_SIZE_BYTES) |       \
         0 << HWIO_SHFT(NAND_DEV0_CFG0, CW_PER_PAGE))

#define NAND_DEVn_CFG1_BASE                                                    \
        (2 << HWIO_SHFT(NAND_DEVn_CFG1, WR_RD_BSY_GAP) |                       \
         0 << HWIO_SHFT(NAND_DEVn_CFG1, BAD_BLOCK_IN_SPARE_AREA) |             \
         0 << HWIO_SHFT(NAND_DEVn_CFG1, BAD_BLOCK_BYTE_NUM) |                  \
         NAND_OPT_OFF << HWIO_SHFT(NAND_DEVn_CFG1, CS_ACTIVE_BSY) |            \
         7 << HWIO_SHFT(NAND_DEVn_CFG1, NAND_RECOVERY_CYCLES) |                \
         NAND_OPT_OFF << HWIO_SHFT(NAND_DEVn_CFG1, WIDE_FLASH) |               \
         NAND_OPT_OFF << HWIO_SHFT(NAND_DEVn_CFG1, ECC_DISABLE))

#define NAND_DEVn_ECC_CFG_BASE                                           \
  (NAND_OPT_ON << HWIO_SHFT(NAND_DEV0_ECC_CFG, ECC_FORCE_CLK_OPEN) |     \
   0x200 << HWIO_SHFT(NAND_DEV0_ECC_CFG, ECC_NUM_DATA_BYTES) |           \
   0xD << HWIO_SHFT(NAND_DEV0_ECC_CFG, ECC_PARITY_SIZE_BYTES) |          \
   0x1 << HWIO_SHFT(NAND_DEV0_ECC_CFG, ECC_MODE) |                       \
   NAND_OPT_OFF << HWIO_SHFT(NAND_DEV0_ECC_CFG, ECC_DISABLE))

/****************************************************************
 * Defines for NAND_ERASED_CW_DETECT_CFG base
 * We set up the base for these registers to be generic.
 ***************************************************************/
  
#define NAND_ERASED_CW_DETECT_CFG_BASE                                       \
  (NAND_OPT_ON << HWIO_SHFT(NAND_ERASED_CW_DETECT_CFG, ERASED_CW_ECC_MASK) | \
   NAND_OPT_ON << HWIO_SHFT(NAND_ERASED_CW_DETECT_CFG, AUTO_DETECT_RES))
  
/****************************************************************
 * Defines for FLASH_DEV_CMD_VLD base
 * The FLASH_DEV_CMD_VLD register determines what device commands
 * are valid. For NAND, READ_STOP does not apply.
 ***************************************************************/

#define NAND_FLASH_DEV_CMD_VLD_BASE                                            \
        (0xECUL << HWIO_SHFT(FLASH_DEV_CMD_VLD, READ_PARAMETER_PAGE_CODE) |    \
         NAND_OPT_OFF << HWIO_SHFT(FLASH_DEV_CMD_VLD, READ_STOP_VLD) |         \
         NAND_OPT_ON << HWIO_SHFT(FLASH_DEV_CMD_VLD, ERASE_START_VLD) |        \
         NAND_OPT_ON << HWIO_SHFT(FLASH_DEV_CMD_VLD, WRITE_START_VLD) |        \
         NAND_OPT_ON << HWIO_SHFT(FLASH_DEV_CMD_VLD, SEQ_READ_START_VLD) |     \
         NAND_OPT_ON << HWIO_SHFT(FLASH_DEV_CMD_VLD, READ_START_VLD))

/* Configure XFR register (dependent on clk and device) */
#define FLASH_NAND_CFG_XFR(x, y)  HWIO_OUT (FLASH_XFR_STEP##x##, y)

/* General nand controller commands */
#define FLASH_NAND_SFT_RESET_CMD           0x1
#define FLASH_NAND_PAGE_READ_CMD           0x32
#define FLASH_NAND_PAGE_READ_ECC_CMD       0x33
#define FLASH_NAND_PAGE_READ_ALL_CMD       0x34
#define FLASH_NAND_SEQ_PAGE_READ_CMD       0x15
#define FLASH_NAND_PARAMETER_PAGE_READ_CMD 0x35
#define FLASH_NAND_PRG_PAGE_CMD            0x36
#define FLASH_NAND_PRG_PAGE_ECC_CMD        0x37
#define FLASH_NAND_PRG_PAGE_ALL_CMD        0x39
#define FLASH_NAND_BLOCK_ERASE_CMD         0x3A
#define FLASH_NAND_ID_FETCH_CMD            0xB
#define FLASH_NAND_STATUS_CMD              0xC
#define FLASH_NAND_RESET_FLASH_CMD         0xD

#define FLASH_NAND_SW_CMD_CP_READ_START    0x3500
#define FLASH_NAND_SW_CMD_CP_CMD1_MSK      0xFFFF0000
#define FLASH_NAND_SW_CMD_CP_WRITE_START   0x1085

#define FLASH_DEV_CMD0_DEFAULT_CMD         0x1080D060
#define FLASH_DEV_CMD0_WRITE_CMD_MSK       0x0000FFFF
#define FLASH_DEV_CMD0_ERASE_CMD_MSK       0xFFFF0000
#define FLASH_DEV_CMD0_RDM_WRITE_VALUE0    0x7080D060
#define FLASH_DEV_CMD0_RDM_WRITE_VALUE1    0x8580D060
#define FLASH_DEV_CMD0_RDM_WRITE_VALUE2    0x7085D060
#define FLASH_DEV_CMD0_RDM_WRITE_VALUE3    0x1085D060
#define FLASH_DEV_CMD0_CACHE_WRITE_CMD     0x1580D060

#define FLASH_DEV_CMD1_DEFAULT_CMD         0xF00F3000
#define FLASH_DEV_CMD1_READ_CMD_MSK        0xFFFF0000
#define FLASH_DEV_CMD1_READ_ADDR_CMD_MSK   0xFFFFFF00
#define FLASH_DEV_CMD1_READ_ADDR_CMD_SHFT  8
#define FLASH_DEV_CMD1_RDM_READ_VALUE      0xF00FE005
#define FLASH_DEV_CMD1_CACHE_READ_START    0xF00F3031
#define FLASH_DEV_CMD1_CACHE_READ_END      0xF00F303F

#define FLASH_DEV_CMD2_DEFAULT_VALUE       0xF0FF7090
#define FLASH_DEV_CMD2_RESET_CMD_MSK       0xFF00FFFF
#define FLASH_DEV_CMD2_RESET_CMD_SHFT      16
#define FLASH_DEV_CMD2_STATUS_CMD_MSK      0xFFFF00FF
#define FLASH_DEV_CMD2_STATUS_CMD_SHFT     8
#define FLASH_DEV_CMD2_ID_CMD_MSK          0xFFFFFF00
#define FLASH_DEV_CMD2_ID_CMD_SHFT         0

#define FLASH_NAND_READ_DEFAULT_CMD        0x3000
#define FLASH_NAND_WRITE_DEFAULT_CMD       0x1080
#define FLASH_NAND_ERASE_DEFAULT_CMD       0xD060
#define FLASH_NAND_RDM_READ_DEFAULT_CMD    0xE005
#define FLASH_NAND_RESET_DEFAULT_CMD       0xFF
#define FLASH_NAND_READ_ID_DEFAULT_CMD     0x90
#define FLASH_NAND_STATUS_DEFAULT_CMD      0x70

#define FLASH_DEV_CMD_READ_STRT_VLD_MSK    0x00000001

/* These macros ONLY write to the shadow variables
 * and not to the hardware registers.
 * They're meant to be used for DataMover accesses.
 */
#define FLASH_SPARE_SIZE_BYTES(priv, x)                                   \
  do {                                                                    \
    priv->cfg0 &= ~HWIO_FMSK(NAND_DEV0_CFG0, SPARE_SIZE_BYTES);           \
    priv->cfg0 |= x << HWIO_SHFT(NAND_DEV0_CFG0, SPARE_SIZE_BYTES);       \
  } while (0)

#define FLASH_ECC_SIZE(priv, x)                                           \
  do {                                                                    \
    priv->cfg0 &= ~HWIO_FMSK(NAND_DEV0_CFG0, ECC_PARITY_SIZE_BYTES);      \
    priv->cfg0 |=                                                         \
      x << HWIO_SHFT(NAND_DEV0_CFG0, ECC_PARITY_SIZE_BYTES);              \
  } while (0)

#define FLASH_UD_SIZE(priv, x)                                            \
  do {                                                                    \
    priv->cfg0 &= ~HWIO_FMSK(NAND_DEV0_CFG0, UD_SIZE_BYTES);              \
    priv->cfg0 |=                                                         \
      x << HWIO_SHFT(NAND_DEV0_CFG0, UD_SIZE_BYTES);                      \
  } while (0)

#define FLASH_CW_PER_PAGE(priv, x)                                        \
  do {                                                                    \
    priv->cfg0 &= ~HWIO_FMSK(NAND_DEV0_CFG0, CW_PER_PAGE);                \
    priv->cfg0 |=                                                         \
            x << HWIO_SHFT(NAND_DEV0_CFG0, CW_PER_PAGE);                  \
  } while (0)

#define FLASH_NUM_ADDR_CYCLES(priv, x)                                    \
  do {                                                                    \
    priv->cfg0 &= ~HWIO_FMSK(NAND_DEV0_CFG0, NUM_ADDR_CYCLES);            \
    priv->cfg0 |= x << HWIO_SHFT(NAND_DEV0_CFG0, NUM_ADDR_CYCLES);        \
  } while (0)

#define FLASH_DISABLE_ECC(priv)                                           \
  do {                                                                    \
    priv->cfg1 &= ~HWIO_FMSK(NAND_DEVn_CFG1, ECC_DISABLE);                \
    priv->cfg1 |= NAND_OPT_ON << (HWIO_SHFT(NAND_DEVn_CFG1, ECC_DISABLE));\
  } while (0)

#define FLASH_ENABLE_ECC(priv)                                             \
  do {                                                                     \
    priv->cfg1 &= ~HWIO_FMSK(NAND_DEVn_CFG1, ECC_DISABLE);                 \
    priv->cfg1 |= NAND_OPT_OFF << (HWIO_SHFT(NAND_DEVn_CFG1, ECC_DISABLE));\
  } while (0)

/* Resets BCH ECC enable bit to select RS ECC engine */
#define FLASH_SELECT_RS_ECC(priv)                                            \
  do {                                                                         \
    priv->cfg1 &= ~HWIO_FMSK(NAND_DEVn_CFG1, ENABLE_BCH_ECC);                  \
    priv->cfg1 |= NAND_OPT_OFF << (HWIO_SHFT(NAND_DEVn_CFG1, ENABLE_BCH_ECC)); \
  } while (0)

/* Sets BCH ECC enable bit to select BCH ECC engine */
#define FLASH_SELECT_BCH_ECC(priv)                                             \
  do {                                                                         \
    priv->cfg1 &= ~HWIO_FMSK(NAND_DEVn_CFG1, ENABLE_BCH_ECC);                  \
    priv->cfg1 |= NAND_OPT_ON << (HWIO_SHFT(NAND_DEVn_CFG1, ENABLE_BCH_ECC));  \
  } while (0)
  
#define FLASH_BAD_BLOCK_IN_SPARE_AREA(priv)                               \
  do {                                                                    \
    priv->cfg1 &= ~HWIO_FMSK(NAND_DEVn_CFG1, BAD_BLOCK_IN_SPARE_AREA);    \
    priv->cfg1 |=                                                         \
      NAND_OPT_ON << HWIO_SHFT(NAND_DEVn_CFG1, BAD_BLOCK_IN_SPARE_AREA);  \
  } while (0)

#define FLASH_BAD_BLOCK_BYTE_NUM(priv, x)                                 \
  do {                                                                    \
    priv->cfg1 &= ~HWIO_FMSK(NAND_DEVn_CFG1, BAD_BLOCK_BYTE_NUM);         \
    priv->cfg1 |= x << HWIO_SHFT(NAND_DEVn_CFG1, BAD_BLOCK_BYTE_NUM);     \
  } while (0)

/* Enable auto detect for erased cw detect cfg register  */
#define FLASH_ENABLE_ERASED_CW_DETECT(priv)                                   \
  do {                                                                        \
    priv->erased_cw_det_cfg &=                                                \
      ~HWIO_FMSK(NAND_ERASED_CW_DETECT_CFG, AUTO_DETECT_RES);                 \
    priv->erased_cw_det_cfg |=                                                \
      NAND_OPT_OFF << (HWIO_SHFT(NAND_ERASED_CW_DETECT_CFG, AUTO_DETECT_RES));\
  } while (0)

/* Set max number of zeros allowed for an erased page
#define FLASH_N_MAX_ZEROS_ERASED_PAGE(priv, x)                            \
  do {                                                                    \
    priv->erased_cw_det_cfg &=                                            \
      ~HWIO_FMSK(NAND_ERASED_CW_DETECT_CFG, N_MAX_ZEROS);                 \
    priv->erased_cw_det_cfg |=                                            \
      x << (HWIO_SHFT(NAND_ERASED_CW_DETECT_CFG, N_MAX_ZEROS));           \
  } while (0)
*/
 
/* Sets ECC enable bit to enable ECC, to be set in ECC CFG register  */
#define FLASH_ENABLE_BCH_ECC(priv)                                               \
  do {                                                                           \
    priv->ecc_cfg &= ~HWIO_FMSK(NAND_DEV0_ECC_CFG, ECC_DISABLE);                 \
    priv->ecc_cfg |= NAND_OPT_OFF << (HWIO_SHFT(NAND_DEV0_ECC_CFG, ECC_DISABLE));\
  } while (0)

/* Sets ECC disable bit to disable ECC, to be set in ECC CFG register  */
#define FLASH_DISABLE_BCH_ECC(priv)                                             \
  do {                                                                          \
    priv->ecc_cfg &= ~HWIO_FMSK(NAND_DEV0_ECC_CFG, ECC_DISABLE);                \
    priv->ecc_cfg |= NAND_OPT_ON << (HWIO_SHFT(NAND_DEV0_ECC_CFG, ECC_DISABLE));\
  } while (0)
  
/* Sets ecc mode to be set in ECC CFG register  */
#define FLASH_BCH_ECC_MODE(priv, x)                               \
  do {                                                            \
    priv->ecc_cfg &= ~HWIO_FMSK(NAND_DEV0_ECC_CFG, ECC_MODE);     \
    priv->ecc_cfg |= x << HWIO_SHFT(NAND_DEV0_ECC_CFG, ECC_MODE); \
  } while (0)

/* Sets the number of ecc parity bytes to be set in ECC CFG register  */
#define FLASH_BCH_ECC_SIZE(priv, x)                                        \
  do {                                                                     \
    priv->ecc_cfg &= ~HWIO_FMSK(NAND_DEV0_ECC_CFG, ECC_PARITY_SIZE_BYTES); \
    priv->ecc_cfg |=                                                       \
      x << HWIO_SHFT(NAND_DEV0_ECC_CFG, ECC_PARITY_SIZE_BYTES);            \
  } while (0)
 
/* Sets the number of bytes coveredby ecc logic, to be set in ECC CFG register  */
#define FLASH_BCH_ECC_DATA_BYTES(priv, x)                               \
  do {                                                                  \
    priv->ecc_cfg &= ~HWIO_FMSK(NAND_DEV0_ECC_CFG, ECC_NUM_DATA_BYTES); \
    priv->ecc_cfg |=                                                    \
      x << HWIO_SHFT(NAND_DEV0_ECC_CFG, ECC_NUM_DATA_BYTES);            \
  } while (0)

/* Byte size for ECC bytes check for erased page check.
   When ECC enabled on main only
 */
#define FLASH_ERASE_ECC_CHECK_SIZE1          16

#define FLASH_ERASE_4BIT_ECC_CHECK_SIZE1_8   15
#define FLASH_ERASE_4BIT_ECC_CHECK_SIZE1_16  16
#define FLASH_ERASE_8BIT_ECC_CHECK_SIZE1_8   19
#define FLASH_ERASE_8BIT_ECC_CHECK_SIZE1_16  20

/* Byte size for ECC bytes check for erased page check.
   When ECC enabled on main and spare
 */
#define FLASH_ERASE_ECC_CHECK_SIZE2          10

#define FLASH_ERASE_4BIT_ECC_CHECK_SIZE2_8   7
#define FLASH_ERASE_4BIT_ECC_CHECK_SIZE2_16  8
#define FLASH_ERASE_8BIT_ECC_CHECK_SIZE2_8   13
#define FLASH_ERASE_8BIT_ECC_CHECK_SIZE2_16  14

/* Spare udata length per codeword */
#define FLASH_HAL_SPARE_UDATA_LEN_PER_CW     4
#define FLASH_HAL_SPARE_UDATA_LEN_MAX        \
  (FLASH_HAL_SPARE_UDATA_LEN_PER_CW * 8)

/* Bailout time for NAND controller in us */
#define FLASH_NANDC_BAILOUT_TIME           14000

#define FLASH_ERASED_CW_DETECT_STATUS        0x40
#define FLASH_ERASED_PAGE_DETECT_STATUS      0x80
 

/* Return mask for post processing */
uint32 get_mask(uint32 num, uint32 clear_flag);

/* Init pre-defined sequence function pointers */
void hal_nandc_fptrs_init(struct flash_hal_ctlr_ops  *ctlr_ops);

/* Init pre-defined sequence function pointers for default(all) mode */
void hal_nandc_fptrs_init_default(struct flash_hal_ctlr_ops  *ctlr_ops);

/* Init pre-defined sequence function pointers for reduced read only mode */
void hal_nandc_fptrs_init_reduced(struct flash_hal_ctlr_ops  *ctlr_ops);

enum flash_hal_status hal_nandc_predef_cmd_ptrs_init(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *buff_data, uint32 *used_buff_size);
  
enum flash_hal_status hal_nandc_predef_cmd_ptrs_init_default(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *buff_data, uint32 *used_buff_size);

enum flash_hal_status hal_nandc_predef_cmd_ptrs_init_reduced(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *buff_data, uint32 *used_buff_size);

/* Indicates beginning of a flash HAL operation. */
void flash_hal_nand_begin_op(struct flash_hal_ctlr_data *priv_data,
  void *ws_buffer, uint32 ws_buf_len_bytes);

/* Indicates end of a flash HAL operation. */
void flash_hal_nand_end_op(struct flash_hal_ctlr_data *priv_data);

void hal_nandc_post_process_op_list(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_op_list *op_list, enum flash_hal_status *status);

void hal_nandc_transfer(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_cmd_seq *cmd_seq, enum flash_hal_status *status);

int hal_nandc_iface_init(void);

void hal_nandc_iface_deinit(void);

#endif /* FLASHI_HAL_NANDC_H */
