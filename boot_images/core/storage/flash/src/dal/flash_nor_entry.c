/*=============================================================================
 *
 * FILE:      flash_nor_entry.c
 *
 * DESCRIPTION: Function and data structure declarations specific to the NOR
 *              entry layer
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_nor_entry.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/12/12     sv      Update flash profile macros
 * 03/19/12     sv      Add power management support for SPI NOR
 * 03/19/12     eo      Support for erase blocks by size
 * 07/20/11     eo      Add virtual to physical address conversion
 * 05/20/11     sv      Remove obtaining flash stats table from properties
 * 04/25/11     sv      Fix memory leak issue
 * 01/05/11     eo      Remove creation of DAL memory buffer descriptors
 * 11/05/10     jz      Added check of device_type_vtbl before calling its deinit
 * 10/14/10     eo      Fix compiler reported errors
 * 10/04/10     sv/nb   Move SPI specific initialization code to SPI NOR layer
 * 09/13/10     eo      Add formalized partitions support. 
 * 08/11/10     nr      Added stats framework support
 * 06/21/10     eo      Move power management to flash properties
 * 05/26/10     eo      include comdef.h needed in WM platform
 * 04/03/10     sv      memory allocation for SPI-NOR
 * 03/15/10     bb      Support for SPI-NOR
 * 12/28/09     sc      Added 2x support in DAL
 * 02/23/09     bb      Read/Write API changes with iovec design changes
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nor_entry.h"
#include <string.h>

/* NOR entry init data */
struct nor_entry_data nor_entry_init_data;

/*------------------------------------------------------------------------------
Following functions are extended in flash_handle Interface.
------------------------------------------------------------------------------*/
/*
 * Check Page lies within the partition
 */
static int flash_check_page_in_partition(flash_client_ctxt *client_ctxt,
  uint32 start_page, uint32 page_count)
{
  int result = FLASH_DEVICE_DONE;
  uint32 total_pages = 0;
  uint32 blk_size_shift = client_ctxt->client_data.block_size_shift;

  /* Get total number of pages in the partition */
  total_pages = (client_ctxt->client_data.partition_block_limit -
    client_ctxt->client_data.partition_block_start) << blk_size_shift;

  if (1 == page_count)
  {
    /* Ensure requested page does not exceed number of pages in partition */
    if (start_page >= total_pages)
    {
       result = FLASH_DEVICE_INVALID_PARAMETER;
    }
  }
  else
  {
    /* Ensure the following
     * - start and last page of the requested page_range does not exceed
     *   number of pages in partition
     */
    if ((start_page >= total_pages) ||
     ((start_page + page_count) > total_pages))
    {
      result = FLASH_DEVICE_INVALID_PARAMETER;
    }
  }

  return result;
}

/*
 * De-allocate all flash driver allocated memory
 */
static int nor_entry_mem_deinit(void)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_flash_meminfo *meminfo;

  meminfo = &nor_entry_init_data.meminfo;

  /* Free memory allocated by the driver */
  if (meminfo->handle_drv_mem)
  {
    if (DAL_SUCCESS != DALSYS_DestroyObject(
      (DALSYSObjHandle) meminfo->handle_drv_mem))
    {
      result = FLASH_DEVICE_FAIL;
    }
  }

  /* Reset all memory handles and flags */
  meminfo->handle_drv_mem = 0;
  nor_entry_init_data.nor_mem_initialized = FALSE;

  return result;
}

/*
 * Allocate memory for all driver usage
 */
static int nor_entry_mem_init(void)
{
  uint32 dal_mem_size = 0;
  uint32 alloc_attr;
  int result = FLASH_DEVICE_DONE;
  struct nor_flash_meminfo *meminfo;

  meminfo = &nor_entry_init_data.meminfo;

  /* If the memory is already initialized, then do nothin */
  if (TRUE == nor_entry_init_data.nor_mem_initialized)
  {
    return FLASH_DEVICE_DONE;
  }

  /* Clear the virtual to physical list */
  flash_vtop_init_list();

  /* Get the driver workspace memory size from property table */
  dal_mem_size = flash_get_property(FLASH_DRV_WS_BUFF_SIZE_ID);

  if (FLASH_PROPERTY_NOT_DEFINED == dal_mem_size)
  {
    FLASHLOG(2, ("Flash: Get DALWS Memory property failed!\n"));
    result = FLASH_DEVICE_FAIL;
  }

  /* Set the allocation attribute to allocate uncached - physically contiguous
   * region
   */
  alloc_attr = DALSYS_MEM_PROPS_PHYS_CONT | DALSYS_MEM_PROPS_UNCACHED;

  /* Workspace Memory for flash driver usage */
  if (FLASH_DEVICE_DONE == result)
  {
    if (DAL_SUCCESS != DALSYS_MemRegionAlloc(alloc_attr,
      DALSYS_MEM_ADDR_NOT_SPECIFIED, DALSYS_MEM_ADDR_NOT_SPECIFIED,
      dal_mem_size, &meminfo->handle_drv_mem, NULL))
    {
      FLASHLOG(2, ("Flash: DRVWS Memory allocation failed!\n"));
      result = FLASH_DEVICE_FAIL;
    }
    else
    {
      /* If the allocation is success, get the memory address
       * information */
      DALSYS_MemInfo(meminfo->handle_drv_mem,
        &meminfo->drv_mem_info);

      /* Set the memory initialization flag to TRUE */
      nor_entry_init_data.nor_mem_initialized = TRUE;
    }
  }

  /* If the memory allocation fails, de-allocate if any
   * allocated memory */
  if (FLASH_DEVICE_DONE != result)
  {
    nor_entry_mem_deinit();
  }
  else
  {
    /* Add DAL memory details to the virtual
     * to physical conversion list */
    flash_vtop_add_item(
      meminfo->drv_mem_info.VirtualAddr,
      meminfo->drv_mem_info.PhysicalAddr,
      meminfo->drv_mem_info.dwLen);
  }

  return result;
}

/*
 * Common initializations during NOR probe
 */
static int nor_entry_init(flash_client_ctxt *client_ctxt)
{
  uint32 temp_val;
  int result = FLASH_DEVICE_FAIL;

  result = nor_entry_mem_init();

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(3, ("Flash: Failed to initialize memory.\n"));
    goto exitfunction;
  }

  if (FLASH_DEVICE_DONE == result)
  {
    temp_val = flash_get_property(FLASH_MULTI_RW_SUPPORT_COUNT_ID);

    if (FLASH_PROPERTY_NOT_DEFINED == temp_val)
    {
      FLASHLOG(3, ("Flash: Failed to get RW Support count from property.\n"));
      result = FLASH_DEVICE_FAIL;
    }
    else
    {
      nor_entry_init_data.multi_rw_support_count = temp_val;
    }
  }

  if (FLASH_DEVICE_DONE == result)
  {
    temp_val = flash_get_property(FLASH_DAL_BUILD_ENV_ID);

    if (FLASH_PROPERTY_NOT_DEFINED == temp_val)
    {
      FLASHLOG(3, ("Flash: Failed to get build_env flag.\n"));
      result = FLASH_DEVICE_FAIL;
    }
    else
    {
      nor_entry_init_data.build_env = temp_val;
    }
  }

exitfunction:
  return result;
}

/*
 * API for power management.  At present this API does not do anything
 */
static int nor_entry_power_event(flash_client_ctxt *client_ctxt, DalPowerCmd
  power_cmd, DalPowerDomain power_domain)
{
  return FLASH_DEVICE_DONE;
}

/*
 * Handle to the driver close API  At present this API does not do anything
 */
static int nor_entry_close(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;

  if (FLASH_DEV_TYPE_VTBL(client_ctxt)->close)
  {
    /* Synchronize with other clients */
    DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
    result = FLASH_DEV_TYPE_VTBL(client_ctxt)->close(client_ctxt);
    DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  }

  return result;
}

/*
 * Handle to the driver deinit API.
 */
static int nor_entry_deinit (flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;

  if (FLASH_DEV_TYPE_VTBL(client_ctxt) && FLASH_DEV_TYPE_VTBL(client_ctxt)->deinit)
  {
    /* Synchronize with other clients */
    DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
    result = FLASH_DEV_TYPE_VTBL(client_ctxt)->deinit(client_ctxt);
    DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
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

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Interface with the NOR Partition Layer */
  result = nor_partition_update_limits(client_ctxt, partition_name);

  if (FLASH_DEVICE_DONE == result)
  {
    if (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->open_partition)
    {
      result =  FLASH_DEV_TYPE_VTBL(client_ctxt)->open_partition(client_ctxt,
        partition_name);
    }
  }

  /* Sync leave */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

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
    /* Synchronize with other clients */
    DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
    result =  FLASH_DEV_TYPE_VTBL(client_ctxt)->get_info(client_ctxt,
      info_type, flash_info);
    /* Sync leave */
    DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  }
  
  return result;
}

/*
 * This API gets device status information
 */
static int nor_entry_get_status(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_FAIL;

  if (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->get_status)
  {
    /* Synchronize with other clients */
    DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
    result =  FLASH_DEV_TYPE_VTBL(client_ctxt)->get_status(client_ctxt);
    /* Sync leave */
    DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  }

  return result;
}

/*
 * This API calls the handle to erase a range of blocks within
 * a partition.
 */
static int nor_entry_erase_blocks(flash_client_ctxt *client_ctxt,
  struct flash_block_vector *block_vector, uint32 iovec_count)
{
  int result = FLASH_DEVICE_DONE;
  struct flash_block_vector *tmp_block_vector = block_vector;
  uint32 tmp_iovec_count = iovec_count;

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  FLASH_CORE_PROFILE_START(FLASH_PROFILE_ERASE_BLOCKS, \
    FLASH_PROFILE_DEFAULT_NPAGES);

  /* For each iovec entry, ensure the block range is valid and lies
   * within the partition */
  while (tmp_iovec_count--)
  {
    if ((tmp_block_vector->start_block + tmp_block_vector->block_count - 1) >=
        (client_ctxt->client_data.partition_block_limit -
         client_ctxt->client_data.partition_block_start))
    {
      result = FLASH_DEVICE_INVALID_PARAMETER;
      break;
    }
    tmp_block_vector++;
  }

  if ((FLASH_DEVICE_DONE == result) && 
    (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->erase_blocks))
  {
    result =  FLASH_DEV_TYPE_VTBL(client_ctxt)->erase_blocks(client_ctxt,
      block_vector, iovec_count);
  }

  FLASH_CORE_PROFILE_END(FLASH_PROFILE_ERASE_BLOCKS);

  /* Sync leave */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  return result;
}

/*
 * This API calls the handle to initiate an erase of a block within
 * a partition.
 */
static int nor_entry_begin_erase_block(flash_client_ctxt *client_ctxt,
  uint32 block)
{
  int result = FLASH_DEVICE_DONE;

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Ensure the block number lies within the partition */
  if (block >=
    (client_ctxt->client_data.partition_block_limit -
    client_ctxt->client_data.partition_block_start))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  if ((FLASH_DEVICE_DONE == result) &&
    (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->begin_erase_block))
  {
    result =  FLASH_DEV_TYPE_VTBL(client_ctxt)->begin_erase_block(client_ctxt,
      block);
  }

  /* Sync leave */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  return result;
}

/*
 * This API calls the handle to suspend an erase operation of a 
 * block within a partition.
 */
static int nor_entry_erase_suspend(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_NOT_SUPPORTED;

  if (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->erase_suspend)
  {
    /* Synchronize with other clients */
    DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
    result = FLASH_DEV_TYPE_VTBL(client_ctxt)->erase_suspend(client_ctxt);
    /* Sync leave */
    DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  }

  return result;
}

/*
 * This API calls the handle to resume an erase operation of a 
 * block within a partition.
 */
static int nor_entry_erase_resume(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_NOT_SUPPORTED;

  if (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->erase_resume)
  {
    /* Synchronize with other clients */
    DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
    result = FLASH_DEV_TYPE_VTBL(client_ctxt)->erase_resume(client_ctxt);
    /* Sync leave */
    DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  }

  return result;
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
 * This API calls the handle to write page(s) within a partition
 */
static int nor_entry_write_pages(flash_client_ctxt *client_ctxt,
  enum page_write_opcode write_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec)
{
  int result = FLASH_DEVICE_DONE;

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  FLASH_CORE_PROFILE_START((FLASH_PROFILE_WRITE_MAIN + (int)write_type), \
    page_data_iovec->total_page_count);

  /* For each IOVEC, Ensure requested page(s) lies within the partition */
  if (FLASH_DEVICE_DONE != flash_check_page_in_partition(client_ctxt,
    page_data_iovec->start_page, page_data_iovec->total_page_count))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  if ((FLASH_DEVICE_DONE == result) &&
    (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->write_pages))
  {
    result = FLASH_DEV_TYPE_VTBL(client_ctxt)->write_pages(client_ctxt,
      write_type, buffer_desc, page_data_iovec);
  }

  FLASH_CORE_PROFILE_END((FLASH_PROFILE_WRITE_MAIN + (int)write_type));

  /* Sync leave */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  return result;
}

/*
 * This API calls the handle to write a partial page within a partition
 */
static int nor_entry_write_partial_page(flash_client_ctxt *client_ctxt,
  dalsys_mem_desc_list *buffer_desc,
  const struct flash_partial_page_op *partial_page_op)
{
  int result = FLASH_DEVICE_DONE;

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  FLASH_CORE_PROFILE_START(FLASH_PROFILE_WRITE_PARTIAL_PAGE, \
    partial_page_op->write_size_bytes);

  /* For each IOVEC, Ensure requested page lies within the partition */
  if (FLASH_DEVICE_DONE != flash_check_page_in_partition(client_ctxt,
    partial_page_op->page, 1))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  if ((FLASH_DEVICE_DONE == result) &&
    (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->write_partial_page))
  {
    result = FLASH_DEV_TYPE_VTBL(client_ctxt)->write_partial_page(client_ctxt,
      buffer_desc, partial_page_op);
  }

  FLASH_CORE_PROFILE_END(FLASH_PROFILE_WRITE_PARTIAL_PAGE);

  /* Sync leave */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  return result;
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
 * This API calls the handle to read page(s) within a partition
 */
static int nor_entry_read_pages(flash_client_ctxt *client_ctxt,
  enum page_read_opcode read_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec)
{
  int result = FLASH_DEVICE_DONE;

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  FLASH_CORE_PROFILE_START((FLASH_PROFILE_READ_MAIN + (int)read_type), \
    page_data_iovec->total_page_count);

  /* For each IOVEC, Ensure requested page(s) lies within the partition */
  if (FLASH_DEVICE_DONE != flash_check_page_in_partition(client_ctxt,
    page_data_iovec->start_page, page_data_iovec->total_page_count))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  if ((FLASH_DEVICE_DONE == result) &&
    (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->read_pages))
  {
    result = FLASH_DEV_TYPE_VTBL(client_ctxt)->read_pages(client_ctxt, 
      read_type, buffer_desc, page_data_iovec);
  }

  FLASH_CORE_PROFILE_END((FLASH_PROFILE_READ_MAIN + (int)read_type));

  /* Sync leave */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  return result;
}

/*
 * This API calls the handle to get read pointer address. 
 */
static int nor_entry_read_pointer(flash_client_ctxt *client_ctxt,
  void **read_addr, uint32 page)
{
  int result = FLASH_DEVICE_FAIL;

  if (FLASH_DEV_TYPE_VTBL(client_ctxt)->read_pointer)
  {
    /* Synchronize with other clients */
    DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
    result = FLASH_DEV_TYPE_VTBL(client_ctxt)->read_pointer(client_ctxt,
      read_addr, page);
    /* Sync leave */
    DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  }

  return result;
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
 * This API calls the handle to copy page(s) within a partition.
 * This operation is not supported.
 */
static int nor_entry_copy_pages(flash_client_ctxt *client_ctxt,
  struct flash_copy_iovec *copy_iovec, uint32 iovec_cnt)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/*
 * This API calls the handle to erase a range of sized blocks within
 * a partition.
 */
static int nor_entry_erase_blocks_by_size(flash_client_ctxt *client_ctxt,
  enum block_size_opcode block_size_type, 
  struct flash_block_vector *block_vector, uint32 iovec_count)
{
  int result = FLASH_DEVICE_DONE;
  struct flash_block_vector *tmp_block_vector = block_vector;
  uint32 tmp_iovec_count = iovec_count;

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  FLASH_CORE_PROFILE_START(FLASH_PROFILE_ERASE_BLOCKS, \
    FLASH_PROFILE_DEFAULT_NPAGES);

  /* Save client's default block parameter values */
  nor_entry_init_data.default_block_size_shift = 
    client_ctxt->client_data.block_size_shift;
  nor_entry_init_data.default_pages_per_block =
    client_ctxt->client_data.pages_per_block;
  nor_entry_init_data.default_parti_block_start = 
    client_ctxt->client_data.partition_block_start;
  nor_entry_init_data.default_parti_block_limit =
    client_ctxt->client_data.partition_block_limit;
    
  switch (block_size_type)
  {
    case BLOCK_SIZE_4KB:
    {
      client_ctxt->client_data.block_size_shift = 4;
      client_ctxt->client_data.pages_per_block = 16;
      client_ctxt->client_data.partition_block_start = 
        client_ctxt->client_data.partition_block_start *
        (nor_entry_init_data.default_pages_per_block >> 4);
      client_ctxt->client_data.partition_block_limit = 
        client_ctxt->client_data.partition_block_limit *
        (nor_entry_init_data.default_pages_per_block >> 4);
      break;
    }
    case BLOCK_SIZE_32KB:
    {
      client_ctxt->client_data.block_size_shift = 7;
      client_ctxt->client_data.pages_per_block = 128;
      if (nor_entry_init_data.default_pages_per_block > 128)
      {
        client_ctxt->client_data.partition_block_start = 
          client_ctxt->client_data.partition_block_start *
          (nor_entry_init_data.default_pages_per_block >> 7);
        client_ctxt->client_data.partition_block_limit = 
          client_ctxt->client_data.partition_block_limit *
          (nor_entry_init_data.default_pages_per_block >> 7);
      }
      else if (nor_entry_init_data.default_pages_per_block < 128)
      {
        client_ctxt->client_data.partition_block_start = 
          client_ctxt->client_data.partition_block_start /
          (128 >> nor_entry_init_data.default_block_size_shift);
        client_ctxt->client_data.partition_block_limit = 
          client_ctxt->client_data.partition_block_limit /
          (128 >> nor_entry_init_data.default_block_size_shift);
      }      
      break;
    }
    case BLOCK_SIZE_64KB:
    {
      client_ctxt->client_data.block_size_shift = 8;
      client_ctxt->client_data.pages_per_block = 256;
      client_ctxt->client_data.partition_block_start = 
        client_ctxt->client_data.partition_block_start /
        (256 >> nor_entry_init_data.default_block_size_shift);
      client_ctxt->client_data.partition_block_limit = 
        client_ctxt->client_data.partition_block_limit /
        (256 >> nor_entry_init_data.default_block_size_shift);
      break;
    }
    default:
    {
      result = FLASH_DEVICE_INVALID_PARAMETER;
      break;
    }
  }
  
  if (FLASH_DEVICE_DONE == result)
  {      
    /* For each iovec entry, ensure the block range is valid and lies
     * within the partition */
    while (tmp_iovec_count--)
    {
      /* For each IOVEC, ensure requested block(s) lie within partition range*/
      if ((tmp_block_vector->start_block + tmp_block_vector->block_count - 1) >=
          (client_ctxt->client_data.partition_block_limit -
           client_ctxt->client_data.partition_block_start))
      {
        result = FLASH_DEVICE_INVALID_PARAMETER;
        break;
      }
    
      tmp_block_vector++;
    }
  }

  if ((FLASH_DEVICE_DONE == result) && 
    (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->erase_blocks_by_size))
  {
    result = FLASH_DEV_TYPE_VTBL(client_ctxt)->erase_blocks_by_size(client_ctxt,
      block_size_type, block_vector, iovec_count);
  }
  
  /* Restore client's default block parameter values */
  client_ctxt->client_data.block_size_shift =
    nor_entry_init_data.default_block_size_shift;
  client_ctxt->client_data.pages_per_block =
    nor_entry_init_data.default_pages_per_block;
  client_ctxt->client_data.partition_block_start =
    nor_entry_init_data.default_parti_block_start;
  client_ctxt->client_data.partition_block_limit =
    nor_entry_init_data.default_parti_block_limit;

  FLASH_CORE_PROFILE_END(FLASH_PROFILE_ERASE_BLOCKS);

  /* Sync leave */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  return result;
}

/*
 * This API calls the handle to set the power mode for the flash hardware
 */
static int nor_entry_set_power_mode(flash_client_ctxt *client_ctxt, 
  enum flash_power_mode power_mode)
{
  int result = FLASH_DEVICE_NOT_SUPPORTED;
  
  if((power_mode != FLASH_POWER_LOW_PWR_MODE) && 
    (power_mode != FLASH_POWER_NORMAL_MODE))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }  
  else
  {
    if (NULL != FLASH_DEV_TYPE_VTBL(client_ctxt)->set_power_mode)
    {
      /* Synchronize with other clients */
      DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
      result = FLASH_DEV_TYPE_VTBL(client_ctxt)->set_power_mode(client_ctxt, 
        power_mode);
      /* Sync leave */
      DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
    }
  }

  return result;
}

/*
 * Probe the NOR device
 */
int nor_entry_device_probe(flash_client_ctxt *client_ctxt)
{
  int status = FLASH_DEVICE_NOT_SUPPORTED;
  int init_status = FLASH_DEVICE_FAIL;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  flash_probe_tbl *nor_prober = 
    (flash_probe_tbl *) flash_get_property(FLASH_PROBE_NOR_TABLE_ID);

  /* Initialize nor_entry_vtbl to the nor_entry APIs */
  static flash_vtbl nor_entry_vtbl = {
    nor_entry_power_event,
    nor_entry_close,
    nor_entry_deinit,
    nor_entry_open_partition,
    nor_entry_get_info,
    nor_entry_get_status,
    nor_entry_erase_blocks,
    nor_entry_begin_erase_block,
    nor_entry_erase_suspend,
    nor_entry_erase_resume,
    nor_entry_block_set_state,
    nor_entry_block_get_state,
    nor_entry_write_pages,
    nor_entry_write_partial_page,
    nor_entry_write_2x_pages,
    nor_entry_read_pages,
    nor_entry_read_pointer,
    nor_entry_copy_pages,
    nor_entry_get_uuid,
    nor_entry_erase_blocks_by_size,
    nor_entry_set_power_mode
  };

  /* Initialize memory for NOR driver to use */
  if ((FLASH_PROPERTY_NOT_DEFINED != (uint32) nor_prober)  &&
      (nor_prober->device_prober))
  {
    init_status = nor_entry_init(client_ctxt);
  }

  if (FLASH_DEVICE_DONE != init_status)
  {
    /* Error Fatal */
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                    "Flash: Failed to do initialization for probe!");
  }
  else
  {
    /* Synchronize with other clients */
    DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* Init the Statistics framework */
    FLASH_CORE_PROFILE_INIT();

    dev_ctxt->dev_entry_vtbl = &nor_entry_vtbl;

    while ((FLASH_PROPERTY_NOT_DEFINED != (uint32)nor_prober) &&
           (nor_prober->device_prober) )
    {
      status = nor_prober->device_prober(client_ctxt);

      if (status == FLASH_DEVICE_DONE)
      {
        break;
      }
      else
      {
        ++nor_prober;
      }
    }

    DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    if (FLASH_DEVICE_DONE == status)
    {
      FLASH_CORE_PROFILE_SET_INFO(client_ctxt);
    }
	else
	{
      nor_entry_mem_deinit();
    }
  }    

  return status;
}
