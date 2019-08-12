/*====================================================================
 *
 * FILE:        flash_hal_nandc.c
 *
 * SERVICES:    APIs for QPIC NAND Controller
 *
 * DESCRIPTION: APIs for QPIC NAND Controller, which will be used by
 *              HAL common Layer
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/hal/qpic/flash_hal_nandc.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 06/13/14     sb      Update to support 9x45
 * 09/03/13     eo      Include target specific HWIO register mapping
 * 09/05/12     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flashi_hal_common.h"
#include "flashi_hal_nand.h"
#include "flashi_hal_nandc.h"

/*====================================================================
 *
 *          E X T E R N   D E C L A R A T I O N S
 *
 ====================================================================*/



/*====================================================================
 *
 *          S T A T I C   V A R I A B L E   D E F I N I T I O N S
 *
 ====================================================================*/

/* Erased page check should happen during post-processing
   With Data mover.
 */
static void hal_nandc_check_erased_page_dm(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_nand_buffer *spare_data, uint32 nandc_buffer,
  enum flash_hal_status *status)
{
}


/* Erased page check should happen during post-processing
     Without Data mover.
      - Verify that status register have OP_ERR
      - Compare ECC Bytes for 0xFF
      - Compare main data offset[] = 0x76
*/
static void hal_nandc_check_erased_page_ndm(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_nand_buffer *spare_data, uint32 nandc_buffer,
  enum flash_hal_status *status)
{
}

/*
 * This API does the post processing check for an erased page.
 */
static uint32 hal_nandc_post_process_erased_page(
  struct flash_hal_post_process_cmd *pp_list, uint32 max_remaining,
  enum flash_hal_status *status)
{
  uint32 li_count = 0;
  uint32 data_addr = 0;
  uint32 value = 0, mask = 0;
  int32 compare_cnt = 0;

  *status = FLASH_HAL_FAIL;

  /* If the post processing check for erased page is flagged,
     do the post processing check.

     We are doing a 3 point erased page check,
        1. Check the status register for OP_ERROR
        2. Check the main data for ECC corrected byte
        3. Check the ECC bytes for 0xFF

     These checks are entered to the post-processing list with cmd
     FLASH_HAL_ERASED_CHECK by the HAL controller read API.

     This API looks for all the above checks and return status
     accordingly.
   */
  if(FLASH_HAL_ERASED_CHECK == (pp_list->cmd & FLASH_HAL_ERASED_CHECK))
  {
    *status = FLASH_HAL_ERASED_PAGE;
  }

  /* In this API, we don't bother about what to compare against what.
     This API just checks that the address given contains expected value
     or not */
  while ((li_count < max_remaining) &&
         (FLASH_HAL_ERASED_CHECK == (pp_list->cmd & FLASH_HAL_ERASED_CHECK)))
  {

    /* Verify the the main data for ECC corrected byte */
    data_addr = pp_list->addr;

    value = *(uint32 *) data_addr;

    if (value != pp_list->value)
    {
      *status = FLASH_HAL_FAIL;
      break;
    }

    pp_list++;
    li_count++;

    /* Verify all ECC bytes */
    data_addr = pp_list->addr;
    compare_cnt = 10;

    while(compare_cnt > 0)
    {
      mask = pp_list->mask & get_mask(compare_cnt, 0);

      value = *(uint32 *) data_addr;

      if ((value & mask) != (pp_list->value & mask))
      {
        *status = FLASH_HAL_FAIL;
        break;
      }

      data_addr += 4;
      compare_cnt = compare_cnt - 4;
    }

    if (pp_list->op_len > FLASH_ERASE_ECC_CHECK_SIZE2)
    {
      /* Verify udata to ignore ECC error */
      data_addr = pp_list->addr;
      data_addr += 8;
      mask = pp_list->mask;

      compare_cnt = pp_list->op_len - 8;

      while(compare_cnt > 0)
      {
        if (compare_cnt < 4)
        {
          mask = mask & get_mask(compare_cnt, 0);
        }

        value = *(uint32 *) data_addr;

        if ((value & mask) != (pp_list->value & mask))
        {
          /* This means you have a page with udata alone */
          *status = FLASH_HAL_IGNORE_ERROR;

          break;
       }

        data_addr += 4;
        compare_cnt = compare_cnt - 4;
      }
    }

    pp_list++;
    li_count++;
  }

  return li_count;
}

/*
 * Post processes operation list after data mover processing.
 */
void hal_nandc_post_process_op_list(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_op_list *op_list, enum flash_hal_status *status)
{
  uint32 value = 0, erase_cw_count = 0;
  uint32 list_count = 0;
  uint32 pplist_to_skip = 1;
  struct flash_hal_post_process_cmd *pp_list;
  enum flash_hal_status tmp_status = FLASH_HAL_SUCCESS;

  *status = FLASH_HAL_SUCCESS;

  pp_list = &op_list->pp_list[0];

  /* Verify all the post process values */
  while ((list_count < op_list->pp_list_count) &&
         (*status != FLASH_HAL_FAIL))
  {
    if ((FLASH_HAL_DST_VERIFY == (pp_list->cmd & FLASH_HAL_DST_VERIFY)) &&
        (FLASH_HAL_ERASED_CHECK != (pp_list->cmd & FLASH_HAL_ERASED_CHECK)))
    {
      value = *(uint32 *) pp_list->addr;

      if ((value & pp_list->mask) != pp_list->value)
      {

        if (FLASH_NAND_STATUS_MPU_ERROR_MASK ==
            (value & (FLASH_NAND_STATUS_MPU_ERROR_MASK & pp_list->mask)))
        {
          *status = FLASH_HAL_FAIL_MPU_ERR;
          break;
        }

        /* If there is a failure, look for erased page */
        pplist_to_skip = hal_nandc_post_process_erased_page(pp_list + 1,
          (op_list->pp_list_count - list_count), &tmp_status);

        if (FLASH_HAL_ERASED_PAGE == tmp_status)
        {
          erase_cw_count++;
        }

        if (FLASH_HAL_FAIL == tmp_status)
        {
          *status = FLASH_HAL_FAIL;
        }
      }
    }

    list_count += pplist_to_skip;
    /* Skip the post-process list for all erased page check entry */
    pp_list += pplist_to_skip;
    pplist_to_skip = 1;
  }

  if (erase_cw_count == priv_data->cw_cnt)
  {
    *status = FLASH_HAL_ERASED_PAGE;
  }

  /* Keeping track of errors in HAL. Will help in debugging issues */
  if (FLASH_HAL_ERASED_PAGE == *status)
  {
    /* Set the erased page bit in hal status */
    hal_status |= FLASH_HAL_ERASED_PAGE_VALID;
  }
  else if (FLASH_HAL_FAIL_MPU_ERR == *status)
  {
    /* Set the post process error bit in hal status */
    hal_status |= FLASH_HAL_MPU_ERROR;
  }
  else if (FLASH_HAL_FAIL == *status)
  {
    /* Set the post process error bit in hal status */
    hal_status |= FLASH_HAL_PP_VERIFY_ERROR;
  }

}

/* Wrapper function for flash erased page check */
static __inline void hal_nandc_check_erased_page(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_nand_buffer *data, uint32 nandc_buffer,
  enum flash_hal_status *status)
{
  if (FLASH_HAL_DMA_ENABLE == hal_dma_state)
  {
    hal_nandc_check_erased_page_dm(priv_data, data, nandc_buffer, status);
  }
  else
  {
    hal_nandc_check_erased_page_ndm(priv_data, data, nandc_buffer, status);
  }
}

/* ====================================================================
 *
 *        F L A S H   H A L   C O N F I G U R A T I O N   A P I S
 *
 * ==================================================================*/

/*
 * Sets the address registers in NAND controller.
 */
static void hal_nandc_set_addr(struct flash_hal_ctlr_data *priv_data,
  uint32 num_addr, uint8 *addr)
{
  uint32 i = 0;
  uint32 addr_val = 0, addr_mask = 0;

  if (num_addr > FLASH_HAL_MAX_ADDR_CYCLES)
  {
    /* Set the invalid address error bit in hal status */
    hal_status |= FLASH_HAL_INVALID_ADDR_ERROR;
    return;
  }

  /* Set the ADDR0 register */
  while ((i < 4) && (i < num_addr))
  {
    addr_val |= (addr[i] << (8 * i));
    i++;
  }

  addr_mask = get_mask(num_addr, 1);
  priv_data->addr0 = (priv_data->addr0 & addr_mask) | addr_val;


  NANDC_OUT(NAND_ADDR0, priv_data->addr0);

  /* Set the ADDR1 register */
  addr_val = 0;
  addr_mask = 0;

  if (num_addr > 4)
  {
    while (i < num_addr)
    {
      addr_val |= (addr[i] << (8 * (i - 4)));
      i++;
    }

    addr_mask = get_mask(num_addr - 4, 1);
  }

  priv_data->addr1 = (priv_data->addr1 & addr_mask) | addr_val;

  NANDC_OUT(NAND_ADDR1, priv_data->addr1);

  /* Set number of address cycles and address */
  FLASH_NUM_ADDR_CYCLES(priv_data, num_addr);
}

/*
 * Wait function for an operation complete.
 */
static void hal_nandc_wait(uint32 cw_cnt, enum flash_hal_wait_t wait,
  uint32 mask, uint32 bail_out_time)
{
  if (FLASH_HAL_DMA_ENABLE == hal_dma_state)
  {
    uint32 status = 1;
    struct flash_hal_post_process_cmd * pp_list=NULL;

   while (status & 0x1F)
   {
    flash_hal_get_next_ws_addr(&status, 1);
   }

    NANDC_INS(NAND_FLASH_STATUS, status,
      FLASH_HAL_DST_VERIFY|FLASH_HAL_SRC_SYNC|FLASH_HAL_DEST_VIRTUAL);

    /* Set the post processing list */
    flash_hal_get_next_pp_addr(&pp_list);

    pp_list->cmd = FLASH_HAL_DST_VERIFY;
    pp_list->addr = status;
    pp_list->op_len = 4;
    pp_list->mask = mask;
    pp_list->value = (cw_cnt << FLASH_NAND_STATUS_CW_CNTR_SHFT) | 0x20;
  }
  else
  {
    uint32          count  = 0;
    volatile uint32 status = 0;

    while (count < bail_out_time)
    {
      /* Read the status register and check for complete */
      NANDC_IN(NAND_FLASH_STATUS, &status);

      if (status & FLASH_NAND_STATUS_OP_STATUS_MASK)
      {
        FLASH_HAL_BUSY_WAIT(FLASH_NAND_POLL_TIME);
        count++;
      }
      else
      {
        break;
      }
    }

    if (count == bail_out_time)
    {
      /* Set the op timeout error bit in hal status */
      hal_status |= FLASH_HAL_OP_TIMEOUT;
    }
  }
}

/*
 * Private function used for read operation with QPIC NANDC.
 */
static void hal_nandc_use_read(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_nand_cmd *cmd_seq1, struct flash_hal_nand_cmd *cmd_seq2,
  enum flash_hal_status *status)
{
  /* TBD: Need to reduce the function size */

  uint32 cmd;
  uint32 new_vld = 0;
  uint32 data_len_bytes = 0;
  int32  cw_count = 0;
  int32  remaining_data = 0;
  uint32 bail_out_time = 0;
  uint32 nandc_buffer = 0;
  struct flash_hal_nand_buffer *data = NULL;
  enum flash_hal_wait_t wait = FLASH_HAL_WAIT_NONE;
  enum flash_hal_status tmp_status = FLASH_HAL_SUCCESS;

  *status = tmp_status;

  nandc_buffer = (FLASH_HAL_DMA_ENABLE == hal_dma_state)?
    HAL_QPIC_NANDC_NANDC_BUFFER_PADDR : ((uint32)HAL_QPIC_NANDC_NANDC_BUFFER_VADDR);

  /* First sequence can't be NULL */
  if (NULL == cmd_seq1)
  {
    *status = FLASH_HAL_FAIL;
    return;
  }

  /* Extract the config info from the sequence */
  if (FLASH_HAL_DATA_OUT_FROM_DEVICE == cmd_seq1->data_dir)
  {
    data_len_bytes = cmd_seq1->max_op_data_len;
    data = cmd_seq1->data_buff;
    wait = cmd_seq1->wait_mech.wait_type;
    bail_out_time = cmd_seq1->wait_mech.timeout;
  }
  else if (NULL != cmd_seq2)
  {
    if (FLASH_HAL_DATA_OUT_FROM_DEVICE == cmd_seq2->data_dir)
    {
      data_len_bytes = cmd_seq2->max_op_data_len;
      data = cmd_seq2->data_buff;
    }

    wait = cmd_seq2->wait_mech.wait_type;
    bail_out_time = cmd_seq2->wait_mech.timeout;
  }

  /* Set the command and vld registers */
  cmd = cmd_seq1->cmd;

  if (NULL != cmd_seq2)
  {
    cmd |= cmd_seq2->cmd << 8;
    new_vld = priv_data->dev_vld;
  }
  else
  {
    /* Disable Read start command */
    new_vld = priv_data->dev_vld &
      ~(HWIO_FMSK(FLASH_DEV_CMD_VLD, READ_START_VLD));
  }

  if (FLASH_NAND_READ_DEFAULT_CMD != cmd)
  {
    /* Replace CMD register */
    NANDC_OUT(FLASH_DEV_CMD1,
      ((FLASH_DEV_CMD1_DEFAULT_CMD & FLASH_DEV_CMD1_READ_CMD_MSK) | cmd));
  }

  /* Determine the cw count and set config registers */
  /* Optimization Needed */
  cw_count = (data_len_bytes > 0) ? (data_len_bytes - 1): data_len_bytes;
  cw_count = cw_count / FLASH_NAND_BYTES_PER_CW;

  /* Set the codeword */
  FLASH_CW_PER_PAGE(priv_data, cw_count);

  if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
  {
    FLASH_SELECT_BCH_ECC(priv_data);
  }
  else
  {
    FLASH_SELECT_RS_ECC(priv_data);
  }

  cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
    FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_ALL_CMD;

  /* Configure registers */
  NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
  NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);

  if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
  {
    NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);
  }

  NANDC_OUT(EBI2_ECC_BUF_CFG, priv_data->ecc_buf);
  NANDC_OUT(FLASH_DEV_CMD_VLD, new_vld);

  remaining_data = data_len_bytes;

  /* This loop, loops until all the required data read from the device */
  do
  {
    if ((remaining_data < FLASH_NAND_BYTES_PER_CW) &&
      (FLASH_HAL_HW_ECC_DISABLE == priv_data->ecc_state))
    {
      /* This configure is to read only the required number of bytes */
      /* We are rounding the remaining data to next even number so
         that the same configuration works for a x16 device too */
      remaining_data = (remaining_data & 0xFFFFFFFE) + 0x2;

      FLASH_UD_SIZE(priv_data, remaining_data);
      FLASH_SPARE_SIZE_BYTES(priv_data, 0x0);
      if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
      {
        FLASH_BCH_ECC_DATA_BYTES(priv_data, remaining_data);
        FLASH_BCH_ECC_SIZE(priv_data, 0);
      }
      else
      {
        FLASH_ECC_SIZE(priv_data, 0x0);
      }
      NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
      FLASH_BAD_BLOCK_IN_SPARE_AREA(priv_data);
      FLASH_BAD_BLOCK_BYTE_NUM(priv_data, 0x0);
      NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
	  if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
      {
        NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);
      }

    }

    NANDC_OUTS(NAND_FLASH_CMD, cmd, FLASH_HAL_DST_SYNC);
    if (FLASH_HAL_DMA_DISABLE == hal_dma_state)
    {
       NANDC_OUT(NAND_FLASH_STATUS, FLASH_NAND_STATUS_OP_STATUS_MASK);
    }
    NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);
    hal_nandc_wait (cw_count, wait, FLASH_NANDC_STATUS_MASK, bail_out_time);

    /* Currently we are not allowing to copy to more than 2 buffers
       from a codeword buffer */

    /* Copy Main data or to the single data buffer */
    if ((NULL != data) && (data->buf_len))
    {
      NANDC_COPY_FROM_DEVICE(data->buf_ptr, nandc_buffer+data->cw_offset,
        data->buf_len);
    }

    data++;

    if ((NULL != data) && (data->buf_len))
    {
      NANDC_COPY_FROM_DEVICE(data->buf_ptr, nandc_buffer+data->cw_offset,
        data->buf_len);
    }

    /* Check for Erased page */
    if ((NULL != data) && (data_len_bytes == priv_data->total_page_size) &&
      (priv_data->ecc_state != FLASH_HAL_HW_ECC_DISABLE))
    {
      hal_nandc_check_erased_page(priv_data, data, nandc_buffer, &tmp_status);
    }

    data++;

    if (FLASH_HAL_FAIL == tmp_status)
    {
      *status = FLASH_HAL_FAIL;
    }

    remaining_data -= FLASH_NAND_BYTES_PER_CW;
  } while (--cw_count >= 0);

  /* Restore the command back */
  if (FLASH_NAND_READ_DEFAULT_CMD != cmd)
  {
    NANDC_OUT(FLASH_DEV_CMD1, FLASH_DEV_CMD1_DEFAULT_CMD);
  }

  /* Restore the command vld back */
  if (new_vld != priv_data->dev_vld)
  {
    NANDC_OUT(FLASH_DEV_CMD_VLD, priv_data->dev_vld);
  }

  if (FLASH_HAL_FAIL != *status)
  {
    *status = tmp_status;
  }
}


/*
 * Private function used write read operation with QPIN NANDC.
 */
static void hal_nandc_use_write(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_nand_cmd *cmd_seq1, struct flash_hal_nand_cmd *cmd_seq2,
  enum flash_hal_status *status)
{
  /* TBD: Need to reduce the function size */

  uint32 cmd;
  uint32 data_len_bytes = 0;
  int32  cw_count = 0;
  uint32 bail_out_time = 0;
  enum flash_hal_wait_t wait;
  struct flash_hal_nand_buffer *data;
  uint32 nandc_buffer = 0;

  /* if the write buffer not initialized, return */
  if (nand_write_buff == 0)
  {
    *status = FLASH_HAL_FAIL;

    /* Set the buffer error bit in hal status */
    hal_status |= FLASH_HAL_NO_BUFFER_ERROR;
    return;
  }

  nandc_buffer = (FLASH_HAL_DMA_ENABLE == hal_dma_state)?
    HAL_QPIC_NANDC_NANDC_BUFFER_PADDR : ((uint32)HAL_QPIC_NANDC_NANDC_BUFFER_VADDR);

  if (FLASH_HAL_DATA_NONE == cmd_seq1->data_dir)
  {
    data_len_bytes = cmd_seq2->max_op_data_len;

    data = cmd_seq2->data_buff;
    wait = cmd_seq2->wait_mech.wait_type;
    bail_out_time = cmd_seq2->wait_mech.timeout;
  }
  else
  {
    data_len_bytes = cmd_seq1->max_op_data_len;
    data = cmd_seq1->data_buff;
    wait = cmd_seq1->wait_mech.wait_type;
    bail_out_time = cmd_seq1->wait_mech.timeout;
  }


  /* Set the command Register */
  cmd = cmd_seq1->cmd | (cmd_seq2->cmd << 8);

  if (FLASH_NAND_WRITE_DEFAULT_CMD != cmd)
  {
    uint32 new_cmd = ((FLASH_DEV_CMD0_DEFAULT_CMD &
      FLASH_DEV_CMD0_WRITE_CMD_MSK) | (cmd << 16));

    /* Replace CMD register */
    NANDC_OUT(FLASH_DEV_CMD0, new_cmd);
  }

  /* Determine the cw count and set config registers -
     We are going to write entire codeword, even if we have less
     number of bytes to write. Not going to write any partial
     coewords
  */
  cw_count = (data_len_bytes > 0) ? (data_len_bytes - 1): data_len_bytes;
  cw_count = cw_count / FLASH_NAND_BYTES_PER_CW;

  /* Set the codeword */
  FLASH_CW_PER_PAGE(priv_data, cw_count);
  if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
  {
    FLASH_SELECT_BCH_ECC(priv_data);
  }
  else
  {
    FLASH_SELECT_RS_ECC(priv_data);
  }

  cmd = (FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE == priv_data->ecc_state)?
    FLASH_NAND_PRG_PAGE_CMD:FLASH_NAND_PRG_PAGE_ALL_CMD;

  NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
  NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
  if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
  {
    NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);
  }

  NANDC_OUT(EBI2_ECC_BUF_CFG, priv_data->ecc_buf);

  /* This loop, loops until all the required data read from the device */
  do
  {
    uint32 bytes_copied = 0, align_len = 0, temp_write_addr = 0;

    /*----------------------------------------------------------
              FRAME THE NANDC BUFFER ACCORDING TO GIVEN
                       MAIN AND SPARE DATA
      ----------------------------------------------------------*/

    /* Copy Main Data */
    if (data->buf_len)
    {
      align_len = data->buf_len & 0xFFFFFFFC;

      if (align_len)
      {
        NANDC_COPY_TO_DEVICE(nandc_buffer+data->cw_offset,
          data->buf_ptr, align_len);
        bytes_copied = align_len;
      }

      if (align_len < data->buf_len)
      {
        /* Get a temperory buffer and set the partial dword */
        flash_hal_get_next_ws_addr(&temp_write_addr, 1);
        *(uint32 *) temp_write_addr = 0xFFFFFFFF;

        flash_hal_memcpy((void *) temp_write_addr, (uint8 *)data->buf_ptr +
          align_len, data->buf_len - align_len);

        NANDC_COPY_TO_DEVICE((nandc_buffer + bytes_copied), temp_write_addr, 4);

        bytes_copied += 4;
      }
    }

    /* Pad the remaining main data bytes (if any) with 0xFF */
    if (bytes_copied < data->buf_copy_len)
    {
      NANDC_COPY_TO_DEVICE((nandc_buffer + bytes_copied), nand_write_buff,
        (data->buf_copy_len - bytes_copied));
      bytes_copied += data->buf_copy_len - bytes_copied;
    }

    data++;

    /* Copy spare bytes */
    if (data->buf_len)
    {
      align_len = data->buf_len & 0xFFFFFFFC;

      if (align_len)
      {
        NANDC_COPY_TO_DEVICE(nandc_buffer+bytes_copied, data->buf_ptr,
          align_len);
        bytes_copied += align_len;
      }

      if (align_len < data->buf_len)
      {
        /* Get a temperory buffer and set the partial dword */
        flash_hal_get_next_ws_addr(&temp_write_addr, 1);
        *(uint32 *) temp_write_addr = 0xFFFFFFFF;

        flash_hal_memcpy((void *) temp_write_addr,
          (void *)(((uint8 *)data->buf_ptr)+align_len),
          data->buf_len - align_len);

        NANDC_COPY_TO_DEVICE((nandc_buffer + bytes_copied), temp_write_addr, 4);

        bytes_copied += 4;
      }
    }

    /* Pad the remaining spare data bytes (if any) with 0xFF */
    if (bytes_copied < FLASH_NAND_BYTES_PER_CW)
    {
      NANDC_COPY_TO_DEVICE(nandc_buffer+bytes_copied, nand_write_buff,
        (FLASH_NAND_BYTES_PER_CW-bytes_copied));

      if (FLASH_HAL_HW_ECC_DISABLE == priv_data->ecc_state)
      {
        /* This configure is to write only the required number of bytes */
        FLASH_UD_SIZE(priv_data, bytes_copied);
        FLASH_SPARE_SIZE_BYTES(priv_data, 0x0);
        if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
        {
          FLASH_BCH_ECC_DATA_BYTES(priv_data, bytes_copied);
          FLASH_BCH_ECC_SIZE(priv_data, 0);
        }
        else
        {
          FLASH_ECC_SIZE(priv_data, 0x0);
        }
        NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
        FLASH_BAD_BLOCK_IN_SPARE_AREA(priv_data);
        FLASH_BAD_BLOCK_BYTE_NUM(priv_data, 0x0);
        NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
        if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
        {
          NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);
        }

        cmd = FLASH_NAND_PRG_PAGE_CMD;
      }
    }

    data++;

    if (FLASH_HAL_DMA_DISABLE == hal_dma_state)
    {
       NANDC_OUT(NAND_FLASH_STATUS, FLASH_NAND_STATUS_OP_STATUS_MASK);
    }
    NANDC_OUTS(NAND_FLASH_CMD, cmd, FLASH_HAL_DST_SYNC);
    NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);
    hal_nandc_wait(cw_count, wait, FLASH_NANDC_STATUS_MASK, bail_out_time);

    /* Check the status bytes for any error. Here it is done only
       for the non-dm case. For DM, it will be done during
       post-processing */
    if ((FLASH_HAL_HW_ECC_DISABLE != priv_data->ecc_state) &&
        (FLASH_HAL_DMA_ENABLE != hal_dma_state))
    {
      uint32 flash_status = 0;
      NANDC_IN(NAND_FLASH_STATUS, &flash_status);

      if (flash_status & FLASH_NAND_STATUS_OP_ANY_ERR_MASK)
      {
        *status = FLASH_HAL_FAIL;
      }
    }

    bytes_copied = 0;

  } while (--cw_count >= 0);

  if (FLASH_NAND_WRITE_DEFAULT_CMD != cmd)
  {
     /* Replace CMD register */
     NANDC_OUT(FLASH_DEV_CMD0, FLASH_DEV_CMD0_DEFAULT_CMD);
  }

  *status = FLASH_HAL_SUCCESS;
}

/*
 * Generic function to execute a single command.
 */
static void hal_nandc_exec_single_cmd(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_nand_cmd *cmd_seq, enum flash_hal_status *status)
{
  /* TBD: Need to reduce the function size */

  uint32 cmd, new_cmd;
  uint32 mask = FLASH_NANDC_STATUS_MASK;
  uint32 data_cycle = FLASH_HAL_DATA_NONE;
  uint32 data_len_bytes, bail_out_time = 0;
  enum flash_hal_wait_t wait;

  /* Execute single cmd cycle sequence */
  data_cycle = cmd_seq->data_dir;
  data_len_bytes = cmd_seq->data_buff[0].buf_len;
  cmd = cmd_seq->cmd;
  wait = cmd_seq->wait_mech.wait_type;
  bail_out_time = cmd_seq->wait_mech.timeout;

  if (FLASH_HAL_DATA_OUT_FROM_DEVICE == data_cycle)
  {
    /* Set ADDR0 and ADDR1 values */
    hal_nandc_set_addr(priv_data, cmd_seq->num_addr_cycles,
      &cmd_seq->addr[0]);

    /* Read */
    if (data_len_bytes <= 4)
    {
      /* Use Read ID sequence to simulate */
      if (FLASH_NAND_READ_ID_DEFAULT_CMD != cmd)
      {
        /* Ignoring MPU errors for default status command */
        if (FLASH_NAND_STATUS_DEFAULT_CMD == cmd)
        {
          mask = FLASH_NANDC_IGNORE_MPU_STATUS_MASK;
        }

        /* Replace CMD register */
        new_cmd = ((FLASH_DEV_CMD2_DEFAULT_VALUE &
          FLASH_DEV_CMD2_ID_CMD_MSK) | cmd);

        NANDC_OUT(FLASH_DEV_CMD2, new_cmd);
      }

      /* Set the codeword to zero */
      FLASH_CW_PER_PAGE(priv_data, 0);
      if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
      {
        FLASH_SELECT_BCH_ECC(priv_data);
      }
      else
      {
        FLASH_SELECT_RS_ECC(priv_data);
      }

      /* Execute ID command */
      NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
      NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
      if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
      {
        NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);
      }	
      NANDC_OUT(FLASH_DEV_CMD_VLD, priv_data->dev_vld);
      NANDC_OUT(EBI2_ECC_BUF_CFG, priv_data->ecc_buf);

      if (FLASH_HAL_DMA_DISABLE == hal_dma_state)
      {
          NANDC_OUT(NAND_FLASH_STATUS, FLASH_NAND_STATUS_OP_STATUS_MASK);
      }

      NANDC_OUTS(NAND_FLASH_CMD, FLASH_NAND_ID_FETCH_CMD,
        FLASH_HAL_DST_SYNC);
      NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);

      hal_nandc_wait(0, wait, mask, bail_out_time);

      NANDC_INS(NAND_FLASH_READ_ID, cmd_seq->data_buff[0].buf_ptr,
        FLASH_HAL_DEST_VIRTUAL);

      if (FLASH_NAND_READ_ID_DEFAULT_CMD != cmd)
      {
        NANDC_OUT(FLASH_DEV_CMD2, FLASH_DEV_CMD2_DEFAULT_VALUE);
      }
    }
    else
    {
      hal_nandc_use_read(priv_data, cmd_seq, NULL, status);
    }
  }
  else if (FLASH_HAL_DATA_INTO_DEVICE == data_cycle)
  {
    /* TBD: Write */
    /* Use Write 528B page sequence to simulate */
    *status = FLASH_HAL_FAIL;
    /* Set the unsupported op bit in hal status */
    hal_status |= FLASH_HAL_OP_NOT_SUPPORTED_ERROR;

    return;
  }
  else
  {
    uint32 cs_value = 0;

    /* Use Reset sequence to simualte */
    if (FLASH_NAND_RESET_DEFAULT_CMD != cmd)
    {
      /* Replace CMD register */
      new_cmd = ((FLASH_DEV_CMD2_DEFAULT_VALUE &
        FLASH_DEV_CMD2_RESET_CMD_MSK) | (cmd << 16));

      NANDC_OUT(FLASH_DEV_CMD2, new_cmd);
    }

    /* Set the codeword to zero */
    FLASH_CW_PER_PAGE(priv_data, 0);

    /* Execute ID command */

    /* Write to Chipselect register */
    cs_value = (priv_data->chip_sel & 0x1);
  
    if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
    {
      FLASH_SELECT_BCH_ECC(priv_data);
    }
    else
    {
      FLASH_SELECT_RS_ECC(priv_data);
    }

    NANDC_OUT(NAND_FLASH_CHIP_SELECT, cs_value);
    NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
    NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
    if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
    {
      NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);
    }

    NANDC_OUT(FLASH_DEV_CMD_VLD, priv_data->dev_vld);
    NANDC_OUT(EBI2_ECC_BUF_CFG, priv_data->ecc_buf);

    /* Ensure address registers are set for single cmds */
    if (cmd_seq->num_addr_cycles)
    {
      /* Set ADDR0 and ADDR1 values */
      hal_nandc_set_addr(priv_data, cmd_seq->num_addr_cycles,
        &cmd_seq->addr[0]);
    }

    if (FLASH_HAL_DMA_DISABLE == hal_dma_state)
    {
       NANDC_OUT(NAND_FLASH_STATUS, FLASH_NAND_STATUS_OP_STATUS_MASK);
    }
    NANDC_OUTS(NAND_FLASH_CMD, FLASH_NAND_RESET_FLASH_CMD,
      FLASH_HAL_DST_SYNC);
    NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);

    hal_nandc_wait(0, wait, mask, bail_out_time);

    /* Restore the command register */
    if (FLASH_NAND_RESET_DEFAULT_CMD != cmd)
    {
      NANDC_OUT(FLASH_DEV_CMD2, FLASH_DEV_CMD2_DEFAULT_VALUE);
    }
  }

  *status = FLASH_HAL_SUCCESS;
}

/*
 * Generic function to execute a command pair.
 */
static void hal_nandc_exec_pair_cmd(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_nand_cmd *cmd_seq, enum flash_hal_status *status)
{
  struct flash_hal_nand_cmd *cmd_seq0 = cmd_seq;
  struct flash_hal_nand_cmd *cmd_seq1 = (cmd_seq + 1);
  uint32 cmd;
  uint32 new_cmd;
  uint32 data_dir = FLASH_HAL_DATA_NONE;
  uint32 bail_out_time = 0;
  enum flash_hal_wait_t wait;

  cmd = cmd_seq0->cmd | (cmd_seq1->cmd << 8);

  if (FLASH_HAL_DATA_NONE == cmd_seq0->data_dir)
  {
    data_dir = cmd_seq1->data_dir;
  }
  else
  {
    /* Data operation in first sequence? - TBD */
    FLASHLOG(4,("hal_nandc_exec_pair_cmd: "
                "Unsupported command sequence!!\n"));
    *status = FLASH_HAL_FAIL;

    /* Set the unsupported op bit in hal status */
    hal_status |= FLASH_HAL_OP_NOT_SUPPORTED_ERROR;

    return;
  }

  if (0 == cmd_seq1->num_addr_cycles)
  {
    /* Set address registers */
    hal_nandc_set_addr(priv_data, cmd_seq0->num_addr_cycles,
      &cmd_seq0->addr[0]);
  }
  else
  {
    /* Address cycle in second sequence ? */
    FLASHLOG(4,("hal_nandc_exec_pair_cmd: "
               "Unsupported command sequence!!\n"));

    /* Set the unsupported op bit in hal status */
    hal_status |= FLASH_HAL_OP_NOT_SUPPORTED_ERROR;

    *status = FLASH_HAL_FAIL;
    return;
  }

  /*
   *-------------------------------------------------------------------------
   *   Simulate the operations
   *-------------------------------------------------------------------------
   */

  /* data out of device:use read */
  if (FLASH_HAL_DATA_OUT_FROM_DEVICE == data_dir)
  {
    hal_nandc_use_read(priv_data, cmd_seq, cmd_seq+1, status);
  }
  /* data into device:use write */
  else if (FLASH_HAL_DATA_INTO_DEVICE == data_dir)
  {
    hal_nandc_use_write(priv_data, cmd_seq, cmd_seq+1, status);
  }
  /* No data: use erase */
  else if (FLASH_HAL_DATA_NONE == data_dir)
  {
    if (FLASH_NAND_ERASE_DEFAULT_CMD != cmd)
    {
      /* Replace CMD register */
      new_cmd = (
        (FLASH_DEV_CMD0_DEFAULT_CMD & FLASH_DEV_CMD0_ERASE_CMD_MSK) | cmd);
      NANDC_OUT(FLASH_DEV_CMD0, new_cmd);
    }

    wait = cmd_seq1->wait_mech.wait_type;
    bail_out_time = cmd_seq1->wait_mech.timeout;

    /* Set the UD size and Codeword count to zero */
    FLASH_UD_SIZE(priv_data, 0);
    FLASH_CW_PER_PAGE(priv_data, 0);
    if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
    {
      FLASH_SELECT_BCH_ECC(priv_data);
    }
    else
    {
      FLASH_SELECT_RS_ECC(priv_data);
    }

    NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
    NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
    if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
    {
      FLASH_BCH_ECC_DATA_BYTES(priv_data, 0);
      NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);
    }

    NANDC_OUT(EBI2_ECC_BUF_CFG, priv_data->ecc_buf);
    if (FLASH_HAL_DMA_DISABLE == hal_dma_state)
    {
       NANDC_OUT(NAND_FLASH_STATUS, FLASH_NAND_STATUS_OP_STATUS_MASK);
    }
    NANDC_OUTS(NAND_FLASH_CMD, FLASH_NAND_BLOCK_ERASE_CMD,
      FLASH_HAL_DST_SYNC);
    NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);

    hal_nandc_wait(0, wait, FLASH_NANDC_STATUS_MASK, bail_out_time);

    /* Restore the command register */
    if (FLASH_NAND_ERASE_DEFAULT_CMD != cmd)
    {
      NANDC_OUT(FLASH_DEV_CMD0, FLASH_DEV_CMD0_DEFAULT_CMD);
    }

    *status = FLASH_HAL_SUCCESS;
  }
}

/*
 * Generic function to process a command sequence.
 */
void hal_nandc_process_cmd(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_nand_cmd *cmd_seq, enum flash_hal_status *status)
{
  uint32 cmd_is_last = FALSE;
  uint32 cmd_pair_is_exec = FALSE;
  enum flash_hal_wait_t wait;
  enum flash_hal_status temp_status = FLASH_HAL_SUCCESS;

  *status = FLASH_HAL_SUCCESS;

  do
  {
    cmd_is_last = (cmd_seq->op_flag & FLASH_HAL_LAST_CMD) ? TRUE : FALSE;
    wait = cmd_seq->wait_mech.wait_type;
    cmd_pair_is_exec = ((cmd_is_last) || (wait != FLASH_HAL_WAIT_NONE)) ?
      FALSE : TRUE;

    if (cmd_pair_is_exec)
    {
      /* Execute cmd pair cycle sequence */
      hal_nandc_exec_pair_cmd(priv_data, cmd_seq, &temp_status);

      cmd_is_last = ((cmd_seq + 1)->op_flag & FLASH_HAL_LAST_CMD) ?
        TRUE : FALSE;

      cmd_seq += 2;
    }
    else
    {
      /* Execute single cmd cycle sequence */
      hal_nandc_exec_single_cmd(priv_data, cmd_seq, &temp_status);

      cmd_seq += 1;
    }

    if(temp_status != FLASH_HAL_SUCCESS)
    {
      *status = temp_status;
    }
  } while (!cmd_is_last);
}

/*
 * This function processes the command sequence created.
 */
void hal_nandc_transfer(struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_cmd_seq *cmd_seq, enum flash_hal_status *status)
{
  struct flash_hal_nand_cmd *nand_cmd_seq =
    (struct flash_hal_nand_cmd *)cmd_seq->cmd_seq;
  uint32 cmd_op_is_last = FALSE;
  enum flash_hal_status temp_status = FLASH_HAL_SUCCESS;

  /* Initialize the status with Success */
  *status = FLASH_HAL_SUCCESS;

  if (nand_cmd_seq)
  {
    do
    {
      /* Process command executes all command till the next
       * last command flag
       */
      hal_nandc_process_cmd(priv_data, nand_cmd_seq, &temp_status);
      SKIP_TO_LAST_CMD(nand_cmd_seq);

      cmd_op_is_last = (nand_cmd_seq->op_flag & FLASH_HAL_LAST_OP) ?
        TRUE : FALSE;

      if(temp_status != FLASH_HAL_SUCCESS)
      {
        *status = temp_status;
      }

      ++nand_cmd_seq;
      /* Loop breaks when last operation reached or any command fails */
    } while (FALSE == cmd_op_is_last);
  }

  if ((FLASH_HAL_FAIL == *status) || 
      ((FLASH_HAL_NO_ERROR != hal_status) &&
       (FLASH_HAL_ERASED_PAGE_VALID != hal_status)))
  {
    /* Set the op failed bit in hal status */
    hal_status |= FLASH_HAL_OP_FAILED;
    *status = FLASH_HAL_FAIL;
  }
}
