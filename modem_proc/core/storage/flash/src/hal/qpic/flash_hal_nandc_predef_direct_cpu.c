/*=====================================================================
 *
 * FILE:        flash_hal_nandc_predef_direct_cpu.c
 *
 * DESCRIPTION: Source predefined operation support with qpic nandc hal
 *              in direct cup mode
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2012,2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===================================================================*/

/*====================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------- 
 * 06/13/14     sb      Update to support 8909
 * 09/05/12     sv      Initial Revision
*===================================================================*/

/*====================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "flash_hal_main.h"
#include "flashi_hal_nand.h"
#include "flashi_hal_common.h"
#include "flashi_hal_nandc.h"

/* Flash page pointer location */
enum flash_hal_page_pointer
{
  FLASH_HAL_PAGE_MAIN_AREA, /* main area in page */
  FLASH_HAL_PAGE_SPARE_AREA  /* spare area in page */
};

static uint32 nandc_status = 0;

/*-------------------------------------------------------------------
 | This file contains the functions for supporting pre-allocated
 | operations specifically for QPIC flash HAL in Direct CPU mode
 |
 *-------------------------------------------------------------------*/

/*
 * Set the address buffers for page operations
 */
static void nandc_set_addr(struct flash_hal_ctlr_data *priv_data,
  uint32 row_val, uint32 col_val)
{
  uint32 addr0_val = 0, addr1_val = 0;

  addr0_val = col_val;
  addr0_val |= row_val << (priv_data->col_count * 8);
  addr1_val = row_val >> ((4 - priv_data->col_count) * 8);

  NANDC_OUT(NAND_ADDR0, addr0_val);
  NANDC_OUT(NAND_ADDR1, addr1_val);
}

/*
 * Wait function for an operation complete.
 */
static void nandc_wait(enum flash_hal_wait_t wait, uint32 *status)
{
  uint32          count  = 0;
  uint32 bail_out_time = FLASH_NANDC_BAILOUT_TIME;

  while (count < bail_out_time)
  {
    /* Read the status register and check for complete */
    NANDC_IN(NAND_FLASH_STATUS, status);

    if ((*status) & FLASH_NAND_STATUS_OP_STATUS_MASK)
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

/*
 * Checks for erased codeword content.
 */
static void hal_nandc_predef_erase_check(
  struct flash_hal_ctlr_data * priv_data,
  const struct flash_hal_page_unit_layout *page_unit_layout,
  enum flash_hal_status *status)
{
  *status = FLASH_HAL_FAIL;

  /* Check erased page/codeword bits erased cw detect status reg. */
  if ((priv_data->erased_cw_det_status & FLASH_NANDC_ERASED_CW_DET_STATUS_MASK) ==
       FLASH_ERASED_CW_DETECT_STATUS)
  {
    *status = FLASH_HAL_ERASED_PAGE;

  }
}

/*
 * Check the read codeword status. This API looks for
 * MPU errors, Erased pages and other errors.
 */
static void hal_nandc_predef_read_status_check(
  struct flash_hal_ctlr_data * priv_data,
  const struct flash_hal_page_unit_layout *page_unit_layout,
  uint32 cw_status, uint32 cw_count,
  enum flash_hal_status *status)
{
  *status = FLASH_HAL_SUCCESS;

  /* Look for any errors in the status register */
  if ((cw_status & FLASH_NAND_STATUS_ANY_ERR_MASK) ||
      ((cw_status & FLASH_NAND_STATUS_CW_CNTR_MASK) >>
      FLASH_NAND_STATUS_CW_CNTR_SHFT) ^ ((uint32)cw_count))
  {
    /* If there is an error, check for MPU */
    if (FLASH_NAND_STATUS_MPU_ERROR_MASK == (cw_status &
      (FLASH_NAND_STATUS_MPU_ERROR_MASK & FLASH_NANDC_STATUS_MASK)))
    {
      *status = FLASH_HAL_FAIL_MPU_ERR;
    }
    else
    {
      /* Look for Erased page, if it is not an erased
         page and it's udata is empty. then flag it
         as a real failure */
      hal_nandc_predef_erase_check(priv_data, page_unit_layout, status);
    }
  }
}

/*
 * Execute a Read ID command
 */
static void nandc_FF_90_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_buffer_desc *buff_desc,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status *status)
{
  uint32 op_status;
  uint32 i = 0;
  struct flash_hal_ctlr_data *priv_data;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  /* Do configure, if anything to configure */
  if (TRUE == priv_data->config_is_needed)
  {
    dev_info->ctlr_ops.flash_hal_configure(priv_data, status);
  }

  /*--------------------------------------------------------------
   |
   |  Reading ID
   |  Config
   |  Issue reset command
   |  Wait for status
   |  Issue read ID command
   |  Status
   |  Read the ID
   |
   *-------------------------------------------------------------*/

  /* Set the codeword count and UD size to zero */
  FLASH_CW_PER_PAGE(priv_data, 0);
  FLASH_UD_SIZE(priv_data, 0);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, 1);

  NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
  NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
  NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);
  nandc_set_addr(priv_data, 0, 0);
  NANDC_OUT(FLASH_DEV_CMD_VLD, priv_data->dev_vld);
  NANDC_OUT(EBI2_ECC_BUF_CFG, priv_data->ecc_buf);

  NANDC_OUT(NAND_FLASH_CMD, FLASH_NAND_RESET_FLASH_CMD);

  i = 2;

  while (i)
  {
    NANDC_OUT(NAND_FLASH_STATUS, FLASH_NAND_STATUS_OP_STATUS_MASK);
    NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);
    nandc_wait(FLASH_HAL_WAIT_ON_DEVICE_READY, &op_status);

    if (op_status & FLASH_NAND_STATUS_ANY_ERR_MASK)
    {
      /* If there is an error, check for MPU */
      if (FLASH_NAND_STATUS_MPU_ERROR_MASK == (op_status &
        (FLASH_NAND_STATUS_MPU_ERROR_MASK & FLASH_NANDC_STATUS_MASK)))
      {
        if (*status != FLASH_HAL_FAIL)
        {
          *status = FLASH_HAL_FAIL_MPU_ERR;
        }

        hal_status |= FLASH_HAL_MPU_ERROR;
      }
      else
      {
        *status = FLASH_HAL_FAIL;
        hal_status |= FLASH_HAL_OP_FAILED;
      }
    }

    NANDC_OUT(NAND_FLASH_CMD, FLASH_NAND_ID_FETCH_CMD);
    i--;
  }

  NANDC_IN(NAND_FLASH_READ_ID, buff_desc->buff_vaddr);
}

/*
 * Execute a Read ONFI ID command
 */
static void nandc_90_onfi_id_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_onfi_data *probe_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status *status)
{
  uint32 op_status;
  struct flash_hal_ctlr_data *priv_data;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  /* Do configure, if anything to configure */
  if (TRUE == priv_data->config_is_needed)
  {
    dev_info->ctlr_ops.flash_hal_configure(priv_data, status);
  }

  /*--------------------------------------------------------------
   |
   |  Reading ID
   |  Config
   |  Issue reset command
   |  Wait for status
   |  Issue read ID command
   |  Status
   |  Read the ID
   |
   *-------------------------------------------------------------*/

  /* Set the codeword count and UD size to zero */
  FLASH_CW_PER_PAGE(priv_data, 0);
  FLASH_UD_SIZE(priv_data, 0);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, probe_data->addr_cycles);

  NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
  NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
  NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);

  nandc_set_addr(priv_data, probe_data->page, probe_data->col_offset);

  NANDC_OUT(NAND_FLASH_CMD, FLASH_NAND_ID_FETCH_CMD);

  NANDC_OUT(NAND_FLASH_STATUS, FLASH_NAND_STATUS_OP_STATUS_MASK);
  NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);
  nandc_wait(FLASH_HAL_WAIT_ON_DEVICE_READY, &op_status);

  if (op_status & FLASH_NAND_STATUS_ANY_ERR_MASK)
  {
    /* If there is an error, check for MPU */
    if (FLASH_NAND_STATUS_MPU_ERROR_MASK == (op_status &
      (FLASH_NAND_STATUS_MPU_ERROR_MASK & FLASH_NANDC_STATUS_MASK)))
    {
      if (*status != FLASH_HAL_FAIL)
      {
        *status = FLASH_HAL_FAIL_MPU_ERR;
      }

      hal_status |= FLASH_HAL_MPU_ERROR;
    }
    else
    {
      *status = FLASH_HAL_FAIL;
      hal_status |= FLASH_HAL_OP_FAILED;
    }
  }

  NANDC_IN(NAND_FLASH_READ_ID, probe_data->buff_desc.buff_vaddr);
}

/*
 * Execute a parameter page read
 */
static void nandc_00_EC_exec(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_onfi_data *page_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status *status)
{
  uint32 op_status = 0;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_buffer_desc *buff_desc;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  /* Do configure, if anything to configure */
  if (TRUE == priv_data->config_is_needed)
  {
    dev_info->ctlr_ops.flash_hal_configure(priv_data, status);
  }

  /*--------------------------------------------------------------
   |
   |  Reading Parameter Page
   |    - Config
   |    - Address
   |    - Change read command
   |    - Exec
   |    - Status
   |    - Copy the codeword
   |    - Restore original command
   |
   *-------------------------------------------------------------*/

  /* Set the codeword count */
  FLASH_CW_PER_PAGE(priv_data, 0);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, page_data->addr_cycles);

  NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
  NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
  NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);
  
  /* It appears if write of ECC buf cfg is done before the above
     CFG writes, then reads could fail! */
  NANDC_OUT(EBI2_ECC_BUF_CFG, priv_data->ecc_buf);

  /* Update the command to use 0x00-0xEC sequence */
  NANDC_OUT(FLASH_DEV_CMD1, 0xF00F00EC);
  NANDC_OUT(FLASH_DEV_CMD_VLD, 0x1C);

  /* Set main and spare buffer pointers */
  buff_desc = &page_data->buff_desc;

  nandc_set_addr(priv_data, page_data->page, page_data->col_offset);

  /* Issue the read command */
  NANDC_OUT(NAND_FLASH_CMD, FLASH_NAND_PAGE_READ_ALL_CMD);
  NANDC_OUT(NAND_FLASH_STATUS, FLASH_NAND_STATUS_OP_STATUS_MASK);
  NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);
  nandc_wait(FLASH_HAL_WAIT_ON_DEVICE_READY, &op_status);

  if (op_status & FLASH_NAND_STATUS_ANY_ERR_MASK)
  {
    /* If there is an error, check for MPU */
    if (FLASH_NAND_STATUS_MPU_ERROR_MASK == (op_status &
      (FLASH_NAND_STATUS_MPU_ERROR_MASK & FLASH_NANDC_STATUS_MASK)))
    {
      if (*status != FLASH_HAL_FAIL)
      {
        *status = FLASH_HAL_FAIL_MPU_ERR;
      }

      hal_status |= FLASH_HAL_MPU_ERROR;
    }
    else
    {
      *status = FLASH_HAL_FAIL;
      hal_status |= FLASH_HAL_OP_FAILED;
    }
  }
  else
  {
    NANDC_COPY_FROM_DEVICE(buff_desc->buff_vaddr,
      HAL_QPIC_NANDC_NANDC_BUFFER_VADDR, buff_desc->buff_size);
  }

  /* Restore the CMD1 register */
  NANDC_OUT(FLASH_DEV_CMD1, FLASH_DEV_CMD1_DEFAULT_CMD);
  NANDC_OUT(FLASH_DEV_CMD_VLD, priv_data->dev_vld);

  /* Check if timeout occurred */
  if (FLASH_HAL_OP_TIMEOUT == (FLASH_HAL_OP_TIMEOUT & hal_status))
  {
    *status = FLASH_HAL_OP_TIMEOUT_ERR;
  }
}

/*
 * Execute a 0x00-0x30 read page command
 */
static void nandc_00_30_exec(flash_hal_dev_info *dev_info, struct
  flash_hal_predef_page_data *page_data, struct flash_hal_predef_op_list
  **op_list, enum flash_hal_status *status)
{
  enum flash_hal_status tmp_status;
  int32 cw_count = 0, erase_cw_count = 0;
  uint32 cw_status;
  uint32 cmd = 0, page_unit_count = 0;
  uint32 main_addr = 0, spare_addr = 0;
  uint32 spare_len = 0, spare_copy_len = 0;
  uint32 offset = 0, row_val = 0;
  uint32 page_unit_cw_count = 0;
  uint32 nandc_buffer = HAL_QPIC_NANDC_NANDC_BUFFER_VADDR;
  const struct flash_hal_page_unit_layout *page_unit_layout;
  struct flash_hal_page_layout *page_layout;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_buffer_desc *buff_desc;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  /* Do configure, if anything to configure */
  if (TRUE == priv_data->config_is_needed)
  {
    dev_info->ctlr_ops.flash_hal_configure(priv_data, status);
  }

  /* Get page layout */
  dev_info->ctlr_ops.flash_hal_get_page_layout(priv_data, &page_layout);

  cw_count = (priv_data->page_size_bytes / FLASH_NAND_UD_PER_CW) -1;

  /* Set the codeword count */
  FLASH_CW_PER_PAGE(priv_data, cw_count);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, (priv_data->row_count+priv_data->col_count));

  /*--------------------------------------------------------------
   |
   |  - ECC buffer config
   |  - Config
   |
   |  for each page
   |    - Address
   |    - Save Command Register
   |
   |    for each codeword,
   |      - Command Register
   |      - Exec
   |      - Status
   |      - Copy Main Bytes
   |      - Copy Spare Bytes
   |      - Copy ECC Bytes
   |
   |    - Restore Command Register
   |
   *-------------------------------------------------------------*/

  NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
  NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
  NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);

  /* It appears if write of ECC buf cfg is done before the above
     CFG writes, then reads could fail! */
  NANDC_OUT(EBI2_ECC_BUF_CFG, priv_data->ecc_buf);

  cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
    FLASH_NAND_PAGE_READ_ECC_CMD:FLASH_NAND_PAGE_READ_ALL_CMD;

  /* Set main and spare buffer pointers */
  spare_len = page_data->spare_size_bytes;
  buff_desc = page_data->buff_desc;
  row_val = page_data->start_page;

  /* Set the main and spare addresses */
  main_addr = (uint32) buff_desc->buff_vaddr;
  spare_addr = (uint32) (buff_desc + 1)->buff_vaddr;

  nandc_set_addr(priv_data, row_val, 0);

  page_unit_layout = page_layout->unit_layout;
  page_unit_cw_count = page_unit_layout->num_code_word;
  page_unit_count = 0;
  cw_count = (priv_data->page_size_bytes / FLASH_NAND_UD_PER_CW) -1;
  erase_cw_count = 0;

  NANDC_OUT(NAND_ERASED_CW_DETECT_CFG, priv_data->erased_cw_det_cfg);
  FLASH_ENABLE_ERASED_CW_DETECT(priv_data);
  NANDC_OUT(NAND_ERASED_CW_DETECT_CFG, priv_data->erased_cw_det_cfg);
  do
  {
    /* Issue the read command */
    NANDC_OUT(NAND_FLASH_CMD, cmd);
    /* Mask operation status bits only */
    NANDC_IN(NAND_FLASH_STATUS, &nandc_status);
    nandc_status = nandc_status | FLASH_NAND_STATUS_OP_STATUS_MASK;
    NANDC_OUT(NAND_FLASH_STATUS, nandc_status);

    NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);
    nandc_wait(FLASH_HAL_WAIT_ON_DEVICE_READY, &cw_status);

    /* Mask operation status bits only */
    NANDC_IN(NAND_ERASED_CW_DETECT_STATUS, &priv_data->erased_cw_det_status);

    /* Look for any errors */
    hal_nandc_predef_read_status_check(priv_data,
      page_unit_layout, cw_status, (uint32)cw_count, &tmp_status);

    if (FLASH_HAL_ERASED_PAGE == tmp_status)
    {
      erase_cw_count++;
    }
    else if (FLASH_HAL_SUCCESS != tmp_status)
    {
      *status = tmp_status;
    }

    /* Copy data to callers buffer */

    offset = page_unit_layout->main_offset;

    NANDC_COPY_FROM_DEVICE(main_addr, nandc_buffer + offset,
      page_unit_layout->main_len_bytes);

    offset = page_unit_layout->udata_offset;

    if (spare_len)
    {
      spare_copy_len = (spare_len > page_unit_layout->udata_len_bytes) ?
        page_unit_layout->udata_len_bytes:spare_len;

      NANDC_COPY_FROM_DEVICE(spare_addr, nandc_buffer + offset,
        spare_copy_len);

      spare_len -= spare_copy_len;
    }

    main_addr += page_unit_layout->main_len_bytes;
    spare_addr += spare_copy_len;

    page_unit_cw_count--;
	if(page_unit_cw_count == 0)
    {
	  page_unit_layout++;
      page_unit_cw_count = page_unit_layout->num_code_word;
    }
    page_unit_count++;

  } while (--cw_count >= 0);

  /* If all the codewords are erased, then return erased page failure.
     We are breaking at this point as there is no point in continuing */
  if (erase_cw_count == priv_data->cw_cnt)
  {
    *status = FLASH_HAL_ERASED_PAGE;

    /* Set the op erased page bit in hal status */
    hal_status |= FLASH_HAL_ERASED_PAGE_VALID;
  }

  /* Check if timeout occurred */
  if (FLASH_HAL_OP_TIMEOUT == (FLASH_HAL_OP_TIMEOUT & hal_status))
  {
    *status = FLASH_HAL_OP_TIMEOUT_ERR;
  }
}

/*
 * Execute a 0x80-0x10 write page or 0x80-0x15..0x80-0x10
 * cache write page command
 */
static void nandc_80_1x_exec(flash_hal_dev_info *dev_info, struct
  flash_hal_predef_page_data *page_data, struct flash_hal_predef_op_list
  **op_list, enum flash_hal_status *status)
{
  int32 cw_count = 0;
  uint32 cmd = 0, page_unit_count = 0;
  uint32 main_addr = 0, spare_addr = 0;
  uint32 spare_len = 0;
  uint32 offset = 0, row_val = 0;
  uint32 nandc_buffer = HAL_QPIC_NANDC_NANDC_BUFFER_VADDR;
  uint32 cw_status, spare_tmp_addr;
  uint32 page_unit_cw_count = 0;
  uint8 spare_tmp_buff[(FLASH_HAL_SPARE_UDATA_LEN_PER_CW*(NAND_CW_PER_PAGE_4096+1))];
  const struct flash_hal_page_unit_layout *page_unit_layout;
  struct flash_hal_page_layout *page_layout;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_buffer_desc *buff_desc;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  /* Do configure, if anything to configure */
  if (TRUE == priv_data->config_is_needed)
  {
    dev_info->ctlr_ops.flash_hal_configure(priv_data, status);
  }

  /* Get page layout */
  dev_info->ctlr_ops.flash_hal_get_page_layout(priv_data, &page_layout);

  cw_count = (priv_data->page_size_bytes / FLASH_NAND_UD_PER_CW) -1;

  /* Set the codeword count */
  FLASH_CW_PER_PAGE(priv_data, cw_count);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, (priv_data->row_count+priv_data->col_count));

  /*--------------------------------------------------------------
   |
   |  ECC buffer config
   |  Config
   |
   |  If using cached write replace DEV_CMD0 with 0x1580D060
   |  for each page
   |    - Address
   |    - Save Command Register
   |
   |    If last page and using cached write restore DEV_CMD0 to 0x1080D060
   |    for each codeword,
   |      - Copy Main Bytes
   |      - Copy Spare Bytes
   |      - Command Register
   |      - Exec
   |      - Status
   |
   |    - Restore Command Register
   |
   *-------------------------------------------------------------*/

  NANDC_OUT(EBI2_ECC_BUF_CFG, priv_data->ecc_buf);

  NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
  NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
  NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);

  /* Set command registers */
  cmd = (priv_data->ecc_state == FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE)?
    FLASH_NAND_PRG_PAGE_CMD:FLASH_NAND_PRG_PAGE_ALL_CMD;

  /* Set main and spare buffer pointers */
  buff_desc = page_data->buff_desc;
  row_val = page_data->start_page;

  nandc_set_addr(priv_data, row_val, 0);

  main_addr = (uint32) buff_desc->buff_vaddr;
  spare_addr = (uint32) (buff_desc + 1)->buff_vaddr;

  spare_len = page_data->spare_size_bytes;
  page_unit_layout = page_layout->unit_layout;
  page_unit_count = 0;
  page_unit_cw_count = page_unit_layout->num_code_word;
  cw_count = (priv_data->page_size_bytes / FLASH_NAND_UD_PER_CW) -1;

  spare_tmp_addr = (uint32) spare_tmp_buff;

  flash_hal_memset((void *)spare_tmp_addr, 0xFF, 
    (FLASH_HAL_SPARE_UDATA_LEN_PER_CW*(NAND_CW_PER_PAGE_4096+1)));

  flash_hal_memcpy((void *) spare_tmp_addr, (void *) spare_addr, spare_len);

  do
  {
    offset = page_unit_layout->main_offset;

    NANDC_COPY_TO_DEVICE(nandc_buffer + offset, main_addr,
      page_unit_layout->main_len_bytes);

    offset = page_unit_layout->udata_offset;

    if (page_unit_layout->udata_len_bytes)
    {
      /* Copy spare user data to callers buffer */
      NANDC_COPY_TO_DEVICE(nandc_buffer + page_unit_layout->main_len_bytes,
       spare_tmp_addr, page_unit_layout->udata_len_bytes);

      spare_tmp_addr += page_unit_layout->udata_len_bytes;
    }

    NANDC_OUT(NAND_FLASH_CMD, cmd);
    /* Mask operation status bits only */
    NANDC_IN(NAND_FLASH_STATUS, &nandc_status);
    nandc_status = nandc_status | FLASH_NAND_STATUS_OP_STATUS_MASK;
    NANDC_OUT(NAND_FLASH_STATUS, nandc_status);

    NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);
    nandc_wait(FLASH_HAL_WAIT_ON_DEVICE_READY, &cw_status);

    if ((cw_status & FLASH_NAND_STATUS_ANY_ERR_MASK) ||
      ((cw_status & FLASH_NAND_STATUS_CW_CNTR_MASK) >>
      FLASH_NAND_STATUS_CW_CNTR_SHFT) ^ ((uint32)cw_count))
    {
      *status = FLASH_HAL_FAIL;
      hal_status |= FLASH_HAL_WRITE_OP_ERROR;
    }

    main_addr += page_unit_layout->main_len_bytes;
    spare_addr += page_unit_layout->udata_len_bytes;

    page_unit_cw_count--;
	if(page_unit_cw_count == 0)
    {
	  page_unit_layout++;
      page_unit_cw_count = page_unit_layout->num_code_word;
    }
    page_unit_count++;

  } while (--cw_count >= 0);

  /* Check if timeout occurred */
  if (FLASH_HAL_OP_TIMEOUT == (FLASH_HAL_OP_TIMEOUT & hal_status))
  {
    *status = FLASH_HAL_OP_TIMEOUT_ERR;
  }

  /* Clear the Status Registers */
  NANDC_OUT(NAND_FLASH_READ_STATUS, FLASH_NAND_READ_STATUS_CLEAR);
  NANDC_OUT(NAND_FLASH_STATUS, FLASH_NAND_STATUS_OP_DONE);
}

/*
 * Execute a 0x60-0xD0 erase block command
 */
static void nandc_60_D0_exec(flash_hal_dev_info *dev_info, struct
  flash_hal_predef_block_data *block_data, struct flash_hal_predef_op_list
  **op_list, enum flash_hal_status *status)
{
  uint32 op_status, row_val = 0;
  uint32 addr0_val =0, addr1_val = 0, row_shift = 4;
  struct flash_hal_ctlr_data *priv_data;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  /* Do configure, if anything to configure */
  if (TRUE == priv_data->config_is_needed)
  {
    dev_info->ctlr_ops.flash_hal_configure(priv_data, status);
  }

  /*--------------------------------------------------------------
   |
   |  Erasing a block involves following steps
   |
   |  Config
   |
   |  for each block
   |    - Address
   |    - Command Register
   |    - Exec
   |    - Status
   |
   *-------------------------------------------------------------*/

  /* Set the codeword count and UD size to zero */
  FLASH_CW_PER_PAGE(priv_data, 0);
  FLASH_UD_SIZE(priv_data, 0);

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, priv_data->row_count);

  NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
  NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
  NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);

  row_val = block_data->start_addr;

  addr0_val = row_val;
  addr1_val = row_val >> (row_shift * 8);

  NANDC_OUT(NAND_ADDR0, addr0_val);
  NANDC_OUT(NAND_ADDR1, addr1_val);

  NANDC_OUT(NAND_FLASH_CMD, FLASH_NAND_BLOCK_ERASE_CMD);
  /* Mask operation status bits only */
  NANDC_IN(NAND_FLASH_STATUS, &nandc_status);
  nandc_status = nandc_status | FLASH_NAND_STATUS_OP_STATUS_MASK;
  NANDC_OUT(NAND_FLASH_STATUS, nandc_status);

  NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);
  nandc_wait(FLASH_HAL_WAIT_ON_DEVICE_READY, &op_status);

  if (op_status & FLASH_NAND_STATUS_ANY_ERR_MASK)
  {
    *status = FLASH_HAL_FAIL;
    hal_status |= FLASH_HAL_ERASE_OP_ERROR;
  }

  /* Check if timeout occurred */
  if (FLASH_HAL_OP_TIMEOUT == (FLASH_HAL_OP_TIMEOUT & hal_status))
  {
    *status = FLASH_HAL_OP_TIMEOUT_ERR;
  }

  /* Clear the Status Registers */
  NANDC_OUT(NAND_FLASH_READ_STATUS, FLASH_NAND_READ_STATUS_CLEAR);
  NANDC_OUT(NAND_FLASH_STATUS, FLASH_NAND_STATUS_OP_DONE);
}


/*
 * Execute a 0x00-0x30 to read only bad block marker bytes
 */
static void nandc_00_30_bb_marker_exec(flash_hal_dev_info *dev_info, struct
  flash_hal_predef_spare_data *page_data, struct flash_hal_predef_op_list
  **op_list, enum flash_hal_status *status)
{
  uint32 cw_status;
  uint32 spare_addr = 0, spare_len = 0;
  struct flash_hal_ctlr_data *priv_data;
  struct flash_hal_page_layout *page_layout;

  *status = FLASH_HAL_SUCCESS;
  priv_data = &dev_info->config;

  dev_info->ctlr_ops.flash_hal_configure(priv_data, status);

  /* Get page layout */
  dev_info->ctlr_ops.flash_hal_get_page_layout(priv_data, &page_layout);

  /*--------------------------------------------------------------
   |
   |  - ECC buffer config
   |  - Config
   |  - Address
   |  - Command Register
   |  - Exec
   |  - Status
   |  - Copy Spare Bytes
   |
   *-------------------------------------------------------------*/

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

  /* Set the number of address cycles */
  FLASH_NUM_ADDR_CYCLES(priv_data, (priv_data->row_count+priv_data->col_count));

  NANDC_OUT(NAND_DEV0_CFG0, priv_data->cfg0);
  NANDC_OUTI(NAND_DEVn_CFG1, priv_data->chip_sel, priv_data->cfg1);
  NANDC_OUT(NAND_DEV0_ECC_CFG, priv_data->ecc_cfg);
  NANDC_OUT(NAND_ERASED_CW_DETECT_CFG, priv_data->erased_cw_det_cfg);

  /* Set main and spare buffer pointers */
  spare_addr = (uint32) page_data->buff_desc.buff_vaddr;
  spare_len = page_data->spare_size_bytes;

  nandc_set_addr(priv_data, page_data->page, page_data->col_offset);

  NANDC_OUT(NAND_FLASH_CMD, FLASH_NAND_PAGE_READ_CMD);
  /* Mask operation status bits only */
  NANDC_IN(NAND_FLASH_STATUS, &nandc_status);
  nandc_status = nandc_status | FLASH_NAND_STATUS_OP_STATUS_MASK;
  NANDC_OUT(NAND_FLASH_STATUS, nandc_status);

  NANDC_OUT(NANDC_EXEC_CMD, FLASH_NANDC_EXEC_CMD);
  nandc_wait(FLASH_HAL_WAIT_ON_DEVICE_READY, &cw_status);

  if (cw_status & FLASH_NAND_STATUS_ANY_ERR_MASK)
  {
    *status = FLASH_HAL_FAIL;
  }
  else
  {
    if (spare_len)
    {
      NANDC_COPY_FROM_DEVICE(spare_addr, HAL_QPIC_NANDC_NANDC_BUFFER_VADDR,
        spare_len);
    }
  }
}



/*
 * Close pre-defined sequence
 */
void hal_nandc_predef_deinit(flash_hal_dev_info *dev_info)
{
}

/**------------------------- PUBLIC FUNCTIONS ---------------------------**/

/*
 * Init pre-defined sequence
 */
void hal_nandc_predef_init(flash_hal_dev_info *dev_info,
  struct flash_hal_predef_init_data *init_data, uint32 *used_buff_size,
  enum flash_hal_status *status)
{
  *status = FLASH_HAL_SUCCESS;
}

/*
 * Init pre-defined sequence function pointers
 */
void hal_nandc_fptrs_init_default(struct flash_hal_ctlr_ops  *ctlr_ops)
{
  ctlr_ops->flash_hal_predef_init = hal_nandc_predef_init;
  ctlr_ops->flash_hal_predef_deinit = hal_nandc_predef_deinit;

  ctlr_ops->flash_hal_read_id = nandc_FF_90_exec;
  ctlr_ops->flash_hal_read_onfi_id = nandc_90_onfi_id_exec;
  ctlr_ops->flash_hal_read_param_page = nandc_00_EC_exec;

  ctlr_ops->flash_hal_read_page = nandc_00_30_exec;
  ctlr_ops->flash_hal_read_bb_marker = nandc_00_30_bb_marker_exec;

  ctlr_ops->flash_hal_write_page = nandc_80_1x_exec;
  ctlr_ops->flash_hal_erase_block = nandc_60_D0_exec;

  ctlr_ops->flash_hal_transfer = hal_nandc_transfer;
  ctlr_ops->flash_hal_begin_op = flash_hal_nand_begin_op;
  ctlr_ops->flash_hal_end_op = flash_hal_nand_end_op;

  ctlr_ops->flash_hal_post_process_op_list = NULL;

}

/*
 * Init pre-defined sequence function pointers - Read Only
 */
void hal_nandc_fptrs_init_reduced(struct flash_hal_ctlr_ops  *ctlr_ops)
{
  ctlr_ops->flash_hal_predef_init = hal_nandc_predef_init;
  ctlr_ops->flash_hal_predef_deinit = hal_nandc_predef_deinit;

  ctlr_ops->flash_hal_read_id = nandc_FF_90_exec;
  ctlr_ops->flash_hal_read_onfi_id = nandc_90_onfi_id_exec;
  ctlr_ops->flash_hal_read_param_page = nandc_00_EC_exec;

  ctlr_ops->flash_hal_read_page = nandc_00_30_exec;
  ctlr_ops->flash_hal_read_bb_marker = nandc_00_30_bb_marker_exec;

  ctlr_ops->flash_hal_write_page = NULL;

  ctlr_ops->flash_hal_erase_block = NULL;

  ctlr_ops->flash_hal_transfer = NULL;
  ctlr_ops->flash_hal_begin_op = NULL;
  ctlr_ops->flash_hal_end_op = NULL;

  ctlr_ops->flash_hal_post_process_op_list = NULL;
}

/* predefined command pts init function for default(full) mode */
enum flash_hal_status hal_nandc_predef_cmd_ptrs_init_default(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *buff_data, uint32 *used_buff_size)
{
  return FLASH_HAL_SUCCESS;
}

/* predefined command pts init function for reduced mode */
enum flash_hal_status hal_nandc_predef_cmd_ptrs_init_reduced(
  struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *buff_data, uint32 *used_buff_size)
{
  return FLASH_HAL_SUCCESS;
}

int hal_nandc_iface_init(void)
{
  return FLASH_HAL_SUCCESS;
}

void hal_nandc_iface_deinit(void)
{
}
