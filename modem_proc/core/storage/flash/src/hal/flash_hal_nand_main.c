/*====================================================================
 *
 * FILE: flash_hal_nand_main.c
 *
 * SERVICES: Interface for Flash HAL
 *
 * DESCRIPTION: This file contain NAND interface APIs for Flash HAL
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/flash_hal_nand_main.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/05/12     sv      cleanup / adapt to page layout changes
 * 01/13/11     sb      Adding 8bit ECC support
 * 11/24/09     jz      Fixed compiler warnings
 * 09/25/09     mh      Add support for probing device using second nand ctlr
 * 07/21/09     sv      Use flash_hal wrapper api's for memory operations
 * 03/30/08     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_hal_main.h"
#include "flashi_hal_nand.h"
#include "flashi_hal_common.h"
#include "flash_hal_reg_list.h"

/*====================================================================
 *
 *          S T A T I C   V A R I A B L E   D E F I N I T I O N S
 *
 ====================================================================*/

/* NAND operation sequence */
static struct flash_hal_nand_cmd  
  flash_hal_nand_op_seq[FLASH_HAL_NAND_OP_SEQ_MAX];

/* Write buffer address */
uint32 nand_write_buff = 0;

/* ====================================================================
 *
 *   F L A S H   H A L   O P E R A T I O N   C O M M O N   A P I S
 *
 * ==================================================================*/

/*
 * Indicates beginning of a flash HAL operation.
 */
void flash_hal_nand_begin_op(struct flash_hal_ctlr_data *priv_data,
  void *ws_buffer, uint32 ws_buf_len_bytes)
{
  /* Init sequence related parameters for the given dev_id */
  priv_data->seq_cnt = 0;
  priv_data->cur_seq_cnt = -1;

  /* Clear the operation sequence structure */
  flash_hal_memset(flash_hal_nand_op_seq, 0x0, sizeof(flash_hal_nand_op_seq));
  flash_hal_op_seq.cmd_seq = (void *) &flash_hal_nand_op_seq[0];

  nand_write_buff = 0;

  /* Allocate code word size bytes for write buffer from the pool */
  flash_hal_get_next_ws_addr(&nand_write_buff,
    (priv_data->cw_size_bytes/sizeof(uint32)));
  
  /* Set the write buffer to 0xFF */
  if (nand_write_buff)
  {
    flash_hal_memset((void *)nand_write_buff, 0xFF, priv_data->cw_size_bytes);
  }
}

/*
 * Indicates end of a flash HAL operation.
 */
void flash_hal_nand_end_op(struct flash_hal_ctlr_data *priv_data)
{
  struct flash_hal_nand_cmd *nand_seq;

  /* Ends the sequence with flag HAL_LAST_OP */
  nand_seq = (struct flash_hal_nand_cmd *) flash_hal_op_seq.cmd_seq;

  nand_seq[priv_data->cur_seq_cnt].op_flag |= FLASH_HAL_LAST_OP;
  priv_data->seq_cnt = priv_data->cur_seq_cnt + 1;
}

/*
 * Maps the main and spare data to the command buffer pointer
 */
void flash_hal_nand_map_buffers(struct flash_hal_ctlr_data *ptPrivData,
   struct flash_hal_nand_cmd *cmd, struct flash_hal_page_data *page_data,
   struct flash_hal_page_layout *page_layout)
{
  uint32 page_unit_count = 0;
  uint32 main_addr = 0;
  uint32 spare_addr = 0;
  uint32 main_len = 0;
  uint32 spare_len = 0;
  uint32 bytes_to_read = 0;
  struct flash_hal_nand_buffer *buf_ptr;
  const struct flash_hal_page_unit_layout *page_unit_layout;
  uint32 page_unit_layout_cnt = 0;

  main_addr = (uint32) page_data->main_data;
  spare_addr = (uint32) page_data->spare_data;
  main_len = page_data->main_size_bytes;
  spare_len = page_data->spare_size_bytes;

  page_unit_layout = page_layout->unit_layout;
  buf_ptr = &cmd->data_buff[0];

  /* Map the given main_data spare data buffers according to
     NAND controller codeword and page layout

     This logic does the following things,

     1. It is optimized to read only the requireed amount of data
        from the NAND controller buffer.
     2. If the requested data spanned across multiple codewords
        it maps the buffer according to the page layout
     3. Assumes this function will be called only when
        page_data->main_size_bytes != 0

  */
  page_unit_count = page_layout->page_unit_count;
  page_unit_layout_cnt = page_unit_layout->num_code_word;
  
  while (page_unit_layout_cnt)
  {
    /* Map the main buffer */
    buf_ptr->buf_ptr = (void *)main_addr;

    buf_ptr->cw_offset = page_unit_layout->main_offset;

    main_addr += page_unit_layout->main_len_bytes;

    /* Update the buffer length */
    buf_ptr->buf_len = (main_len < page_unit_layout->main_len_bytes)?
      main_len: page_unit_layout->main_len_bytes;

    buf_ptr->buf_copy_len = page_unit_layout->main_len_bytes;

    buf_ptr++;

    /* Update the Spare buffer */
    buf_ptr->buf_ptr = (void *) spare_addr;

    buf_ptr->cw_offset = page_unit_layout->udata_offset;

    buf_ptr->buf_len = (spare_len < page_unit_layout->udata_len_bytes)?
      spare_len: page_unit_layout->udata_len_bytes;

    buf_ptr->buf_copy_len = page_unit_layout->udata_len_bytes;

    /* If the spare_addr is zero, do not copy spare data */
    if (spare_addr)
    {
      spare_addr += page_unit_layout->udata_len_bytes;
    }

    buf_ptr++;

    /* Calculate the number of bytes to read */
    if ((main_len < page_unit_layout->main_len_bytes) && (0 == spare_len))
    {
      bytes_to_read += main_len;
    }
    else
    {
      bytes_to_read += ptPrivData->cw_size_bytes;
    }

    main_len = (main_len >= page_unit_layout->main_len_bytes)?
      (main_len - page_unit_layout->main_len_bytes) : 0;

    spare_len = (spare_len >= page_unit_layout->udata_len_bytes)?
      (spare_len - page_unit_layout->udata_len_bytes) : 0;

    if (0 == (spare_len|main_len))
    {
      break;
    }

    if(--page_unit_layout_cnt == 0)
	{
	  page_unit_count--;
	  if (page_unit_count)
      {
        page_unit_layout++;
	    page_unit_layout_cnt = page_unit_layout->num_code_word;
      }
	}
  }

  cmd->data_buff_cnt = (page_unit_count + 1) * 2;
  cmd->max_op_data_len = bytes_to_read;
}

/* ====================================================================
 *
 *   F L A S H   H A L   O P E R A T I O N   N A N D   A P I S
 *
 * ==================================================================*/

/*
 * Sets a command to the NAND device
 */
void flash_hal_set_cmd (uint32 dev_id, uint8 cmd)
{
  uint32 seq_index = 0;
  flash_hal_dev_info  *dev_info;
  struct flash_hal_nand_cmd *nand_seq;

  dev_info = &flash_hal_devices[dev_id];
  nand_seq = (struct flash_hal_nand_cmd *) flash_hal_op_seq.cmd_seq;

  /* Create new sequence */
  dev_info->config.cur_seq_cnt++;
  seq_index = dev_info->config.cur_seq_cnt;

  nand_seq[seq_index].op_flag = FLASH_HAL_LAST_CMD;
  nand_seq[seq_index].cmd = cmd;
  nand_seq[seq_index].num_addr_cycles = 0;
  nand_seq[seq_index].data_buff_cnt = 0;
  nand_seq[seq_index].data_dir = FLASH_HAL_DATA_NONE;
  nand_seq[seq_index].wait_mech.wait_type  = FLASH_HAL_WAIT_NONE;
  nand_seq[seq_index].wait_mech.timeout = 0;
}

/*
 * Issues a sub-command to the NAND device.  If a NAND operation consists
 * of multiple commands, then all the commands except the first termed
 * as sub-commands by HAL.
 */
void flash_hal_set_sub_cmd(uint32 dev_id, uint8 cmd)
{
  uint32 seq_index = 0;
  flash_hal_dev_info  *dev_info;
  struct flash_hal_nand_cmd *nand_seq;

  dev_info = &flash_hal_devices[dev_id];
  nand_seq = (struct flash_hal_nand_cmd *) flash_hal_op_seq.cmd_seq;

  dev_info->config.cur_seq_cnt++;
  seq_index = dev_info->config.cur_seq_cnt;

  nand_seq[seq_index-1].op_flag &= ~FLASH_HAL_LAST_CMD;
  nand_seq[seq_index].op_flag = FLASH_HAL_LAST_CMD;
  nand_seq[seq_index].cmd = cmd;
  nand_seq[seq_index].num_addr_cycles = 0;
  nand_seq[seq_index].data_buff_cnt = 0;
  nand_seq[seq_index].data_dir = FLASH_HAL_DATA_NONE;
  nand_seq[seq_index].wait_mech.wait_type  = FLASH_HAL_WAIT_NONE;
  nand_seq[seq_index].wait_mech.timeout = 0;
}

/*
 * Issues address bytes to be used in a NAND operation.
 */
void flash_hal_set_addr(uint32 dev_id, uint32 num_bytes,
  const uint8 *addr_bytes)
{
  uint32 i = 0;
  flash_hal_dev_info  *dev_info;
  struct flash_hal_nand_cmd *nand_seq;

  dev_info = &flash_hal_devices[dev_id];
  nand_seq = (struct flash_hal_nand_cmd *) flash_hal_op_seq.cmd_seq;

  for (i = 0; i < num_bytes; i++)
  {
    nand_seq[dev_info->config.cur_seq_cnt].addr[i] = addr_bytes[i];
  }

  nand_seq[dev_info->config.cur_seq_cnt].num_addr_cycles = num_bytes;
}

/*
 * Issues a wait to the NAND device.
 */
void flash_hal_wait(uint32 dev_id, enum flash_hal_wait_t wait, uint32 timeout)
{
  flash_hal_dev_info  *dev_info;
  struct flash_hal_nand_cmd *nand_seq;

  dev_info = &flash_hal_devices[dev_id];
  nand_seq = (struct flash_hal_nand_cmd *) flash_hal_op_seq.cmd_seq;

  nand_seq[dev_info->config.cur_seq_cnt].wait_mech.wait_type = wait;
  nand_seq[dev_info->config.cur_seq_cnt].wait_mech.timeout = timeout;
}

/*
 * Reads data_len_bytes bytes from device.
 */
void flash_hal_read_data(uint32 dev_id, uint32 data_len_bytes, uint8 *buffer)
{
  uint32 seq_index = 0;
  flash_hal_dev_info  *dev_info;
  struct flash_hal_nand_cmd *nand_seq;

  dev_info = &flash_hal_devices[dev_id];
  nand_seq = (struct flash_hal_nand_cmd *) flash_hal_op_seq.cmd_seq;

  seq_index = dev_info->config.cur_seq_cnt;

  nand_seq[seq_index].data_dir = FLASH_HAL_DATA_OUT_FROM_DEVICE;
  nand_seq[seq_index].data_buff_cnt = 1;
  nand_seq[seq_index].max_op_data_len = data_len_bytes;
  nand_seq[seq_index].data_buff[0].buf_len = data_len_bytes;
  nand_seq[seq_index].data_buff[0].buf_ptr = (void *) buffer;
  nand_seq[seq_index].data_buff[0].cw_offset = 0;
}

/*
 * Writes data_len_bytes bytes to the device.
 */
void flash_hal_write_data(uint32 dev_id, uint32 data_len_bytes,
  const uint8 *buffer)
{
  uint32 seq_index = 0;
  flash_hal_dev_info  *dev_info;
  struct flash_hal_nand_cmd *nand_seq;

  dev_info = &flash_hal_devices[dev_id];
  nand_seq = (struct flash_hal_nand_cmd *) flash_hal_op_seq.cmd_seq;
  seq_index = dev_info->config.cur_seq_cnt;

  nand_seq[seq_index].data_dir = FLASH_HAL_DATA_INTO_DEVICE;

  nand_seq[seq_index].data_buff_cnt = 1;
  nand_seq[seq_index].max_op_data_len = data_len_bytes;
  nand_seq[seq_index].data_buff[0].buf_len = data_len_bytes;
  nand_seq[seq_index].data_buff[0].buf_copy_len = data_len_bytes;
  nand_seq[seq_index].data_buff[0].buf_ptr = (void *) buffer;
  nand_seq[seq_index].data_buff[0].cw_offset = 0;
}

/*
 * Read data bytes from main and/or spare areas of the device.
 */
void flash_hal_read_main_spare(uint32 dev_id,
  struct flash_hal_page_data *page_data)
{
  uint32 seq_index = 0;
  flash_hal_dev_info  *dev_info;
  struct flash_hal_nand_cmd *nand_seq;
  struct flash_hal_page_layout *page_layout;

  dev_info = &flash_hal_devices[dev_id];
  nand_seq = (struct flash_hal_nand_cmd *) flash_hal_op_seq.cmd_seq;
  seq_index = dev_info->config.cur_seq_cnt;

  nand_seq[seq_index].data_dir = FLASH_HAL_DATA_OUT_FROM_DEVICE;
  flash_hal_get_page_layout(dev_id, &page_layout);

  flash_hal_nand_map_buffers(&flash_hal_devices[dev_id].config,
    &nand_seq[seq_index], page_data, page_layout);
}

/*
 * Write data bytes to main and/or spare areas of the device.
 */
void flash_hal_write_main_spare(uint32 dev_id,
  struct flash_hal_page_data *page_data)
{
  uint32 seq_index = 0;
  flash_hal_dev_info  *dev_info;
  struct flash_hal_nand_cmd *nand_seq;
  struct flash_hal_page_layout *page_layout;

  dev_info = &flash_hal_devices[dev_id];
  nand_seq = (struct flash_hal_nand_cmd *) flash_hal_op_seq.cmd_seq;
  seq_index = dev_info->config.cur_seq_cnt;

  nand_seq[seq_index].data_dir = FLASH_HAL_DATA_INTO_DEVICE;

  flash_hal_get_page_layout(dev_id, &page_layout);
  flash_hal_nand_map_buffers(&flash_hal_devices[dev_id].config,
    &nand_seq[seq_index], page_data, page_layout);
}

/*
 * Read data_len_bytes bytes from source buffer at given offset.
 */
void flash_hal_read_data_with_offset(uint32 dev_id, uint32 offset,
  uint32 data_len_bytes, uint8 *buffer)
{
  uint32 seq_index = 0;
  flash_hal_dev_info  *dev_info;
  struct flash_hal_nand_cmd *nand_seq;

  dev_info = &flash_hal_devices[dev_id];
  nand_seq = (struct flash_hal_nand_cmd *) flash_hal_op_seq.cmd_seq;

  seq_index = dev_info->config.cur_seq_cnt;

  nand_seq[seq_index].data_dir = FLASH_HAL_DATA_OUT_FROM_DEVICE;
  nand_seq[seq_index].data_buff_cnt = 1;
  nand_seq[seq_index].max_op_data_len = 528;
  nand_seq[seq_index].data_buff[0].buf_len = data_len_bytes;
  nand_seq[seq_index].data_buff[0].buf_copy_len = data_len_bytes;
  nand_seq[seq_index].data_buff[0].buf_ptr = (void *) buffer;
  nand_seq[seq_index].data_buff[0].cw_offset = offset;
}
