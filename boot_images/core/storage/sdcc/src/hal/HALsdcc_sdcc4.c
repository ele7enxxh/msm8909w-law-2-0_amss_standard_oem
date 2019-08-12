 /**********************************************************************
 * HALsdcc.c
 *
 * Secure Digital Card Controller (SDCC) Hardware Abstraction Layer
 * (HAL)
 *
 * This file implements the SDCC driver HAL components that are common
 * to all implementations of the SDCC4 core
 *
 * Copyright (c) 2008-2013
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History


when        who     what, where, why
---------------------------------------------------------------------- 
2013-06-13  bn      Added MCLK_REG_WR_ACTIVE check
2010-10-10  rh      Branch from HAL_sdcc2
2010-03-02  yg      Add DATA_END_BIT clean case to HAL_sdcc_StatusClear
2009-11-13  rh      Add FIFO block read support
2009-09-21  vin     Added HAL_SDCC_STATUS_TXACTIVE 
2009-07-31  vj      Added HAL_SDCC_STATUS_DATA_END case in status check
2009-07-16  vj      Clear the WIDEBUS field in HAL Init
2009-07-15  vin     Using SDIO interrupt bit SDIO_INTR_OPER
2009-06-30  vin     Added HAL_sdcc_IsIntEnabled and HAL_sdcc_SetInvertOut 
2009-05-07  vin     HAL_sdcc_DataTimeout simplified to just program MCI 
                    register
2009-04-07  vin     Removed INTLOCK and INTFREE
2009-02-16  vj      OS Abstraction Layer
2008-12-05  sr      HAL Adaptation for WM6.
2008-04-15  s h     Revised.
2008-01-29  umr     Initial version.
======================================================================*/

#include "HALsdcc.h"
#include "HALsdcc_sdcc4.h"
#include <string.h>

/* The "HAL Module Versioning" as per the HAL Specification */
#define HAL_SDCC_MAJOR_VER 0x02   /* 8 bits */
#define HAL_SDCC_MINOR_VER 0x01   /* 8 bits */
#define HAL_SDCC_BUILD_VER 0x0001 /* 16 bits */
#define HAL_SDCC_VERSION_STRING "SDCC4 HAL v2.0.1"

/* Mandatory Hal Module Versioning, but this is not used. */
#define HAL_SDCC_VERSION_CODE (( HAL_SDCC_MAJOR_VER << 24)  \
                               | (HAL_SDCC_MINOR_VER << 16)  \
                               | (HAL_SDCC_BUILD_VER << 0))

struct write_var {
   uint32 status_reg_addr;
   uint32 term_bitmsk;
   uint32 size;
   uint32 error_flag;
};

void hal_sdcc_memcpy32 (void * des, void * src);
uint32 hal_sdcc_memcpy_read (void * des, void * src, uint32 status_reg, uint32 mask);
uint32 hal_sdcc_memcpy_write (void * des, void * src, struct write_var *var);

void
HAL_sdcc_SetSdCmd (const uint32 nController, const struct HAL_sdcc_SdCmd * hal_cmd)
{
  uint32 command = 0;

  if (!hal_cmd)
    return;

  /* set the command */
  command = hal_cmd->cmd_index;

  /* Construct the command with options */
  if (hal_cmd->enable == TRUE)
    command |= MCI_CMD_ENABLED;

  /* CPSM waits for a response. */
  if (hal_cmd->response == TRUE)
    command |= MCI_CMD_RESPONSE;

  /*If True, receives a 136-bit long response.   */
  if (hal_cmd->longrsp == TRUE)
    command |= MCI_CMD_LONG_RESP;

  /* PROG_DONE status bit will be asserted when busy is
   * de-asserted */
  if (hal_cmd->prog_ena == TRUE)
    command |= MCI_CMD_PROG_ENABLED;

  /* indicates that this is a Command with Data */
  if (hal_cmd->dat_cmd == TRUE)
    command |= MCI_CMD_DAT_CMD;

  HWIO_SDC_REG_OUT (MCI_ARGUMENT, sdcc_reg_base[nController], hal_cmd->cmd_arg);
  HWIO_SDC_REG_OUT (MCI_CMD, sdcc_reg_base[nController], command);
  HAL_sdcc_WaitWrActive (nController); 
}


void
HAL_sdcc_DataControl (const uint32 nController, const struct HAL_sdcc_DataControl * dctl)
{
  uint32 HAL_data_ctrl = 0;

  HAL_data_ctrl = (dctl->block_size) << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT;

  /* Enable DM if requested */
  if (dctl->dm_enable)
    HAL_data_ctrl |= MCI_DATA_DMA_ENABLED;

  /*
   * Set Read / Write bit
   * 0 : controller to card - Write
   * 1 : card to controller - Read
   */
  if (dctl->direction == HAL_SDCC_DIR_READ)
    HAL_data_ctrl |= MCI_DATA_READ;

  /* 
   * Set Data transfer mode:
   * 0 : block data transfer
   * 1 : stream data transfer
   */
  if (dctl->mode != HAL_SDCC_DCTL_BLK_DATA_XFER)
    HAL_data_ctrl |= MCI_DATA_STREAM_MODE;

  /* Enable Data transfer if enabled. */
  if (dctl->data_xfer_enable)
    HAL_data_ctrl |= MCI_DATA_DPSM_ENABLED;

  /* start the DPSM */
  HWIO_SDC_REG_OUT (MCI_DATA_CTL, sdcc_reg_base[nController], HAL_data_ctrl);
  HAL_sdcc_WaitWrActive (nController); 
}


uint32
HAL_sdcc_GetIntState (const uint32 nController)
{
   return HWIO_SDC_REG_IN (MCI_INT_MASK0, sdcc_reg_base[nController]);
}

void
HAL_sdcc_EnableInt (const uint32 nController,
                    enum HAL_sdcc_IntSource int_cause)
{
  uint32 int_mask = 0;
  uint32 reg_value = 0;

  switch (int_cause)
  {
    case HAL_SDCC_INT_SOURCE_SDIO:
      int_mask = MCI_INT_MASK_SDIO_INTR_OPER;
      break;
    case HAL_SDCC_INT_SOURCE_PROG:
      int_mask = MCI_INT_MASK_PROG_DONE;
      break;
    case HAL_SDCC_INT_SOURCE_DATA_TIMEOUT:
      int_mask = MCI_INT_MASK_DATA_TIMEOUT;
      break;
    case HAL_SDCC_INT_SOURCE_SDIO_AND_PROG:
      int_mask = MCI_INT_MASK_PROG_DONE | MCI_INT_MASK_SDIO_INTR_OPER;
      break;
    case HAL_SDCC_INT_SOURCE_DATA_END:
      int_mask = MCI_INT_MASK_DATAEND;
      break;

    default:
      // ERROR
      return;
  }

  /* Enable selected interrupts */
  reg_value = HWIO_SDC_REG_IN (MCI_INT_MASK0, sdcc_reg_base[nController]);
  reg_value = reg_value | int_mask;
  reg_value = HWIO_SDC_REG_OUT (MCI_INT_MASK0, sdcc_reg_base[nController], reg_value);
}

uint32
HAL_sdcc_IsIntEnabled (const uint32 nController,
                       enum HAL_sdcc_IntSource nIntSource)
{
  uint32 int_mask = 0;
  uint32 reg_value = 0;

  switch (nIntSource)
  {
    case HAL_SDCC_INT_SOURCE_SDIO:
      int_mask = MCI_INT_MASK_SDIO_INTR_OPER;
      break;
    case HAL_SDCC_INT_SOURCE_PROG:
      int_mask = MCI_INT_MASK_PROG_DONE;
      break;
    case HAL_SDCC_INT_SOURCE_DATA_TIMEOUT:
      int_mask = MCI_INT_MASK_DATA_TIMEOUT;
      break;
    case HAL_SDCC_INT_SOURCE_SDIO_AND_PROG:
      int_mask = MCI_INT_MASK_PROG_DONE | MCI_INT_MASK_SDIO_INTR_OPER;
      break;
    case HAL_SDCC_INT_SOURCE_DATA_END:
      int_mask = MCI_INT_MASK_DATAEND;
      break;

    default:
      // ERROR
      return 0;
  }

  reg_value = HWIO_SDC_REG_IN (MCI_INT_MASK0, sdcc_reg_base[nController]);
  reg_value = reg_value & int_mask;
  return (reg_value == int_mask);
}

void
HAL_sdcc_DisableInt (const uint32 nController,
                     enum HAL_sdcc_IntSource int_cause,
                     uint32 * reg_value)
{
  uint32 int_mask = 0;

  switch (int_cause)
  {
    case HAL_SDCC_INT_SOURCE_SDIO:
      int_mask = MCI_INT_MASK_SDIO_INTR_OPER;
      break;
    case HAL_SDCC_INT_SOURCE_PROG:
      int_mask = MCI_INT_MASK_PROG_DONE;
      break;
    case HAL_SDCC_INT_SOURCE_DATA_TIMEOUT:
      int_mask = MCI_INT_MASK_DATA_TIMEOUT;
      break;
    case HAL_SDCC_INT_SOURCE_SDIO_AND_PROG:
      int_mask = MCI_INT_MASK_PROG_DONE | MCI_INT_MASK_SDIO_INTR_OPER;
      break;
    case HAL_SDCC_INT_SOURCE_DATA_END:
      int_mask = MCI_INT_MASK_DATAEND;
      break;

    default:
      // ERROR
      return;
  }

  /* disable Selected interrupts */
  *reg_value = HWIO_SDC_REG_IN (MCI_INT_MASK0, sdcc_reg_base[nController]);
  *reg_value = (*reg_value) & (~int_mask);
  HWIO_SDC_REG_OUT (MCI_INT_MASK0, sdcc_reg_base[nController], *reg_value);
}

void
HAL_sdcc_IntStateGet (const uint32 nController, uint32 * reg_value)
{
  *reg_value = HWIO_SDC_REG_IN (MCI_INT_MASK0, sdcc_reg_base[nController]);
}

void
HAL_sdcc_IntStateSet (const uint32 nController, uint32 reg_value)
{
  /* disable Selected interrupts */
  reg_value = HWIO_SDC_REG_OUT (MCI_INT_MASK0, sdcc_reg_base[nController], reg_value);
}

void
HAL_sdcc_FifoBlockRead (const uint32 nController, uint32 *pDataPt, uint32 *size)
{
   void * fifoAddr = NULL;
   uint32 ret;
   fifoAddr = (void *)(sdcc_reg_phys[nController] + HWIO_SDC_MCI_FIFO_REG_IDX);
   ret = hal_sdcc_memcpy_read (pDataPt, fifoAddr, 
                      sdcc_reg_base[nController] + HWIO_SDC_MCI_STATUS_REG_IDX,
                      HWIO_MCI_STATUS_RXFIFO_HALF_FULL_BMSK);
   *size -= (ret - (uint32)pDataPt);
}

void
HAL_sdcc_FifoBlockWrite (const uint32 nController, uint32 *pDataPt, uint32 *size)
{
   void * fifoAddr = NULL;
   uint32 ret;
   struct write_var wvar;
   fifoAddr = (void *)(sdcc_reg_phys[nController] + HWIO_SDC_MCI_FIFO_REG_IDX);

   // Fill in the bit checking mask register, used by the optimized memcpy
   // routine
   wvar.status_reg_addr = sdcc_reg_base[nController] + HWIO_SDC_MCI_STATUS_REG_IDX; 
   wvar.term_bitmsk = HWIO_MCI_STATUS_TXFIFO_HALF_FULL_BMSK;
   wvar.size = *size;
   wvar.error_flag = HAL_SDCC_DIR_WRITE_ERROR;

   ret = hal_sdcc_memcpy_write (fifoAddr, pDataPt, &wvar); 
   *size -= (ret - (uint32)pDataPt);
}

void 
HAL_sdcc_WaitWrActive (const uint32 nController)
{
   uint32 cnt = 1000;
   uint32 reg;
  
   while (cnt--)
   {
     reg = HWIO_SDC_REG_IN(MCI_STATUS2, sdcc_reg_base[nController]);
     if ((reg & HWIO_MCI_STATUS2_MCLK_REG_WR_ACTIVE_BMSK) == 0)
     {
       return; 
     }
   }
}
