/*=============================================================================
 *
 * FILE:      flash_nor_i2c_core.c
 *
 * DESCRIPTION: This file initializes the Flash NOR I2C core APIs
 *
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010,2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_nor_i2c_core.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 03/19/12     sv      Adapt to SPI NOR power management changes
 * 03/19/12     eo      Support for erase blocks by size
 * 02/28/12     sv      Fix Klocwork error
 * 10/11/10     bn      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nor_i2c.h"

/*------------------------------------------------------------------------------
  Variables local to this file.
 ------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Variables global in this driver.
 ------------------------------------------------------------------------------*/
 flash_vtbl nor_i2c_base_vtbl;

/*------------------------------------------------------------------------------
  Extern variable(s) and function(s).
 ------------------------------------------------------------------------------*/

/* NOR entry init data */
extern struct nor_entry_data nor_entry_init_data;

/*------------------------------------------------------------------------------
  Following functions are local to this driver.
 ------------------------------------------------------------------------------*/

/*
 * Writes pages in Flash I2C EEPROM
 */
static int nor_i2c_write_pages(flash_client_ctxt *client_ctxt,
  enum page_write_opcode write_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec)
{
  int result = FLASH_DEVICE_DONE;
  int32  buf_desc_count, total_bytes_to_send, start_addr;
  uint32 num_shifts = client_ctxt->client_data.page_size_shift; 
  struct nor_dev_data *dev_data;
  DALSysMemDescBuf *buf_desc;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;
  struct nor_i2c_xfer_buff_info i2c_tx_buff_info;

  dev_data = &dev_ctxt->dev_info.u.nor;

  /* Get starting buffer descriptor from buffer desc list */
  buf_desc = DALFW_MemDescBufPtr(buffer_desc, 0);
  buf_desc_count = (int32)buffer_desc->dwNumDescBufs;
  if (buf_desc == NULL)
  {
    result = FLASH_DEVICE_FAIL;
  }

  total_bytes_to_send = (int32)(page_data_iovec->total_page_count * 
    dev_data->page_size_bytes);

  start_addr = page_data_iovec->start_page << num_shifts;

  while ((total_bytes_to_send > 0) && (result == FLASH_DEVICE_DONE))
  {
    i2c_tx_buff_info.data = (uint8 *)buf_desc->VirtualAddr;
    i2c_tx_buff_info.length = buf_desc->size;
    i2c_tx_buff_info.start_addr = start_addr;

    result = nor_i2c_write_op(&i2c_tx_buff_info);
    start_addr += buf_desc->size;
    total_bytes_to_send -= buf_desc->size;

    /* Get next buffer descriptor */
    if (buf_desc_count)
    {
      buf_desc++;
      buf_desc_count--;
    } 
  }
  
  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(3,(" Flash_i2c: Failed nor_i2c_write_pages!\n"));
  }

  return result;
}

/*
 * Reads pages in Flash I2C EEPROM
 */
static int nor_i2c_read_pages(flash_client_ctxt *client_ctxt,
  enum page_read_opcode read_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec)
{
  int result = FLASH_DEVICE_DONE;
  int32  buf_desc_count, total_bytes_to_receive, start_addr;  
  uint32 num_shifts = client_ctxt->client_data.page_size_shift; 
  DALSysMemDescBuf *buf_desc;
  struct nor_i2c_xfer_buff_info i2c_rx_buff_info;

  /* Get starting buffer descriptor from buffer desc list */
  buf_desc = DALFW_MemDescBufPtr(buffer_desc, 0);
  buf_desc_count = (int32)buffer_desc->dwNumDescBufs;
  if (buf_desc == NULL)
  {
    result = FLASH_DEVICE_FAIL;
  }
  
  total_bytes_to_receive = (int32)(page_data_iovec->total_page_count * 
    page_data_iovec->main_size_bytes);

  start_addr = page_data_iovec->start_page << num_shifts;

  while ((total_bytes_to_receive > 0) && (result == FLASH_DEVICE_DONE))
  {
    i2c_rx_buff_info.data = (uint8 *)buf_desc->VirtualAddr;
    i2c_rx_buff_info.length = buf_desc->size;
    i2c_rx_buff_info.start_addr = start_addr;

    result = nor_i2c_read_op(&i2c_rx_buff_info);

    start_addr += buf_desc->size;
    total_bytes_to_receive -= buf_desc->size;

    /* Get next buffer descriptor */
    if (buf_desc_count)
    {
      buf_desc++;
      buf_desc_count--;
    } 
  }

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(3,(" Flash_i2c: Failed nor_i2c_read_pages!\n"));
  }

  return result;
}

/*
 * Get the I2C EEPROM device information
 */
static int nor_i2c_get_info (flash_client_ctxt *client_ctxt,
  enum flash_info_type info_type, void *get_data)
{
  int result = FLASH_DEVICE_DONE;

  if (client_ctxt->refs)
  {
    if (FLASH_DEVICE_INFO == info_type)
    {
      struct flash_info *info = (struct flash_info *)get_data;

      /* Copy the device name */
      flash_strcpy((uint8 *)info->device_name,
        (uint8 *)client_ctxt->flash_dev_ctxt->dev_info.common.device_name,32);

      /* Get Device type and width  */
      info->type = client_ctxt->flash_dev_ctxt->dev_info.common.type;
      info->tech = client_ctxt->flash_dev_ctxt->dev_info.common.tech;
      info->width = client_ctxt->flash_dev_ctxt->dev_info.common.width;
      info->block_count = 
        client_ctxt->flash_dev_ctxt->dev_info.u.nor.block_count;
      info->write_style =
        client_ctxt->flash_dev_ctxt->dev_info.common.write_style;

      /* Get Device manufacturer/make ID, device ID, version  */
      info->maker_id = client_ctxt->flash_dev_ctxt->dev_info.u.nor.dev_ids[0];
      info->device_id = client_ctxt->flash_dev_ctxt->dev_info.u.nor.dev_ids[1];
      info->version_id =
        client_ctxt->flash_dev_ctxt->dev_info.u.nor.dev_ids[2];

      /* Get Partition specific information  */
      info->partition.block_count = client_ctxt->client_data.block_count;
      info->partition.pages_per_block =
        client_ctxt->client_data.pages_per_block;
      info->partition.page_size_bytes =
        client_ctxt->client_data.page_size_bytes;
      info->partition.total_page_size_bytes =
        client_ctxt->client_data.total_page_size_bytes;
      info->partition.physical_start_block =
        client_ctxt->client_data.partition_block_start;
    }
    else if(FLASH_NOR_ERASE_REGION_INFO == info_type)
    {
      struct flash_nor_erase_region_info *info = 
        (struct flash_nor_erase_region_info *)get_data;

      info->erase_region_count = 1;
      info->erase_region[0].block_count = 
        client_ctxt->flash_dev_ctxt->dev_info.u.nor.block_count;
      info->erase_region[0].block_size_in_bytes = 
        client_ctxt->client_data.pages_per_block *
        client_ctxt->client_data.page_size_bytes;
    }       
  }
  else
  {
    get_data = NULL;
    result = FLASH_DEVICE_FAIL;
  }

  return result;
}

/*------------------------------------------------------------------------------
  VTABLE Initialization
 ------------------------------------------------------------------------------*/

flash_vtbl nor_i2c_base_vtbl = {
  NULL,                      /* Power event */
  NULL,                      /* Close */
  NULL,                      /* De-init */
  NULL,                      /* Open partition */
  nor_i2c_get_info,          /* get_info */
  NULL,                      /* Get device status */
  NULL,                      /* Erase blocks */
  NULL,                      /* begin erase block */
  NULL,                      /* erase_suspend */
  NULL,                      /* erase_resume */
  NULL,                      /* Set block state */
  NULL,                      /* Get block state */
  nor_i2c_write_pages,       /* Write pages */
  NULL,                      /* write_partial_pages */
  NULL,                      /* write_2x_pages */
  nor_i2c_read_pages,        /* read pages */
  NULL,                      /* read_pointer */
  NULL,                      /* Copy pages */
  NULL,                      /* Get UUID */
  NULL,                      /* Erase NOR blocks by size */
  NULL,                      /* set power mode */
};
