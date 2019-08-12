/**====================================================================
 *
 * FILE:        flash_hal_nandc_predef_bam_exec.c
 *
 * SERVICES:    This prepares a predefined sequence for BAM transfer
 *
 * DESCRIPTION: The perdefined BAM support in flash HAL is to improve
 *              throughput with pre-allocated BAM lists.
 *
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/hal/qpic/flash_hal_nandc_predef_bam_exec.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 11/28/14     sb      Use 0x35 cmd for ONFI parameter page read
 * 08/20/14     sb      Cleanup the BB marker read API
 * 09/03/13     eo      Include target specific HWIO register mapping
 * 10/02/12     sv      Add Transfer Steps config
 * 09/04/12     sv      Initial Revision
 *===================================================================*/

#include "flashi_hal_nandc_predef_bam.h"
#include "flash_bam_wrapper.h"

/* Return the cache maintanance information for predef operations */
void hal_nandc_get_cache_info(struct flash_hal_cache_info *mem_info)
{
  /* Please refer the above memory allocation table for details on
     cache maintanance information.

     The region table is populated in such a way that all the read buffers
     are together and write buffers together. Also the multi related
     buffers grouped together for easy maintanance.
   */ 
    mem_info->read_buffer_count = 1;
    mem_info->read_mem =
      &nandc_predef_bam_data.mem_info.region[NANDC_BAM_REG_READ_BUFF_REGION];
    mem_info->write_buffer_count = 1;
    mem_info->write_mem =
      &nandc_predef_bam_data.mem_info.region[NANDC_BAM_CMDBLOCK_BUFF_REGION];
}


int flash_bam_exec_op(struct flash_bam_xfer_desc_list *tx_cmd_list, 
  struct flash_bam_xfer_desc_list *tx_read_list, 
  struct flash_bam_xfer_desc_list *tx_write_list)
{
  int result;
  struct flash_hal_mem_data *mem_data;
  uint32 temp_count = 0;
  struct flash_hal_cache_info mem_info;
	
  hal_nandc_get_cache_info(&mem_info);
  
  mem_data = &mem_info.read_mem[0];

  /* Do the cache maintanence - cache invalidate - read buffers*/
  while (temp_count++ < mem_info.read_buffer_count)
  {
    flash_hal_cache_inval(mem_data->buff_vaddr, mem_data->buff_size);
    mem_data++;
  }

  /* Do the cache maintanence - cache clean - write buffers*/
  temp_count = 0;
  mem_data = &mem_info.write_mem[0];

  while (temp_count++ < mem_info.write_buffer_count)
  {
    flash_hal_cache_clean(mem_data->buff_vaddr, mem_data->buff_size);
    mem_data++;
  }
  
  result = flash_bam_transfer(tx_cmd_list, tx_read_list, tx_write_list);

  while (temp_count++ < mem_info.read_buffer_count)
  {
    flash_hal_cache_inval(mem_data->buff_vaddr, mem_data->buff_size);
    mem_data++;
  }

  return result;
}

/*
 * Set the address buffers for block operations
 */
static void nandc_set_block_addr( struct flash_hal_ctlr_data *priv_data,
  uint32 *addr0, uint32 *addr1, uint32 start_block, uint32 pages_per_block,
  uint32 num_blocks)
{
    /* We use only ADDR0 register for block addressing */
    *addr0 = start_block;

    /* Clear ADDR1 */
    *addr1 = 0;
}

/*
 * Set the address buffers for page operations
 */
static void nandc_set_addr( struct flash_hal_ctlr_data *priv_data,
  uint32 row_val, uint32 *addr0, uint32 *addr1, uint32 num_pages)
{
  uint32 addr0_val = 0x0;
  uint32 addr1_val = 0x0;
  uint32 row_shift = 0;

  row_shift = sizeof(uint32) - priv_data->col_count;
  
  addr0_val = 0;
  addr0_val |= row_val << (priv_data->col_count * 8);
  addr1_val = row_val >> (row_shift * 8);

  /* ADDR0 = 'col_count' bytes of '0' and remaining bytes with row_val */
  *addr0 = addr0_val;

  /* ADDR1 = remaining bytes of row_val */
  *addr1 = addr1_val;

}

/*
 * Set the config buffers
 */
static void nandc_set_cfg(struct flash_hal_ctlr_data *priv_data)
{
  struct flash_hal_op_cmd *op_cmd  = nandc_predef_bam_data.cmd_list.cfg_list;

  /* Configure CFG0 and CFG1 Registers */
  op_cmd->data = priv_data->cfg0;
  op_cmd++;
  op_cmd->data = priv_data->cfg1;
  op_cmd++;

  /* Configure ECC CFG register */
  op_cmd->data = priv_data->ecc_cfg | FLASH_NAND_DEV0_ECC_CFG_ECC_SW_RESET;
  op_cmd++;

    /* Configure ECC CFG register */
  op_cmd->data = priv_data->ecc_cfg;
  op_cmd++;

  /* Reset the Erased CW detect status */
  op_cmd++;

  /* Configure Erased CW detectcfg register */
  FLASH_ENABLE_ERASED_CW_DETECT(priv_data);
  op_cmd->data = priv_data->erased_cw_det_cfg; 
}


/*
 * Set the command register
 */
static __inline void nandc_set_cmd(uint32 cmd)
{
  struct flash_hal_op_cmd *op_cmd  = nandc_predef_bam_data.cmd_list.cmd_exec_list;
  op_cmd->data = cmd;
}

/*
 * Set the buffer pointers for read
 */
static void nandc_set_read_buffers(
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_page_layout *page_layout,
  struct flash_hal_op_desc *read_desc,
  struct flash_hal_ctlr_data *priv_data,
  uint32 *num_read_desc)
{
  uint32 spare_copy_len = 0;
  uint32 page_unit_count = 0;
  uint32 main_addr = 0, spare_addr = 0;
  uint32 spare_len = 0;
  struct flash_hal_buffer_desc *buff_desc;
  const struct flash_hal_page_unit_layout *page_unit_layout;
  struct flash_hal_op_desc *read_desc_ptr;
  struct flash_hal_op_cmd *op_cmd;
  uint32 page_unit_layout_cnt = 0;
  
  /* NOTE: This code is optimized to read entire
           page data every time. This supports the below
           scenarios,
             - Read all main data bytes
             - Read all main and spare bytes
          No partial reads are supported by this code.
   */

  /* Set main and spare buffer pointers */
  buff_desc = page_data->buff_desc;
  read_desc_ptr = read_desc;

  main_addr = (uint32) buff_desc->buff_paddr;
  spare_addr = (uint32) (buff_desc + 1)->buff_paddr;

  spare_len = page_data->spare_size_bytes;

  page_unit_layout = page_layout->unit_layout;
  page_unit_count = page_layout->page_unit_count;

  /* Config Locate Register for Data Read */
  op_cmd = nandc_predef_bam_data.cmd_list.locate_list;
  page_unit_layout_cnt = page_unit_layout->num_code_word;
  
  /* Config Locate Register for the first code words - This is sufficient for now
   * Need to be revisited if new layouts are introduced */
  op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST | 
    page_unit_layout->main_len_bytes << FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT;
  op_cmd++;
  
  while (page_unit_layout_cnt)
  {
    read_desc_ptr->buf_pa = main_addr;
    read_desc_ptr->buf_size = page_unit_layout->main_len_bytes;
    read_desc_ptr->flags = FLASH_BAM_DESC_FLAG_INT;

    if ((page_unit_count == 1) && (page_unit_layout_cnt == 1))
    { 
      /* Config Locate Register for the last code words - This is sufficient for now
       * Need to be revisited if new layouts are introduced */
      op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST | 
        (page_unit_layout->main_len_bytes << 
        FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT);
    }

    spare_copy_len = (spare_len > page_unit_layout->udata_len_bytes) ?
      page_unit_layout->udata_len_bytes : spare_len;

    if (spare_copy_len)
    {
      read_desc_ptr->flags = 0;
      read_desc_ptr++;
      read_desc_ptr->buf_pa = spare_addr;
      read_desc_ptr->buf_size = spare_copy_len;
      read_desc_ptr->flags = FLASH_BAM_DESC_FLAG_INT;

      // check if we need to read less that 16 user bytes and any issue will be there */
      op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST | 
        ((page_unit_layout->main_len_bytes + spare_copy_len) << 
        FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT);
    }

    spare_len -= spare_copy_len;
    main_addr += page_unit_layout->main_len_bytes;
    spare_addr += spare_copy_len;

    if(--page_unit_layout_cnt == 0)
	{
	  page_unit_count--;
	  if (page_unit_count)
      {
        page_unit_layout++;
	    page_unit_layout_cnt = page_unit_layout->num_code_word;
      }
	}
    read_desc_ptr++;
  }

  *num_read_desc = read_desc_ptr - read_desc;
}

/*
 * Set the buffer pointers for write
 */
static void nandc_set_write_buffers(
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_page_layout *page_layout,
  struct flash_hal_op_desc *write_desc,
  struct flash_hal_ctlr_data *priv_data,
  uint32 *num_write_desc)
{
  int32 spare_len = 0, spare_copy_len = 0;
  uint32 page_unit_count = 0;
  uint32 main_addr = 0;
  uint32 spare_addr = 0;
  struct flash_hal_buffer_desc *buff_desc;
  struct flash_hal_op_desc *write_desc_ptr;
  const struct flash_hal_page_unit_layout *page_unit_layout;
  uint32 page_unit_layout_cnt = 0;
  
  /* NOTE: This code is optimized to write entire
           page data every time. This supports the below
           scenarios,
             - Write all main data bytes
             - Write all main and spare bytes
          No partial main writes are supported by this code.
          Partial spare writes are supported.
   */

  buff_desc = page_data->buff_desc;
  write_desc_ptr = write_desc;

  main_addr = (uint32) buff_desc->buff_paddr;
  spare_addr = (uint32) (buff_desc + 1)->buff_paddr;

  spare_len = page_data->spare_size_bytes;
  page_unit_layout = page_layout->unit_layout;
  page_unit_count = page_layout->page_unit_count;
  page_unit_layout_cnt = page_unit_layout->num_code_word;
  
  /* Set the copy command list with main/spare
     offsets and length from the page layout
     structure */
  while (page_unit_layout_cnt)
  {
    write_desc_ptr->buf_pa = main_addr;
    write_desc_ptr->buf_size = page_unit_layout->main_len_bytes;
    write_desc_ptr->flags = FLASH_BAM_DESC_FLAG_EOT; 
      
    main_addr += page_unit_layout->main_len_bytes;

    /* Copy only the required number of bytes */
    spare_copy_len =
      (spare_len > ((int32) page_unit_layout->udata_len_bytes)) ?
      page_unit_layout->udata_len_bytes : spare_len;

    if (spare_copy_len)
    {
      write_desc_ptr->flags = 0;
      write_desc_ptr++;       
      write_desc_ptr->buf_pa = spare_addr;
      write_desc_ptr->buf_size = spare_copy_len;
      write_desc_ptr->flags = FLASH_BAM_DESC_FLAG_EOT; 
    }
    
    if (spare_copy_len <  page_unit_layout->udata_len_bytes)
    {
      write_desc_ptr->flags = 0;
      write_desc_ptr++;       
      write_desc_ptr->buf_pa = 
        nandc_predef_bam_data.reg_read_buff.spare_fill_buffer;
      write_desc_ptr->buf_size = 
        page_unit_layout->udata_len_bytes - spare_copy_len;
      write_desc_ptr->flags = FLASH_BAM_DESC_FLAG_EOT; 
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

    write_desc_ptr++;
  }

  *num_write_desc = write_desc_ptr - write_desc;
}

/*
 * Prepare the probe command pointer list
 */
void hal_nandc_bam_FF_90_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_buffer_desc *buff_desc,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  struct nandc_predef_cmd_list *cmd_list;
  uint32 *addr0, *addr1;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  int result = 0;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  nandc_set_addr(priv_data, 0, addr0, addr1, 1);

  /*---- Set config registers -----*/

  FLASH_CW_PER_PAGE(priv_data, 0);
  FLASH_UD_SIZE(priv_data, 0);
  FLASH_NUM_ADDR_CYCLES(priv_data, 1);

  nandc_set_cfg(priv_data);

  /*---- Set Command registers -----*/
  nandc_set_cmd(FLASH_NAND_RESET_FLASH_CMD);

  /*---- Set Miscellaneous command lists -----*/
  op_cmd = &(nandc_predef_bam_data.cmd_list.misc_list[NANDC_BAM_MISC0_CMD_LIST]);

  /* 1. Update command VLD register */
  op_cmd->reg_addr = HAL_QPIC_NANDC_FLASH_DEV_CMD_VLD_PADDR;
  op_cmd->data = priv_data->dev_vld;

  /* 2. Change the command register */
  (++op_cmd)->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_CMD_PADDR;
   op_cmd->data = FLASH_NAND_ID_FETCH_CMD;

  /* 3. Exec register */
  (++op_cmd)->reg_addr = HAL_QPIC_NANDC_NANDC_EXEC_CMD_PADDR;
  op_cmd->data = 1;

  /*----- Configure read ID copy -----*/
  cmd_list = &nandc_predef_bam_data.cmd_list;
  op_cmd = &(cmd_list->copyid_list[0]);

  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_READ_ID_PADDR;
  op_cmd->data = (uint32) buff_desc->buff_paddr;

  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.cmd_ptr_list[NANDC_FF_90_PROBE].op_list;
  tx_cmd_list.desc_count = 
    nandc_predef_bam_data.cmd_ptr_list[NANDC_FF_90_PROBE].op_list_count;
 
  result = flash_bam_exec_op(&tx_cmd_list, NULL, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_FF_90_pprocess(priv_data, *op_list, status);
  }
}

/*
 * Prepare the probe command pointer list
 */
void hal_nandc_bam_90_onfi_id_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_onfi_data *probe_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  uint32 addr0_val =0, addr1_val = 0;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  struct nandc_predef_cmd_list *cmd_list;
  uint32 *addr0, *addr1;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  int result = 0;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Set address buffers */
  addr0_val = probe_data->col_offset;
  addr0_val |= probe_data->page << (priv_data->col_count * 8);
  addr1_val = probe_data->page >> ((4 - priv_data->col_count) * 8);

  /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  *addr0 = addr0_val;
  *addr1 = addr1_val;

  /*---- Set config registers -----*/

  FLASH_CW_PER_PAGE(priv_data, 0);
  FLASH_UD_SIZE(priv_data, 0);
  FLASH_NUM_ADDR_CYCLES(priv_data, probe_data->addr_cycles);

  nandc_set_cfg(priv_data);

  /*---- Set Command registers -----*/
  nandc_set_cmd(FLASH_NAND_ID_FETCH_CMD);

  /*----- Configure read ID copy -----*/
  cmd_list = &nandc_predef_bam_data.cmd_list;
  op_cmd = &(cmd_list->copyid_list[0]);

  op_cmd->reg_addr = HAL_QPIC_NANDC_NAND_FLASH_READ_ID_PADDR;
  op_cmd->data = (uint32) probe_data->buff_desc.buff_paddr;

  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.cmd_ptr_list[NANDC_90_ONFI_ID].op_list;
  tx_cmd_list.desc_count =
    nandc_predef_bam_data.cmd_ptr_list[NANDC_90_ONFI_ID].op_list_count;

  result = flash_bam_exec_op(&tx_cmd_list, NULL, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_90_onfi_id_pprocess(priv_data, *op_list, status);
  }
}

/*
 * Prepare the parameter page read command pointer list
 */
void hal_nandc_bam_00_EC_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_onfi_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  uint32 addr0_val =0, addr1_val = 0;
  struct flash_hal_op_cmd *op_cmd;
  struct flash_hal_op_desc *read_desc;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_bam_xfer_desc_list tx_cmd_list, tx_read_list;
  uint32 *addr0, *addr1;
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Set address bytes */
  addr0_val = page_data->col_offset;
  addr0_val |= page_data->page << (priv_data->col_count * 8);
  addr1_val = page_data->page >> ((4 - priv_data->col_count) * 8);

  /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  *addr0 = addr0_val;
  *addr1 = addr1_val;

  /*---- Set config registers -----*/

  /* Set the codeword count */
  FLASH_CW_PER_PAGE(priv_data, 0);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, page_data->addr_cycles);

  nandc_set_cfg(priv_data);

  /* Set command registers */
  nandc_set_cmd(FLASH_NAND_PARAMETER_PAGE_READ_CMD);

  /* Config Locate Register for Data Read */
  op_cmd = nandc_predef_bam_data.cmd_list.locate_list;
  op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST |
    page_data->buff_desc.buff_size << FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT;

  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.cmd_ptr_list[NANDC_00_EC_READ].op_list;
  tx_cmd_list.desc_count =
    nandc_predef_bam_data.cmd_ptr_list[NANDC_00_EC_READ].op_list_count;
  
    /* Data Descriptor list */
  read_desc =  nandc_predef_bam_data.desc.read_copy_desc;

  read_desc->buf_pa = (uint32) page_data->buff_desc.buff_paddr;;
  read_desc->buf_size =  page_data->buff_desc.buff_size;
  read_desc->flags = BAM_IOVEC_FLAG_INT;

  tx_read_list.desc_list_ptr = (bam_iovec_type *)read_desc;
  tx_read_list.desc_count = 1;

  result = flash_bam_exec_op(&tx_cmd_list, &tx_read_list, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_00_EC_pprocess(priv_data, *op_list, status);
  }
}

/*
 * Prepare the Transfer Steps config command pointer list
 */
void hal_nandc_set_xfer_cfg_exec(flash_hal_dev_info *dev_info, 
  struct flash_hal_xfer_cfg *cfg_data, enum flash_hal_status *status)
{
  struct flash_hal_op_cmd *op_cmd;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  int result = 0;

  *status = FLASH_HAL_FAIL;
  (void)dev_info;

  /*---- Set Transfer Steps Register data -----*/
  op_cmd = &(nandc_predef_bam_data.cmd_list.xfer_steps_list[0]);

  if((cfg_data != NULL) && 
    (cfg_data->xfer_reg_count == NAND_MAX_XFER_STEPS_CNT))
  {
    uint32 *xfer_data = NULL;
    uint32 i = 0;
	
	xfer_data = cfg_data->xfer_reg_data;
    for ( i = 0; i < cfg_data->xfer_reg_count; i++)
    {
      op_cmd->data = *xfer_data++;
	  op_cmd++;
    }
  
    tx_cmd_list.desc_list_ptr = (bam_iovec_type *)
      nandc_predef_bam_data.cmd_ptr_list[NANDC_XFER_STEPS_CFG].op_list;
    tx_cmd_list.desc_count = 
      nandc_predef_bam_data.cmd_ptr_list[NANDC_XFER_STEPS_CFG].op_list_count;
 
    result = flash_bam_exec_op(&tx_cmd_list, NULL, NULL);

    if (result == FLASH_BAM_SUCCESS)
    {
      *status = FLASH_HAL_SUCCESS;
    }
  }
}


/*
 * Prepare the read command pointer list
 */
void hal_nandc_bam_00_30_x1_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  uint32 cmd = 0, cw_count = 0;
  struct flash_hal_page_layout *page_layout;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  uint32 *addr0 = NULL, *addr1 = NULL;
  uint32 num_read_desc = 0;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  struct flash_bam_xfer_desc_list tx_read_list;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  if (1 != page_data->total_pages)
  {
    *status = FLASH_HAL_FAIL;
    return;
  }

  cmd_op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_x1_READ];
  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Get page layout */
  dev_info->ctlr_ops.flash_hal_get_page_layout(priv_data, &page_layout);

  /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  nandc_set_addr(priv_data, page_data->start_page, addr0, addr1, 1);

  /*---- Set config registers -----*/

  cw_count = priv_data->cw_cnt - 1;

  /* Set the codeword count */
  FLASH_CW_PER_PAGE(priv_data, cw_count);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, (priv_data->row_count +
    priv_data->col_count));

  nandc_set_cfg(priv_data);

  /* Set command registers */
  cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
    FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_ALL_CMD;

  nandc_set_cmd(cmd);

  /* Configure buffer pointers */
  nandc_set_read_buffers(page_data, page_layout, 
    nandc_predef_bam_data.desc.read_copy_desc, priv_data, &num_read_desc);

  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  tx_cmd_list.desc_count = cmd_op_list->op_list_count;
  
  /* Data Descriptor list */
  tx_read_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.desc.read_copy_desc;
  tx_read_list.desc_count = num_read_desc;

  result = flash_bam_exec_op(&tx_cmd_list, &tx_read_list, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_00_30_x1_pprocess(priv_data, cmd_op_list, status);
  }
}

/*
 * Prepare the write command pointer list
 */
void hal_nandc_bam_80_10_x1_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_page_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  uint32 cmd = 0, cw_count = 0;
  struct flash_hal_page_layout *page_layout;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_op_cmd *op_cmd;
  uint32 *addr0, *addr1;
  uint32 num_write_desc = 0;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  struct flash_bam_xfer_desc_list tx_write_list;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result;

  *status = FLASH_HAL_SUCCESS;
  cmd_op_list =  &nandc_predef_bam_data.cmd_ptr_list[NANDC_80_10_x1_WRITE];

  priv_data = &dev_info->config;

  if (page_data->total_pages != 1)
  {
    *status = FLASH_HAL_FAIL;
    return;
  }

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Get page layout */
  dev_info->ctlr_ops.flash_hal_get_page_layout(priv_data, &page_layout);

   /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  nandc_set_addr(priv_data, page_data->start_page, addr0, addr1, 1);

  /*---- Set config registers -----*/

  cw_count = priv_data->cw_cnt - 1;

  /* Set the codeword count */
  FLASH_CW_PER_PAGE(priv_data, cw_count);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, (priv_data->row_count +
    priv_data->col_count));

  nandc_set_cfg(priv_data);

  /* Set command registers */
  cmd = (FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE == priv_data->ecc_state)?
    FLASH_NAND_PRG_PAGE_CMD:FLASH_NAND_PRG_PAGE_ALL_CMD;

  nandc_set_cmd(cmd);

  nandc_set_write_buffers(page_data, page_layout, 
    nandc_predef_bam_data.desc.write_copy_desc, priv_data, &num_write_desc);

  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  tx_cmd_list.desc_count = cmd_op_list->op_list_count;
  
  /* Data Descriptor list */
  tx_write_list.desc_list_ptr = (bam_iovec_type *)
    nandc_predef_bam_data.desc.write_copy_desc;
  tx_write_list.desc_count = num_write_desc;

  result = flash_bam_exec_op(&tx_cmd_list, NULL, &tx_write_list);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_80_10_x1_pprocess(priv_data, *op_list, status);
  }
}


/*
 * Prepare the erase command pointer list
 */
void hal_nandc_bam_60_D0_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_block_data *block_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  struct flash_hal_ctlr_data *priv_data;
  uint32 *addr0, *addr1;
  struct flash_hal_op_cmd *op_cmd;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;
  cmd_op_list =  &nandc_predef_bam_data.cmd_ptr_list[NANDC_60_D0_ERASE];

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Set address bytes */
  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  /* Set address bytes */
  nandc_set_block_addr(priv_data, addr0, addr1, block_data->start_addr,
    block_data->pages_per_block, block_data->num_blocks);

  /* Set the codeword count and UD size to zero */
  FLASH_UD_SIZE(priv_data, 0);
  FLASH_CW_PER_PAGE(priv_data, 0);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, priv_data->row_count);

  /* Set config */
  nandc_set_cfg(priv_data);

  /* Set erase command */
  nandc_set_cmd(FLASH_NAND_BLOCK_ERASE_CMD);

  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  tx_cmd_list.desc_count = cmd_op_list->op_list_count;

  result = flash_bam_exec_op(&tx_cmd_list, NULL, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_60_D0_pprocess(priv_data, cmd_op_list, status);
  }
}


/*
 * Prepare the read bad block marker command pointer list
 */
void hal_nandc_bam_00_30_bb_marker_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_spare_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status  *status)
{
  uint32 spare_addr = 0;
  uint32 addr0_val =0, addr1_val = 0;
  struct flash_hal_op_cmd *op_cmd;
  struct flash_hal_op_desc *read_desc;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_bam_xfer_desc_list tx_cmd_list;
  struct flash_bam_xfer_desc_list tx_read_list;
  volatile uint32 *addr0, *addr1;
  struct flash_hal_predef_op_list *cmd_op_list;
  int result;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  cmd_op_list = &nandc_predef_bam_data.cmd_ptr_list[NANDC_00_30_BB_READ];
  /* Set main and spare buffer pointers */
  spare_addr = (uint32) page_data->buff_desc.buff_paddr;

  /* Set address buffers */
  addr0_val = page_data->col_offset;
  addr0_val |= page_data->page << (priv_data->col_count * 8);
  addr1_val = page_data->page >> ((4 - priv_data->col_count) * 8);

  op_cmd = nandc_predef_bam_data.cmd_list.addr_list;
  addr0 = &(op_cmd->data);
  op_cmd++;
  addr1 = &(op_cmd->data);

  *addr0 = addr0_val;
  *addr1 = addr1_val;

  /*---- Set config registers -----*/
  FLASH_DISABLE_BCH_ECC(priv_data);

  if(FLASH_HAL_HW_ECC_8BIT == priv_data->ecc_len_bits)
  {
    FLASH_BAD_BLOCK_BYTE_NUM(priv_data, NAND_8BIT_ECC_BAD_BLOCK_BYTE_NUM);
  }	
  else
  {
    FLASH_BAD_BLOCK_BYTE_NUM(priv_data, NAND_BAD_BLOCK_BYTE_NUM);
  }

  FLASH_CW_PER_PAGE(priv_data, 0);
  FLASH_UD_SIZE(priv_data, page_data->spare_size_bytes);
  FLASH_SPARE_SIZE_BYTES(priv_data, 0);
  FLASH_BAD_BLOCK_IN_SPARE_AREA(priv_data);
  FLASH_NUM_ADDR_CYCLES(priv_data, (priv_data->row_count+priv_data->col_count));

  nandc_set_cfg(priv_data);

  /* Set command registers */
  nandc_set_cmd(FLASH_NAND_PAGE_READ_CMD);

  /* Config Locate Register for Data Read */
  op_cmd = nandc_predef_bam_data.cmd_list.locate_list;

  /* Config Locate Register */
  op_cmd->data = FLASH_NAND_READ_LOCATION_n_LAST;
  op_cmd->data |= page_data->spare_size_bytes << FLASH_NAND_READ_LOCATION_n_SIZE_SHIFT;

    /* Data Descriptor list */
  read_desc =  nandc_predef_bam_data.desc.read_copy_desc;

  read_desc->buf_pa = spare_addr;
  read_desc->buf_size =  page_data->spare_size_bytes;
  read_desc->flags =  BAM_IOVEC_FLAG_INT;

  tx_read_list.desc_list_ptr = (bam_iovec_type *)read_desc;
  tx_read_list.desc_count = 1;

  /* Command descriptor list */
  tx_cmd_list.desc_list_ptr = (bam_iovec_type *)cmd_op_list->op_list;
  tx_cmd_list.desc_count = cmd_op_list->op_list_count;

  result = flash_bam_exec_op(&tx_cmd_list, &tx_read_list, NULL);

  if (result != FLASH_BAM_SUCCESS)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    hal_nandc_bam_00_30_bb_marker_pprocess(priv_data, *op_list, status);
  }
}
