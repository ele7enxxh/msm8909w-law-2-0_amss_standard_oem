/*=============================================================================
 *
 * FILE:      flash_nor_entry_reduced.c
 *
 * DESCRIPTION: Function and data structure declarations specific to the NOR
 *              entry layer in boot mode
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_nor_entry_reduced.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 03/19/12      sv       Adapt to changes for SPI NOR power management
 * 03/19/12      eo       Support for erase blocks by size
 * 05/20/11      sv       Flash driver Optimization
 * 10/11/10      bn       Initial Version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nor_entry.h"

/*------------------------------------------------------------------------------
Following functions are extended in flash_handle Interface.
------------------------------------------------------------------------------*/

/*
 * Handle to the driver close API  At present this API does not do anything
 */
static int nor_entry_close(flash_client_ctxt *client_ctxt)
{
  return FLASH_DEVICE_DONE;
}

/*
 * Handle to the driver deinit API.
 */
static int nor_entry_deinit (flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;

  if (FLASH_DEV_TYPE_VTBL(client_ctxt)->deinit)
  {
    result = FLASH_DEV_TYPE_VTBL(client_ctxt)->deinit(client_ctxt);
  }

  return result;
}
/*
 * Handle to the open partition request
 */
static int nor_entry_open_partition(flash_client_ctxt *client_ctxt, 
  const unsigned char *partition_name)
{
  int result = FLASH_DEVICE_FAIL;

  /* Set Client Ctxt default Vtbl to the Base Vtbl in Device Ctxt */
  client_ctxt->device_type_vtbl = client_ctxt->flash_dev_ctxt->dev_base_vtbl;

  if (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->open_partition)
  {
    result =  FLASH_DEV_TYPE_VTBL(client_ctxt)->open_partition(client_ctxt,
      partition_name);
  }

  return result;
}

/*
 * This API gets device and parition specific information
 */
static int nor_entry_get_info(flash_client_ctxt *client_ctxt,
  enum flash_info_type info_type, void *flash_info)
{
  int result = FLASH_DEVICE_FAIL;

  if (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->get_info)
  {
    result =  FLASH_DEV_TYPE_VTBL(client_ctxt)->get_info(client_ctxt,
      info_type, flash_info);
  }

  return result;
}

/*
 * This API calls the handle to write page(s) within a partition
 */
static int nor_entry_write_pages(flash_client_ctxt *client_ctxt,
  enum page_write_opcode write_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec)
{
  int result = FLASH_DEVICE_FAIL;

  if (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->write_pages)
  {
    result =  FLASH_DEV_TYPE_VTBL(client_ctxt)->write_pages(client_ctxt,
    write_type, buffer_desc, page_data_iovec);
  }

  return result;
}

/*
 * This API calls the handle to read page(s) within a partition
 */
static int nor_entry_read_pages(flash_client_ctxt *client_ctxt,
  enum page_read_opcode read_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec)
{
  int result = FLASH_DEVICE_FAIL;

  if (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->read_pages)
  {
    result =  FLASH_DEV_TYPE_VTBL(client_ctxt)->read_pages(client_ctxt, 
    read_type, buffer_desc, page_data_iovec);
  }

  return result;
}

/*
 * API for power management.  This operation is not supported.
 */
static int nor_entry_power_event(flash_client_ctxt *client_ctxt, DalPowerCmd
  power_cmd, DalPowerDomain power_domain)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API gets device status information. 
 * This operation is not supported.
 */
static int nor_entry_get_status(flash_client_ctxt *client_ctxt)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to erase a range of blocks within
 * a partition. This operation is not supported.
 */
static int nor_entry_erase_blocks(flash_client_ctxt *client_ctxt,
  struct flash_block_vector *block_vector, uint32 iovec_count)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to initiate an erase of a block within
 * a partition. This operation is not supported.
 */
static int nor_entry_begin_erase_block(flash_client_ctxt *client_ctxt,
  uint32 block)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to suspend an erase operation of a 
 * block within a partition. This operation is not supported.
 */
static int nor_entry_erase_suspend(flash_client_ctxt *client_ctxt)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to resume an erase operation of a 
 * block within a partition. This operation is not supported.
 */
static int nor_entry_erase_resume(flash_client_ctxt *client_ctxt)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to set the status of a block
 * This operation is not supported.
 */
static int nor_entry_block_set_state(flash_client_ctxt *client_ctxt, uint32 block,
  enum flash_block_state block_state)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to get the status of a block
 * This operation is not supported.
 */
static int nor_entry_block_get_state(flash_client_ctxt *client_ctxt,
  uint32 block, enum flash_block_state *block_state)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to write a partial page within a partition
 * This operation is not supported.
 */
static int nor_entry_write_partial_page(flash_client_ctxt *client_ctxt,
  dalsys_mem_desc_list *buffer_desc,
  const struct flash_partial_page_op *partial_page_op)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to write 2x pages
 * This operation is not supported.
 */
static int nor_entry_write_2x_pages(flash_client_ctxt *client_ctxt,
  enum page_write_opcode write_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_2x_page_iovec *page_2x_data_iovec)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to get read pointer address. 
 * This operation is not supported.
 */
static int nor_entry_read_pointer(flash_client_ctxt *client_ctxt,
  void **read_addr, uint32 page)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to copy page(s) within a partition.
 * This operation is not supported.
 */
static int nor_entry_copy_pages(flash_client_ctxt *client_ctxt,
  struct flash_copy_iovec *copy_iovec, uint32 iovec_cnt)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle get_uuid(s) from a device.
 * This operation is not supported.
 */
static int nor_entry_get_uuid(flash_client_ctxt *client_ctxt, void *uuid_buf,
  uint32 uuid_buf_len, uint32 *uuid_len)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to erased blocks by size specified.
 * This operation is not supported.
 */
static int nor_entry_erase_blocks_by_size(flash_client_ctxt *client_ctxt,
  enum block_size_opcode block_size_type, 
  struct flash_block_vector *block_vector, uint32 iovec_count)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to set the power mode for the flash hardware
 */
static int nor_entry_set_power_mode(flash_client_ctxt *client_ctxt, 
  enum flash_power_mode power_mode)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * Probe the NOR device
 */
int nor_entry_device_probe_reduced(flash_client_ctxt *client_ctxt)
{
  int status = FLASH_DEVICE_NOT_SUPPORTED;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  flash_probe_tbl *nor_prober = 
    (flash_probe_tbl *) flash_get_property(FLASH_PROBE_NOR_TABLE_REDUCED_ID);

  /* Initialize nor_entry_vtbl to the nor_entry APIs */
  static flash_vtbl nor_entry_vtbl = {
    nor_entry_power_event,        /* power_event */
    nor_entry_close,              /* close */
    nor_entry_deinit,             /* deinit */ 
    nor_entry_open_partition,     /* open_partition */
    nor_entry_get_info,           /* get_info */
    nor_entry_get_status,         /* get_status */
    nor_entry_erase_blocks,       /* erase_blocks */
    nor_entry_begin_erase_block,  /* begin_erase_block */
    nor_entry_erase_suspend,      /* erase_suspend */
    nor_entry_erase_resume,       /* erase_resume */
    nor_entry_block_set_state,    /* block_set_state */
    nor_entry_block_get_state,    /* block_get_state */
    nor_entry_write_pages,        /* write_pages */
    nor_entry_write_partial_page, /* write_partial_pages */
    nor_entry_write_2x_pages,     /* write_2x_pages */
    nor_entry_read_pages,         /* read_pages */
    nor_entry_read_pointer,       /* read_pointer */
    nor_entry_copy_pages,         /* copy_pages */
    nor_entry_get_uuid,           /* read_otp_page */
    nor_entry_erase_blocks_by_size, /* erase_blocks_by_size */
    nor_entry_set_power_mode      /* Set power mode */
  };

  dev_ctxt->dev_entry_vtbl = &nor_entry_vtbl;

  if ((FLASH_PROPERTY_NOT_DEFINED != (uint32)nor_prober) && 
    (nor_prober->device_prober))
  {
    /* Probe device */
    while ((NULL != nor_prober) && (nor_prober->device_prober))
    {
      status = nor_prober->device_prober(client_ctxt);

      if (status == FLASH_DEVICE_DONE)
      {
        break;
      }

      ++nor_prober;
    }
  }
  
  return status;
}
