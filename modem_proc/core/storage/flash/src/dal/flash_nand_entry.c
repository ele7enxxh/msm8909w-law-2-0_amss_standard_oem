/*=============================================================================
 *
 * FILE:      flash_nand_entry.c
 *
 * DESCRIPTION: Function and data structure declarations specific to the NAND
 *             entry layer
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_nand_entry.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/03/14     sb      Fix KW warnings
 * 10/03/13     sb      Expose API to set block state
 * 06/21/13     sb      Expose write pages and erase blocks APIs
 * 05/03/13     eo      Add flash ops mechanism to enable/disable QPIC clocks
 * 11/01/12     sv      Call DALSYS_MemRegionAlloc only if size is not 0
 * 09/05/12     bb/sv   cleanups
 * 07/27/12     sv      BAM based NAND support/Remove 2x support codes
 * 01/18/12     sv      Adapt to changes for SPI NOR power management
 * 01/03/12     sv      Update Flash profile macros/flash diag support
 * 10/27/11     sb      Fixing Compiler warnings
 * 09/22/11     eo      Support for erase blocks by size
 * 05/20/11     bb      Flash driver Optimization
 * 11/04/10     jz      Added check of device_type_vtbl before calling its deinit
 * 10/21/10     sv      set the hal property for wait function config
 * 10/07/10     eo      Return Not Supported status for unsupported operations
 * 08/11/10     nr      Added stats framework support
 * 04/25/10     jz      Updated for 2X support, fixed block_set_state logic
 * 04/22/10     bb      Support for isolating NOR build from NAND
 * 04/05/10     bb      Fix for bad block optimization
 * 01/28/10     sb      Bad block optimization
 * 02/12/10     wt      Modified Babylon Marker for 2X support
 * 02/08/10     bb      Clean up and moved common code to nand entry layer
 * 12/28/09     sc      Added 2x support in DAL
 * 10/25/09     mh      Rename nand_partittion_open to nand_partition_update_limit
 * 10/15/09     sv      Update Babylon marker na
 * 08/13/09     bb      Added support for optimized copy & multi r/w APIs
 * 09/24/09     eo      Remove status check in phys to user buffers copy
 * 06/21/09     sv      HAL memutil changes/allocate cached memory for non DM
 * 06/03/09     mh      Fix bug in check for page within partition
 * 05/11/09     jm,bb   Support for babylon
 * 04/20/09     bb      Updates for DAL/HAL optimizations
 * 03/24/09     bb      Used safe string copy function
 * 02/23/09     bb      Read/Write API changes with iovec design changes
 * 01/30/09     bb      Moved memory init from driver to nand entry
 * 11/15/08     mm      Add properties for NAND and NOR Probe table
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nand_entry.h"
#include "flash_nand.h"
#include "flash_ops_setup.h"
#include "flash_clocks.h"

#include <string.h>

/* NAND entry init data */
struct nand_entry_data nand_entry_init_data;

/* A variable to allow flash tool utilities to reset client config
 * if needed via write breakpoint on 0:ALL partition based
 * reads and writes */
static byte tools_can_update_config = 0;
/*===================================================================
 *
 *   F L A S H   D R I V E R   I N I T   R O U T I  N E S
 *
 ====================================================================*/

/*
 * De-allocate all flash driver allocated memory
 */
static int nand_entry_mem_deinit(void)
{
  int result = FLASH_DEVICE_DONE;
  struct nand_flash_meminfo *meminfo;

  meminfo = &nand_entry_init_data.meminfo;

  /* Free memory allocated for the HAL */
  if (meminfo->handle_hal_mem)
  {
    if (DAL_SUCCESS != DALSYS_DestroyObject(
      (DALSYSObjHandle) meminfo->handle_hal_mem))
    {
      result = FLASH_DEVICE_FAIL;
    }
  }

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
  meminfo->handle_hal_mem = 0;

  nand_entry_init_data.nand_mem_initialized = FALSE;

  return result;
}

/*
 * Allocate memory for all driver usage
 */
int nand_entry_mem_alloc(uint32 hal_mem_size, uint32 dal_mem_size)
{
  uint32 alloc_attr;
  int result = FLASH_DEVICE_DONE;
  struct nand_flash_meminfo *meminfo;

  /* If the memory is already initialized, then do nothin */
  if (TRUE == nand_entry_init_data.nand_mem_initialized)
  {
    result = nand_entry_mem_deinit();
  }

  meminfo = &nand_entry_init_data.meminfo;

  /* Clear the virtual to physical list */
  flash_vtop_init_list();

  /* Set the allocation attribute to allocate cached - physically
   * contiguous region for non DM and uncached - physically contiguous
   * region incase of DM
   */
  alloc_attr = DALSYS_MEM_PROPS_PHYS_CONT;

  if (FLASH_HAL_DMA_ENABLE == nand_entry_init_data.dma_state)
  {
    alloc_attr |= DALSYS_MEM_PROPS_UNCACHED;
  }

  /* Allocate Memory for HAL usage */
  if ((FLASH_DEVICE_DONE == result) && hal_mem_size)
  {
    if (DAL_SUCCESS != DALSYS_MemRegionAlloc(alloc_attr,
      DALSYS_MEM_ADDR_NOT_SPECIFIED, DALSYS_MEM_ADDR_NOT_SPECIFIED,
      hal_mem_size, &meminfo->handle_hal_mem, NULL))
    {
      FLASHLOG(2, ("Flash: HALWS Memory allocation failed!\n"));
      result = FLASH_DEVICE_FAIL;
    }
    else
    {
      /* If the allocation is success, get the memory address
       * information */
      DALSYS_MemInfo(meminfo->handle_hal_mem,
        &meminfo->hal_mem_info);

      /*  Need to touch the memory  for it to be configured in
       *  MMU table for ARM11 */
      DALSYS_memset((void *)meminfo->hal_mem_info.VirtualAddr,
        0xFF, 10);
    }
  }

  /* Workspace Memory for flash driver usage */
  if ((FLASH_DEVICE_DONE == result) && dal_mem_size)
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
      DALSYS_MemInfo(meminfo->handle_drv_mem, &meminfo->drv_mem_info);

      /* Set the memory initialization flag to TRUE */
      nand_entry_init_data.nand_mem_initialized = TRUE;
    }
  }

  /* If the memory allocation fails, de-allocate if any
   * allocated memory */
  if (FLASH_DEVICE_DONE != result)
  {
    nand_entry_mem_deinit();
  }
  else
  {
    /* Add the HAL / DAL memory details to the virtual
     * to physical conversion list */
    flash_vtop_add_item(
      meminfo->hal_mem_info.VirtualAddr,
      meminfo->hal_mem_info.PhysicalAddr,
      meminfo->hal_mem_info.dwLen);

    flash_vtop_add_item(
      meminfo->drv_mem_info.VirtualAddr,
      meminfo->drv_mem_info.PhysicalAddr,
      meminfo->drv_mem_info.dwLen);
  }

  return result;
}

/*
 * Allocate memory for all driver usage
 */
static int nand_entry_mem_init(void)
{
  uint32 hal_mem_size = 0;
  uint32 dal_mem_size = 0;
  uint32 temp_val;
  int result = FLASH_DEVICE_DONE;

  /* If the memory is already initialized, then do nothin */
  if (TRUE == nand_entry_init_data.nand_mem_initialized)
  {
    return FLASH_DEVICE_DONE;
  }

  /* Get the HAL memory size from property table */
  hal_mem_size = flash_get_property(FLASH_HAL_WS_BUFF_SIZE_ID);

  /* Get the driver workspace memory size from property table */
  dal_mem_size = flash_get_property(FLASH_DRV_WS_BUFF_SIZE_ID);

  if ((FLASH_PROPERTY_NOT_DEFINED == hal_mem_size) ||
      (FLASH_PROPERTY_NOT_DEFINED == dal_mem_size))
  {
    FLASHLOG(2, ("Flash: Get HALWS Memory property failed!\n"));
    result = FLASH_DEVICE_FAIL;
  }

  result = nand_entry_mem_alloc(hal_mem_size, dal_mem_size);

  /* Get the memory utils api type to be used in HAL */
  temp_val = flash_get_property(FLASH_HAL_MEM_UTIL_TYPE_ID);

  if(FLASH_HAL_USE_INT_MEMUTIL == temp_val)
  {
    /* Set the HAL property to use Hal defined mem util which handles
     * issues with memory operations on uncached memory */
    flash_hal_set_property(FLASH_HAL_PROPERTY_USE_INT_MEMUTIL, NULL);
  }
  else
  {
    /* Use standard mem util */
    flash_hal_set_property(FLASH_HAL_PROPERTY_USE_INT_ASM_MEMUTIL, NULL);
  }

  return result;
}

/*
 * Common initializations during NAND probe
 */
static int nand_entry_init(flash_client_ctxt *client_ctxt)
{
  uint32 temp_val;
  int result;

  /* Get the default DMA state from property table */
  temp_val = flash_get_property(FLASH_DMA_STATE_ID);

  if (FLASH_PROPERTY_NOT_DEFINED != temp_val)
  {
    if (FLASH_HAL_DMA_ENABLE == temp_val)
    {
      nand_entry_init_data.dma_state = FLASH_HAL_DMA_ENABLE;
    }
  }
  /* Initialize memory for probe */
  result = nand_entry_mem_init();

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(3, ("Flash: Failed to initialize memory.\n"));
  }
  else
  {
    nand_entry_init_data.multi_rw_support_count = 1;
    /* Set the wait function to be used by Flash HAL */
    flash_hal_set_property(FLASH_HAL_PROPERTY_WAIT_CFG,
      (void *)&flash_busy_wait);

    if (DO_CACHE_MAINTENANCE)
    {
      /* Set the cache invalidate function to be used by Flash HAL */
      flash_hal_set_property(FLASH_HAL_PROPERTY_CACHE_INVAL_CFG,
        (void *)&flash_dcache_inval);

      /* Set the cache clean function to be used by Flash HAL */
      flash_hal_set_property(FLASH_HAL_PROPERTY_CACHE_CLEAN_CFG,
        (void *)&flash_dcache_clean);
    }
  }

  return result;
}

/*===================================================================
 *
 *   F L A S H   D R I V E R   V A L I D A T I O N   R O U T I N E S
 *
 ====================================================================*/

/*
 * Create the DMA friendly memory buffer descriptors
 */
static __inline int flash_get_dma_buffers(
  struct nand_entry_client_data *client_data, dalsys_mem_desc_list *buff_list,
  dalsys_mem_desc_list **phys_buff_list, uint32 operation)
{
  int result = FLASH_DEVICE_DONE;

  /* Create the DMA friendly memory from the given user buffer list.*/
  /* note: nand_phys_handle is initialized once (when any of the buffer is not
   *       dma friendly (not 32 byte aligned or not physically contiguous
   *       This is then reused each time
   */
  if (FLASH_HAL_DMA_ENABLE == nand_entry_init_data.dma_state)
  {
    client_data->nand_phys_handle = DALFW_AllocPhysForMemDescBufs(
      client_data->nand_phys_handle, operation, buff_list, phys_buff_list);

    if ((uint32) client_data->nand_phys_handle == DALSYS_MEM_ADDR_NOT_SPECIFIED)
    {
      result = FLASH_DEVICE_NO_MEM;
    }
  }
  else
  {
    *phys_buff_list = buff_list;
  }

  return result;
}

/*
 * Copy data between two buffer descriptors
 */
static __inline int flash_copy_dma_buffers(DALSysMemDescList * dest_list,
  DALSysMemDescList * src_list)
{
  int result = FLASH_DEVICE_DONE;

  /* Copy the buffer descriptor, only when we use DAL buffer
     descriptors, ie, when DMA is enabled */
  if (FLASH_HAL_DMA_ENABLE == nand_entry_init_data.dma_state)
  {
    if (DAL_SUCCESS != DALFW_MemDescListCopyBufs(dest_list, src_list))
    {
      result = FLASH_DEVICE_FAIL;
    }
  }

  return result;
}

/*
 * Check Page lies within the partition
 */
static int flash_nand_check_page_in_partition(flash_client_ctxt *client_ctxt,
  uint32 start_page, uint32 page_count)
{
  uint32 total_pages = 0;
  int result = FLASH_DEVICE_DONE;
  uint32 blk_size_shift = client_ctxt->client_data.block_size_shift;

  /* Get total number of pages in the partition */
  total_pages = (client_ctxt->client_data.partition_block_limit -
    client_ctxt->client_data.partition_block_start) << blk_size_shift ;

  if(1 == page_count)
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
       - start and last page of the requested page_range does not exceed
         number of pages in partition
       - the page range does not lie cross two blocks

    */
    if ((start_page >= total_pages) ||
     ((start_page + page_count) > total_pages) ||
     (((start_page & (client_ctxt->client_data.pages_per_block - 1)) +
      page_count) > client_ctxt->client_data.pages_per_block))
    {
      result = FLASH_DEVICE_INVALID_PARAMETER;
    }
  }

  return result;
}

/*
 * Check we have enough buffer descriptors to operate with
 */
static int flash_nand_check_buff_desc(dalsys_mem_desc_list *buffer_desc,
  int32 total_page_count)
{
  int result = FLASH_DEVICE_DONE;
  int32 page_count = 0, buff_desc_count = 0;
  DALSysMemDescBuf *buf_desc;

  buff_desc_count = buffer_desc->dwNumDescBufs;

  if (buff_desc_count > 2)
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  /* Get the first buffer descriptor */
  buf_desc = DALFW_MemDescBufPtr(buffer_desc, 0);

  /* Verify that we have enough number of buffer
     descriptors for the given operation by
     checking the total page count equals to
     total number of pages given in the buffer
     descriptors */
  while ((page_count > 0) && (buff_desc_count > 0))
  {
    page_count -= buf_desc->user;
    buff_desc_count -= 2;
    buf_desc += 2;
  }

  if (page_count > 0)
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  return result;
}

/*===================================================================
 *
 *   F L A S H   D R I V E R   E N T R Y   R O U T I N E S
 *
 ====================================================================*/

/*
 * Handle to the driver close API  At present this API does not do anything
 */
static int nand_entry_close(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  result = nand_close(client_ctxt);
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  return result;
}

/*
 * Handle to the driver deinit API  At present this API does not do anything
 */
static int nand_entry_deinit (flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  result = nand_deinit(client_ctxt);
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  /* TBD: do the cleanup of the DAL critical section/ synchronization
   * handles */

  return result;
}

/*
 * Handle to the Open partiton request
 */
static int nand_entry_open_partition(flash_client_ctxt *client_ctxt,
  const unsigned char *partition_name)
{
  int result = FLASH_DEVICE_DONE;

  FLASHLOG (3, (" nand_entry_open_partition %s\n", partition_name));

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Interface with the NAND Partion Layer */
  result = nand_partition_update_limits(client_ctxt, partition_name);

  tools_can_update_config = 1;

  (void)tools_can_update_config;

  if (FLASH_DEVICE_DONE == result)
  {
    result = nand_open_partition(client_ctxt, partition_name);
  }

  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  return result;
}

/*
 * This API gets device and parition specific information
 */
static int nand_entry_get_info(flash_client_ctxt *client_ctxt,
  enum flash_info_type info_type, void *get_data)
{
  int result = FLASH_DEVICE_DONE;

  if (client_ctxt->refs)
  {
    if (FLASH_DEVICE_INFO == info_type)
    {
      struct flash_info *info = (struct flash_info *) get_data;

      /* Copy the device name */
      flash_strcpy((uint8 *) info->device_name,
        (uint8 *)client_ctxt->flash_dev_ctxt->dev_info.common.device_name,32);

      /* Get Device type and width  */
      info->type = client_ctxt->flash_dev_ctxt->dev_info.common.type;
      info->tech = client_ctxt->flash_dev_ctxt->dev_info.common.tech;
      info->width = client_ctxt->flash_dev_ctxt->dev_info.common.width;
      info->block_count = client_ctxt->flash_dev_ctxt->dev_info.u.nand.block_count;

      info->write_style =
        client_ctxt->flash_dev_ctxt->dev_info.common.write_style;

      /* Get Device manufacturer/make ID, device ID, version  */
      info->maker_id = client_ctxt->flash_dev_ctxt->dev_info.u.nand.maker_id;
      info->device_id = client_ctxt->flash_dev_ctxt->dev_info.u.nand.device_id;
      info->version_id =
        client_ctxt->flash_dev_ctxt->dev_info.u.nand.version_id;

      /* Get Partition specific information  */
      info->partition.ecc_state = client_ctxt->client_data.ecc_state;
      info->partition.block_count = client_ctxt->client_data.block_count;
      info->partition.pages_per_block =
        client_ctxt->client_data.pages_per_block;
      info->partition.page_size_bytes =
        client_ctxt->client_data.page_size_bytes;
      info->partition.total_page_size_bytes =
        client_ctxt->client_data.total_page_size_bytes;
      info->partition.max_spare_udata_bytes =
        client_ctxt->client_data.max_spare_udata_bytes;
      info->partition.physical_start_block =
        client_ctxt->client_data.partition_block_start;
      info->partition.attributes =
        client_ctxt->client_data.partition_attributes;
      info->partition.reserved1 =
        client_ctxt->flash_dev_ctxt->dev_info.u.nand.flags1;
      info->partition.reserved2 =
        client_ctxt->flash_dev_ctxt->dev_info.u.nand.flags2;

      /* Get MIBIB block numbers */
      info->mibib_info.old_mibib_block =
        client_ctxt->flash_dev_ctxt->mibib_info.old_mibib_block;
      info->mibib_info.new_mibib_block =
        client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block;
    }
    else
    {
      get_data = NULL;
      result = FLASH_DEVICE_FAIL;
    }
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(FLASH_DEVICE_DONE);

  return result;
}

/*
 * This API calls the handle to to erase a range of blocks with a partition
 */
int nand_entry_erase_blocks(flash_client_ctxt *client_ctxt,
  struct flash_block_vector *block_vector, uint32 iovec_count)
{
  int result = FLASH_DEVICE_DONE;
  struct flash_block_vector *tmp_block_vector = block_vector;
  uint32 tmp_iovec_count = iovec_count;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_ops_begin_op();
  flash_clocks_request();

  FLASH_CORE_PROFILE_START(FLASH_PROFILE_ERASE_BLOCKS, \
    FLASH_PROFILE_DEFAULT_NPAGES);

  /* For each iovec entry, ensure the blokc range is valid and lies
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
    
    if(tmp_iovec_count)
    {
      tmp_block_vector++;
    }  
  }

  /* If valid range of blocks, start  erasing the blocks */
  if (FLASH_DEVICE_DONE == result)
  {
    result = nand_erase_blocks(client_ctxt, block_vector, iovec_count);
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  FLASH_CORE_PROFILE_END(FLASH_PROFILE_ERASE_BLOCKS);

  flash_clocks_request_complete();
  /* Leave Synchronization */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}

/*
 * This API calls the handle to set the status of a block
 */
int nand_entry_block_set_state(flash_client_ctxt *client_ctxt, uint32 block,
  enum flash_block_state block_state)
{
  int result = FLASH_DEVICE_DONE;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_ops_begin_op();
  flash_clocks_request();

  /* Ensure the block is valid and lies within the partition */
  if (block >= (client_ctxt->client_data.partition_block_limit -
      client_ctxt->client_data.partition_block_start))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  result = nand_set_block_state(client_ctxt, block, block_state);

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  flash_clocks_request_complete();
    /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}

/*
 * This API calls the handle to get the status of a block
 */
int nand_entry_block_get_state(flash_client_ctxt *client_ctxt,
  uint32 block, enum flash_block_state *block_state)
{
  int result = FLASH_DEVICE_DONE;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_ops_begin_op();
  flash_clocks_request();

  /* Ensure the block is valid and lies within the partition */
  if (block >= (client_ctxt->client_data.partition_block_limit -
      client_ctxt->client_data.partition_block_start))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  result = nand_get_block_state(client_ctxt, block, block_state);

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  flash_clocks_request_complete();
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}

/*
 * This API calls the handle to write page(s) within a partition
 */
int nand_entry_write_pages(flash_client_ctxt *client_ctxt,
  enum page_write_opcode write_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec)
{
  int result = FLASH_DEVICE_DONE;
  struct nand_entry_client_data *client_data;
  dalsys_mem_desc_list *phys_buf_list;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_ops_begin_op();
  flash_clocks_request();

  FLASH_CORE_PROFILE_START((FLASH_PROFILE_WRITE_MAIN + (int)write_type), \
    page_data_iovec->total_page_count);

  /* For each IOVEC, Ensure requested page(s) lies within the partition */
  if (FLASH_DEVICE_DONE != flash_nand_check_page_in_partition(client_ctxt,
    page_data_iovec->start_page, page_data_iovec->total_page_count))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  if (FLASH_DEVICE_DONE != flash_nand_check_buff_desc(buffer_desc,
    (int32) page_data_iovec->total_page_count))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  if (FLASH_DEVICE_DONE == result)
  {
    client_data = (struct nand_entry_client_data *)
      client_ctxt->client_data.client_specific_data;

    phys_buf_list = (dalsys_mem_desc_list *) &client_data->nand_phys_buf_list;

    /* Get the DMA friendly memory, if Data mover is present */
    result = flash_get_dma_buffers(client_data, buffer_desc, &phys_buf_list,
      ALLOC_PHYS_MEMDESC_OP_WRITE);

    if (FLASH_DEVICE_DONE == result)
    {
      /* Copy the data from user buffers to physical (DMA friendly) buffers */
      result = flash_copy_dma_buffers ((dalsys_mem_desc_list *)
        phys_buf_list, buffer_desc);

      if (FLASH_DEVICE_DONE == result)
      {
        result = nand_write_pages(client_ctxt, write_type,
          (dalsys_mem_desc_list *)phys_buf_list, page_data_iovec);
      }
    }
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  FLASH_CORE_PROFILE_END((FLASH_PROFILE_WRITE_MAIN + (int)write_type));

  flash_clocks_request_complete();
  /* Leave synchornization */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}

/*
 * This API calls the handle to read page(s) within a partition
 */
int nand_entry_read_pages(flash_client_ctxt *client_ctxt,
  enum page_read_opcode read_type, dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec)
{
  int result = FLASH_DEVICE_DONE;
  struct nand_entry_client_data *client_data;
  dalsys_mem_desc_list *phys_buf_list;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_ops_begin_op();
  flash_clocks_request();

  FLASH_CORE_PROFILE_START((FLASH_PROFILE_READ_MAIN + (int)read_type), \
    page_data_iovec->total_page_count);

  /* For each IOVEC, Ensure requested page(s) lies within the partition */
  if (FLASH_DEVICE_DONE != flash_nand_check_page_in_partition(client_ctxt,
    page_data_iovec->start_page, page_data_iovec->total_page_count))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  if (FLASH_DEVICE_DONE != flash_nand_check_buff_desc(buffer_desc,
    (int32) page_data_iovec->total_page_count))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  if (FLASH_DEVICE_DONE == result)
  {
    client_data = (struct nand_entry_client_data *)
      client_ctxt->client_data.client_specific_data;

    phys_buf_list = (dalsys_mem_desc_list *) &client_data->nand_phys_buf_list;

    /* Get the DMA friendly memory, if Data mover is present */
    result = flash_get_dma_buffers(client_data, buffer_desc, &phys_buf_list,
      ALLOC_PHYS_MEMDESC_OP_READ);

    if (FLASH_DEVICE_DONE == result)
    {
      result = nand_read_pages(client_ctxt, read_type,
        (dalsys_mem_desc_list *)phys_buf_list, page_data_iovec);

      if (FLASH_DEVICE_DONE != flash_copy_dma_buffers(buffer_desc,
        (dalsys_mem_desc_list *) phys_buf_list))
      {
        result = FLASH_DEVICE_FAIL;
      }
    }
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);
  FLASH_CORE_PROFILE_END((FLASH_PROFILE_READ_MAIN + (int)read_type));

  flash_clocks_request_complete();
  /* Leave synchornization */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}

static int nand_entry_get_uuid(flash_client_ctxt *client_ctxt, void *uuid_buf,
  uint32 uuid_buf_len, uint32 *uuid_len)
{
  int result = FLASH_DEVICE_FAIL;

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_ops_begin_op();
  flash_clocks_request();

  result = nand_get_uuid(client_ctxt, uuid_buf, uuid_buf_len, uuid_len);

  flash_clocks_request_complete();
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  return result;
}

/*
 * This API calls the handle to copy page(s) within a partition
 */
static int nand_entry_copy_pages(flash_client_ctxt *client_ctxt,
  struct flash_copy_iovec *copy_iovec, uint32 iovec_cnt)
{
  uint32 tmp_iovec_cnt = 0;
  int result = FLASH_DEVICE_DONE;
  struct flash_copy_iovec *tmp_copy_iovec = copy_iovec;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_ops_begin_op();
  flash_clocks_request();

  /* For each IOVEC, Ensure requested page(s) lies within the partition */
  while(tmp_iovec_cnt < iovec_cnt)
  {
    /* Ensure source page(s) lies within the partition */
    if (FLASH_DEVICE_DONE != flash_nand_check_page_in_partition(client_ctxt,
      tmp_copy_iovec->src_page, tmp_copy_iovec->copy_count))
    {
      result = FLASH_DEVICE_INVALID_PARAMETER;
      break;
    }

    /* Ensure dest page(s) lies within the partition */
    if (FLASH_DEVICE_DONE != flash_nand_check_page_in_partition(client_ctxt,
      tmp_copy_iovec->dest_page, tmp_copy_iovec->copy_count))
    {
      result = FLASH_DEVICE_INVALID_PARAMETER;
      break;
    }

    tmp_copy_iovec++;
    tmp_iovec_cnt++;
  }

  if (FLASH_DEVICE_DONE == result)
  {
    result = nand_copy_pages(client_ctxt, copy_iovec, iovec_cnt);
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  flash_clocks_request_complete();
  /* Leave Synchronization */
  DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  
    /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}


flash_vtbl* nand_get_entry_vtbl_reduced(void)
{
  static flash_vtbl nand_entry_vtbl = {NULL};
  
  nand_entry_vtbl.close = nand_entry_close;
  nand_entry_vtbl.deinit = nand_entry_deinit;
  nand_entry_vtbl.open_partition = nand_entry_open_partition;
  nand_entry_vtbl.get_info = nand_entry_get_info; 
  
  nand_entry_vtbl.block_get_state = nand_entry_block_get_state;
  nand_entry_vtbl.read_pages = nand_entry_read_pages;
  
  return (&nand_entry_vtbl);
}

flash_vtbl* nand_get_entry_vtbl_default(void)
{
  static flash_vtbl nand_entry_vtbl = {NULL};

  nand_entry_vtbl.close = nand_entry_close;
  nand_entry_vtbl.deinit = nand_entry_deinit;
  nand_entry_vtbl.open_partition = nand_entry_open_partition;
  nand_entry_vtbl.get_info = nand_entry_get_info; 
  nand_entry_vtbl.erase_blocks = nand_entry_erase_blocks;
  nand_entry_vtbl.block_set_state = nand_entry_block_set_state;
  nand_entry_vtbl.block_get_state = nand_entry_block_get_state;
  nand_entry_vtbl.write_pages = nand_entry_write_pages;
  nand_entry_vtbl.read_pages = nand_entry_read_pages;
  nand_entry_vtbl.copy_pages = nand_entry_copy_pages;
  nand_entry_vtbl.get_uuid = nand_entry_get_uuid;
  
  return (&nand_entry_vtbl);
}

/*
 * Probe the NAND device
 */
int nand_entry_device_probe(flash_client_ctxt *client_ctxt)
{
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;
  int status = FLASH_DEVICE_NOT_SUPPORTED;
  int init_status = FLASH_DEVICE_FAIL;
  flash_vtbl* nand_entry_vtbl;
  nand_entry_vtbl = nand_get_entry_vtbl();

  init_status = nand_entry_init(client_ctxt);

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
    flash_ops_get_handle((uint32 *)client_ctxt);
    flash_ops_begin_op();
    flash_clocks_request();

    /* Init the babylon */
    FLASH_CORE_PROFILE_INIT();

    dev_ctxt->dev_entry_vtbl = nand_entry_vtbl;

    status = nand_probe(client_ctxt);

    flash_clocks_request_complete();
    DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* Get device's info to initialize stats framework */
    if (FLASH_DEVICE_DONE == status)
    {
      FLASH_CORE_PROFILE_SET_INFO(client_ctxt);
    }
    else
    {
      /* flash failure detected */
      nand_entry_mem_deinit();
    }
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(status);

  return status;
}
