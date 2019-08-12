/*=============================================================================
 *
 * FILE:      flash_nand_init.c
 *
 * DESCRIPTION: This file initializes the flash NAND driver.
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2009-2013, 2015 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_nand_init.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 11/12/15     sb      ECC bit flip check support
 * 10/08/15     sb      Don't free-up runtime_device when it points to listed device
 * 08/12/13     sb      Destroy client handle object to fix memory leak
 * 04/28/13     eo      Support variable length device id check
 * 04/15/13     eo      Fix compiler warnings
 * 09/28/12     sv      Invalidate cache after nand read id
 * 09/05/12     sv      cleanup
 * 07/30/12     sv      Remove 2x mode, smem parti tbl read
 * 07/27/12     sv      Set default mode to ECC M+S enabled
 * 12/14/11     eo      Add optimized transfer steps values
 * 08/26/11     jz      Reduce Ehostdl Image Size
 * 05/20/11     bb      Flash driver Optimization
 * 01/13/11     sb      Fix to configure hardware ECC length
 * 11/30/10     jz      Fixed compiler warning
 * 10/24/10     sb      Temperorary removal of internal copy command
 * 07/20/10     jz      2X support updates, check nand_init_client_info return
 * 07/08/10     jz      Cleanup dual nand controller mode
 * 01/28/10     sb      Bad block optimization
 * 02/08/10     bb      Clean up and moved common code to nand entry layer
 * 02/05/10     sv      Fix to return the correct device info
 * 01/04/10     sv      Retrieve mibib from SMEM in appsbl
 * 11/24/09     jz      Fixed compiler warnings
 * 11/10/09     mh      Fix data abort when nand probe fails
 * 10/14/09     mh      Add support for multiple hal_ids per client
 * 10/13/09     bb      Added Support for optimized copy API
 * 10/02/09     bb      Moved nand_base_vtbl init to probe
 * 10/02/09     eo      Fixed unitialized dev_info.common.device_name
 * 09/18/09     rk      Added 512B row column cycle count calculation
 * 09/15/09     bb      Reverting back Version #5
 * 09/15/09     bb      Temperory version for 7x30 WM7
 * 08/27/09     bb      Warning cleanup for WM7
 * 06/29/09     mh      Set ctlr_info for hal
 * 05/29/09     mh      Reduce long functions with sub functions
 * 03/26/09     dp      Splitted the large flash_nand.c file
 * 03/23/09     eo      Fix UUID extraction for OneNAND
 * 02/23/09     bb      Read/Write API changes with iovec design changes
 * 02/19/09     mh      Clean-up, fix prefast warning
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nand_entry.h"
#include "flash_nand.h"

/*------------------------------------------------------------------------------
  Varibles local to this file.
 ------------------------------------------------------------------------------*/

/* Runtime IDed device characteristics */
static struct flash_nand_params *runtime_device;
static uint8 nand_is_onfi = FALSE;

/*------------------------------------------------------------------------------
  Varibles global in this driver.
 ------------------------------------------------------------------------------*/

/* HAL workspace buffer address */
uint32 hal_ws_buffer = 0;

/* HAL workspace buffer size in bytes */
uint32 hal_ws_buffer_size = 0;

/*------------------------------------------------------------------------------
  Extern variable(s) and function(s).
 ------------------------------------------------------------------------------*/

/* NAND entry init data */
extern struct nand_entry_data nand_entry_init_data;

/* NAND base virtual table */
extern flash_vtbl nand_base_vtbl;

/*------------------------------------------------------------------------------
  Following functions are local to this driver.
 ------------------------------------------------------------------------------*/
/*
 * Check if device IDs is present in device config table.
 */
static boolean nand_device_id_match(struct flash_nand_params *nand_device,
  uint8 *ids)
{
  boolean match_found = TRUE;
  int8    dev_ids_idx = 0;

  /* Check for minimum number of device ID bytes */
  if ((int8)nand_device->dev_ids_num_bytes < 2)
  {
    match_found = FALSE;
  }
  else
  {
    while ((dev_ids_idx < FLASH_MAX_DEVICE_IDS) && 
      (dev_ids_idx < (int8)nand_device->dev_ids_num_bytes))
    {
      if (nand_device->dev_ids[dev_ids_idx] != ids[dev_ids_idx])
      {
        match_found = FALSE;
      }
      dev_ids_idx++;
    }
  }
  return match_found;
}

/*
 * Allocate memory for all driver usage
 */
static int nand_mem_init(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_FAIL;
  struct nand_dev_pdata *nand_pdata;
  struct nand_flash_meminfo *meminfo;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  meminfo = &nand_entry_init_data.meminfo;

  nand_pdata = GET_PDATA(client_ctxt);

  if (TRUE == nand_entry_init_data.nand_mem_initialized)
  {
    /* Initialize HAL Buffers */
    hal_ws_buffer = (uint32) meminfo->hal_mem_info.VirtualAddr;
    hal_ws_buffer_size = meminfo->hal_mem_info.dwLen;

    /* Set up the driver buffers - Temperorly for probing the device.
       These buffers will be configured rightly during nand_configure */
    nand_pdata->raw_page_buffer =
      (uint8 *) meminfo->drv_mem_info.VirtualAddr;

    nand_pdata->drv_ws_buffer =
      (uint32 *) (meminfo->drv_mem_info.VirtualAddr +
      dev_ctxt->dev_info.u.nand.total_page_size_bytes);

    DALSYS_memset(nand_pdata->raw_page_buffer, 0xFF,
      meminfo->drv_mem_info.dwLen);

    result = FLASH_DEVICE_DONE;
  }

  return result;
}

/*
 * Allocate memory for all driver usage
 */
static int nand_mem_reinit(flash_client_ctxt *client_ctxt, uint32 page_size,
  uint32 spare_size)
{
  int result = FLASH_DEVICE_FAIL;
  uint32 hal_id = 0 , hal_generic_mem_size = 0;
  uint32 hal_mem_size = 0, dal_mem_size = 0;

  hal_id = client_ctxt->client_data.hal_id;

  /* Get the memory numbers from HAL */
  flash_hal_get_predef_mem_usage (hal_id,
    nand_entry_init_data.multi_rw_support_count, &hal_mem_size);

  /* Get the HAL memory size from property table */
  hal_generic_mem_size = flash_get_property(FLASH_HAL_WS_BUFF_SIZE_ID);

  /* Include the non-optimized HAL memory requirement */
  hal_mem_size += hal_generic_mem_size;

  dal_mem_size = page_size + spare_size + 64;

  result =  nand_entry_mem_alloc(hal_mem_size, dal_mem_size);

  if (FLASH_DEVICE_DONE == result)
  {
    result = nand_mem_init(client_ctxt);
  }

  return result;
}

/*
 * De-allocate all flash driver allocated memory
 */
static int nand_mem_deinit(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_DONE;
  struct nand_dev_pdata *nand_pdata = GET_PDATA(client_ctxt);

  DALSYS_Free(nand_pdata);
  client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data = 0;

  /* Reset all memory handles and flags */
  hal_ws_buffer = 0;
  hal_ws_buffer_size = 0;

  return result;
}

/*
 * NAND predef initialialization
 */
int nand_predef_init(flash_client_ctxt *client_ctxt)
{
  uint32 used_buff_size;
  int    status = FLASH_DEVICE_FAIL;
  DALSYSMemInfo *hal_mem_info, *drv_mem_info;
  struct flash_hal_predef_init_data predef_init_data;
  enum   flash_hal_status hal_status = FLASH_HAL_SUCCESS;

  hal_mem_info = &nand_entry_init_data.meminfo.hal_mem_info;
  drv_mem_info = &nand_entry_init_data.meminfo.drv_mem_info;

  predef_init_data.num_cs = 1;
  predef_init_data.max_support = 1;
  predef_init_data.buff_data.buff_vaddr = hal_mem_info->VirtualAddr;
  predef_init_data.buff_data.buff_paddr = hal_mem_info->PhysicalAddr;
  predef_init_data.buff_data.buff_size = hal_mem_info->dwLen;

  predef_init_data.raw_buff_data.buff_vaddr = drv_mem_info->VirtualAddr;
  predef_init_data.raw_buff_data.buff_paddr = drv_mem_info->PhysicalAddr;
  predef_init_data.raw_buff_data.buff_size = drv_mem_info->dwLen;
  
  flash_hal_predef_init(client_ctxt->client_data.hal_id,
    &predef_init_data, &used_buff_size, &hal_status);

  if (FLASH_HAL_SUCCESS == hal_status)
  {
    uint32 tmp_ws_buffer = 0;

    /* Re-position HAL Buffers */
    hal_ws_buffer = hal_ws_buffer + used_buff_size;
    hal_ws_buffer_size = hal_ws_buffer_size - used_buff_size;

    /* Align the remaining buffer to 32 byte boundary */
    tmp_ws_buffer = FLASH_ALIGN_32(hal_ws_buffer);

    /* Set the global hal workspace buffer variables accordingly */
    hal_ws_buffer_size -= (tmp_ws_buffer - hal_ws_buffer);
    hal_ws_buffer = tmp_ws_buffer;

    status = FLASH_DEVICE_DONE;
  }

  return status;
}

/*
 * Close a NAND client
 */
int nand_close(flash_client_ctxt *client_ctxt)
{
  int     result = FLASH_DEVICE_DONE;
  struct nand_entry_client_data *client_data_handle;

  if (client_ctxt->client_data.client_specific_data)
  {
    client_data_handle = (struct nand_entry_client_data *)
        client_ctxt->client_data.client_specific_data;
    DALSYS_DestroyObject(client_data_handle->nand_phys_handle);

    /* Free any malloced extra data for client */
    result = DALSYS_Free(client_ctxt->client_data.client_specific_data);
    client_ctxt->client_data.client_specific_data = 0;
    FLASHLOG(3, ("Freed client extra data in nand_close\n"));
  }

  return result;
}

/*
 * Configure the hal_device_info fileds of the specific hal_id
 */
static int nand_configure_hal_device_info(struct flash_nand_params  *devices,
  uint32 hal_id)
{
  enum flash_hal_ecc_length nand_ecc_len;
  /* Set default HAL settings */
  flash_hal_set_page_size(hal_id, (enum flash_hal_page_size)
    devices->page_size);
  flash_hal_set_spare_size(hal_id, (enum flash_hal_spare_size)
    devices->spare_size);
  flash_hal_set_dev_width(hal_id, (enum flash_hal_dev_width)
    devices->dev_width);

  flash_hal_set_ecc_state(hal_id, FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE);

  if ((uint32)devices->feature_flags1.ecc <= FLASH_HAL_HW_ECC_4BIT)
  {
    /* Found a match. Configure */
    nand_ecc_len = FLASH_HAL_HW_ECC_4BIT;
  }
  else if ((uint32)devices->feature_flags1.ecc <= FLASH_HAL_HW_ECC_8BIT)
  {
    /* Found a match. Configure */
    nand_ecc_len = FLASH_HAL_HW_ECC_8BIT;
  }
  else
  {
    FLASHLOG(3, ("Device needs more ECC bitlength than supported\n"));
    return FLASH_DEVICE_NOT_SUPPORTED;
  }

  flash_hal_set_ecc_length(hal_id, nand_ecc_len);
  FLASHLOG(3, ("Configured %d bit ECC length\n", nand_ecc_len));
	  
  return FLASH_DEVICE_DONE;
}

/*
 * Initilializes the client information
 */

static int nand_init_client_info(flash_client_ctxt *client_ctxt)
{
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;
  struct nand_dev_pdata *nand_pdata = GET_PDATA(client_ctxt);
  struct nand_client_specific_data *nand_client_data;
  int status = FLASH_DEVICE_DONE;
  uint32 block_size;

  client_ctxt->client_data.pages_per_block =
    dev_ctxt->dev_info.u.nand.pages_per_block;
  client_ctxt->client_data.page_size_bytes =
    dev_ctxt->dev_info.u.nand.page_size_bytes;
  client_ctxt->client_data.total_page_size_bytes =
    dev_ctxt->dev_info.u.nand.total_page_size_bytes;

  nand_pdata->lp_to_pp_lookup = NULL;

  /* Set number of address cycles for the device */
  flash_hal_set_address_cycles(client_ctxt->client_data.hal_id,
    nand_pdata->row_count, nand_pdata->col_count);

  /* Set number of clock cycle gap for the device */
  flash_hal_set_clk_cycle_gap(client_ctxt->client_data.hal_id,
    flash_get_property(FLASH_EBI2_CLOCK_ID), runtime_device->max_wb_ns);

  block_size = client_ctxt->client_data.pages_per_block;
  client_ctxt->client_data.block_size_shift  = 1;

  while (2 != block_size)
  {
    block_size >>= 1;
    ++client_ctxt->client_data.block_size_shift;
  }

  client_ctxt->client_data.max_spare_udata_bytes = 0;
	
  /* Set the hardware ECC for this client */
  switch (client_ctxt->client_data.ecc_state)
  {
    case FLASH_ECC_MAIN_SPARE_ENABLED:
      {
        struct flash_hal_page_layout *page_layout;
        const struct flash_hal_page_unit_layout *page_unit_layout;
        uint32 index;

        flash_hal_set_ecc_state (client_ctxt->client_data.hal_id,
          FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE);
        flash_hal_get_page_layout(client_ctxt->client_data.hal_id,
          &page_layout);
        page_unit_layout = page_layout->unit_layout;

        /* Read the codeword with udata and copy only the udata */
        index=0;

        while (index < page_layout->page_unit_count)
        {
          client_ctxt->client_data.max_spare_udata_bytes +=
            page_unit_layout->udata_len_bytes * page_unit_layout->num_code_word;
          index++ ;
          page_unit_layout++ ;
        }
      }
      break;
  case FLASH_ECC_DISABLED:
    flash_hal_set_ecc_state (client_ctxt->client_data.hal_id,
      FLASH_HAL_HW_ECC_DISABLE);
    break;
  default:
    client_ctxt->client_data.ecc_state = FLASH_ECC_MAIN_SPARE_ENABLED;
    flash_hal_set_ecc_state (client_ctxt->client_data.hal_id,
      FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE);
    break;
  }

  if (client_ctxt->client_data.client_specific_data == 0)
  {
    /* If there is a need to provide a client specific data to each
       client of flash driver, we could define+malloc a 'struct
       nand_client_specific_data' and map them to
       client_ctxt->client_data.client_specific_data here */
    if (DAL_SUCCESS != DALSYS_Malloc(sizeof(struct nand_client_specific_data),
      (void * ) &nand_client_data))
    {
      FLASHLOG(3, ("\nMalloc failed: No memory for client specific data!\n"));
      status = FLASH_DEVICE_FAIL;
      return status;
    }

    /* Clear the memory allocated for client specific data */
    DALSYS_memset((void *) nand_client_data, 0x0,
      sizeof(struct nand_client_specific_data));

    client_ctxt->client_data.client_specific_data = (void *) nand_client_data;
  }

  /* Set Client Ctxt default Vtbl to the Base Vtbl in Device Ctxt */

  return status;
}

/*
 * De-initializes the NAND driver
 */
int nand_deinit (flash_client_ctxt *client_ctxt)
{
  int     result = FLASH_DEVICE_DONE;

  flash_hal_dev_deinit(client_ctxt->client_data.hal_id);
  
  /* Free any malloced extra data for client */
  nand_mem_deinit(client_ctxt);

  /* Clear the HAL ID from the client_ctxt */
  client_ctxt->client_data.hal_id = FLASH_HAL_INVALID_DEVICE_ID;

  /* Release any allocated device private data */
  if (client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data)
  {
    DALSYS_Free(
      client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data);
    client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data = NULL;
  }

  if (client_ctxt->flash_dev_ctxt->parti_ptr)
  {
    /* Free any malloced extra data for client */
    DALSYS_Free(client_ctxt->flash_dev_ctxt->parti_ptr);
    client_ctxt->flash_dev_ctxt->parti_ptr = NULL;
  }

  if (runtime_device && nand_is_onfi)
  {
    DALSYS_Free(runtime_device);
    runtime_device = NULL;
    nand_is_onfi = FALSE;
  }

  FLASHLOG(3, ("Freed any device specific data (shared across clients)\n"));

  return result;
}

/*
 * Open partition for a nand client and init client info
 */
int nand_open_partition(flash_client_ctxt *client_ctxt,
  const unsigned char *partition_name)
{
  int    status = FLASH_DEVICE_DONE;

  FLASHLOG(4, ("nand_open_partition called\n"));

  status = nand_init_client_info(client_ctxt);

  return status;
}

/*
 * Configure the device ctxt fields
 */
static int nand_configure_device_ctxt(flash_client_ctxt *client_ctxt,
  struct flash_nand_params *device)
{
  flash_dev_ctxt        *dev_ctxt = client_ctxt->flash_dev_ctxt;
  int result = FLASH_DEVICE_DONE;

  /* Allocate private data shared across clients */
  if (dev_ctxt->dev_info.common.dev_specific_data == NULL)
  {
    if (DAL_SUCCESS != DALSYS_Malloc(sizeof(struct nand_dev_pdata),
      (void ** )&dev_ctxt->dev_info.common.dev_specific_data))
    {
      FLASHLOG(3, ("\nMalloc failed\n"));
      result = FLASH_DEVICE_FAIL;
      return result;
    }
  }

  /* Initialize one time device context Information */
  dev_ctxt->dev_info.common.device_name    = device->device_name;
  dev_ctxt->dev_info.common.type  = FLASH_NAND;
  dev_ctxt->dev_info.common.tech = (device->feature_flags1.pagepairing ==
    FLASH_NO_PAGE_PAIRING_RESTRICTIONS) ? FLASH_SLC : FLASH_MLC;
  dev_ctxt->dev_info.common.width = (enum flash_width)device->dev_width;
  dev_ctxt->dev_info.common.write_style  = FLASH_WRITES_PAGED;

  dev_ctxt->dev_info.u.nand.maker_id    = device->dev_ids[0];
  dev_ctxt->dev_info.u.nand.device_id   = device->dev_ids[1];
  dev_ctxt->dev_info.u.nand.version_id  = device->dev_ids[2];
  dev_ctxt->dev_info.u.nand.block_count = device->block_count;

  dev_ctxt->dev_info.u.nand.pages_per_block = device->pages_per_block;

  dev_ctxt->dev_info.u.nand.page_size_bytes = device->page_size;
  dev_ctxt->dev_info.u.nand.total_page_size_bytes = device->page_size +
    device->spare_size;
  dev_ctxt->dev_info.u.nand.flags1  = *((uint32 *)&device->feature_flags1);
  dev_ctxt->dev_info.u.nand.flags2  = *((uint32 *)&device->feature_flags2);

  return result;
}

/*
 * Configure the pdata fields
 */
static int nand_configure_pdata(flash_client_ctxt *client_ctxt,
  struct flash_nand_params *device)
{
  struct nand_dev_pdata *nand_pdata;
  struct nand_flash_meminfo *flash_mem_data;
  flash_dev_ctxt        *dev_ctxt = client_ctxt->flash_dev_ctxt;
  int result = FLASH_DEVICE_DONE;
  uint32 block_kbytes, device_mbytes;
  uint32 page_size;

  page_size = dev_ctxt->dev_info.u.nand.page_size_bytes;

  nand_pdata = GET_PDATA(client_ctxt);
  nand_pdata->config_data = device;

  /* Set the device status values */
  nand_pdata->op_status_mask = device->op_status_mask;
  nand_pdata->op_status_value = device->op_status_value;

  nand_pdata->flash_mem_info = &nand_entry_init_data.meminfo;

  flash_mem_data = GET_MEM_INFO(client_ctxt);

  /* Set up the driver buffers */
  nand_pdata->raw_page_buffer =
    (uint8 *) flash_mem_data->drv_mem_info.VirtualAddr;

  nand_pdata->drv_ws_buffer =
    (uint32 *) (flash_mem_data->drv_mem_info.VirtualAddr +
    dev_ctxt->dev_info.u.nand.total_page_size_bytes);

  block_kbytes = (device->page_size * device->pages_per_block) >> 10;
  device_mbytes = (block_kbytes * device->block_count) >> 10;

  /* Compute Row and & Column byte address counts */
  switch (page_size)
  {
    case 2048:
    case 4096:
    case 8192:
      {
        if (device_mbytes >= 256)
        {
          /* 2KB page device with size >= 2Gb */
          nand_pdata->row_count = 3;
        }
        else
        {
          /* 2KB page device with size < 2Gb */
          nand_pdata->row_count = 2;
        }

        /* Since 2KB page, column address byte count is 2*/
        nand_pdata->col_count = 2;

        FLASHLOG (3, ("Denisty: %d MB\n", device_mbytes));
        FLASHLOG (3, ("Row Address Count : %d\n", nand_pdata->row_count));
        FLASHLOG (3, ("Col Address Count : %d\n", nand_pdata->col_count));
      }
      break;
    default:
      result = FLASH_DEVICE_FAIL;
      FLASHLOG (3, ("Unsupported page size\n"));
      return result;
  }

  return result;
}

/*
 * Configure the client and device context info.
 */
static int nand_configure(flash_client_ctxt *client_ctxt, struct flash_nand_params
  *device)
{
  int result = FLASH_DEVICE_DONE;

  if (FLASH_DEVICE_DONE != nand_configure_device_ctxt(client_ctxt, device))
  {
    result = FLASH_DEVICE_FAIL;
    return result;
  }

  if (FLASH_DEVICE_DONE != nand_configure_pdata(client_ctxt, device))
  {
    result = FLASH_DEVICE_FAIL;
    return result;
  }

  if (FLASH_DEVICE_DONE != nand_configure_hal_device_info(device,
    client_ctxt->client_data.hal_id))
  {
    result = FLASH_DEVICE_FAIL;
    return result;
  }

  client_ctxt->client_data.ecc_state = FLASH_ECC_MAIN_SPARE_ENABLED;

  if (FLASH_HAL_8_BIT_WIDTH == device->dev_width)
  {
    FLASHLOG (3, ("x8 NAND Device\n"));
  }
  else
  {
    FLASHLOG (3, ("x16 NAND Device\n"));
  }

  return result;
}

/*
 * Get the list of nand devices support tied to a given chip select
 */
static int nand_get_device_list_supported(struct flash_hal_dev_config *nand_cfg,
  enum flash_hal_status  *hal_status)
{
  int cfg_count;
  struct flash_nand_cfg_data *nand_cfgs;

  flash_nand_get_configs(&nand_cfgs);

  /* Search the BSP to see if it has any NAND device configured to be
     probed */
  for (cfg_count = 0; (nand_cfgs[cfg_count].dev_type !=
       FLASH_HAL_DEVICE_NONE); cfg_count++ )
  {
    /* Find and initialize the hal device for the NAND devices wired to
     * the configured chip select
     */
    if (FLASH_HAL_DEVICE_NAND == nand_cfgs[cfg_count].dev_type)
    {
      /* Basic configuration for flash HAL client init */
      nand_cfg->dev_type = FLASH_HAL_DEVICE_NAND;
      nand_cfg->chip_sel = nand_cfgs[cfg_count].chip_sel;
      break;
    }/* if (FLASH_HAL_DEVICE_NAND == nand_cfgs[cfg_count].dev_type) */
  }/* for (cfg_count = 0; (nand_cfgs[cfg_count].dev_type != */

  if (nand_cfgs[cfg_count].dev_type == FLASH_HAL_DEVICE_NONE)
  {
    FLASHLOG(3, ("\nNo NAND device in Config Table\n"));
    cfg_count = -1;
  }

  return cfg_count;
}

/*  Checks if the IDs read off the NAND device is supported in the list
    of device supported in the supported_nand_devices array
*/
static boolean nand_check_if_enlisted_device(struct flash_nand_params **device,
  uint32 num_nands, uint8 *ids )
{
  boolean result = FALSE;
  struct flash_nand_params *nand_device = *device;

  /* Identify the device from the ones enlisted in supported_nand_devices */
  for (; num_nands != 0; --num_nands, ++nand_device)
  {
    FLASHLOG(4, ("\nIDs in Config Table - ID0 : 0x%x ID1 :0x%x\n",
      nand_device->dev_ids[0], nand_device->dev_ids[1]));

    if (TRUE == nand_device_id_match(nand_device, ids))
    {
      result = TRUE;

      *device = nand_device;

      /* Found a match. Exit */
      FLASHLOG(3, ("\nFound device in configuration table\n"));
      break;
    }
  }
  return result;
}

/*  Checks if the IDs read off the NAND device is supported in the list
    of vendor specific device supported by decoding the IDs
*/
static boolean nand_check_if_supported_vendor_device(flash_client_ctxt *client_ctxt,
   struct flash_nand_params  **devices, uint8 *ids)
{
  int status = FLASH_DEVICE_FAIL;

  status = nand_decode_ids(client_ctxt, devices, ids);

  if (FLASH_DEVICE_DONE == status)
  {
    FLASHLOG(3, ("\nRuntime NAND device configured\n"));
    FLASHLOG(3, ("\nFound device in configuration table\n"));
    return TRUE;
  }

  return FALSE;
}

/*  Checks if the IDs read off the NAND device is an ONFI
    devide ID
*/
static uint8 nand_check_if_onfi_device (flash_client_ctxt *client_ctxt,
   struct flash_nand_params  **devices)
{
  int result;

  result = nand_onfi_probe(client_ctxt, devices, &nand_is_onfi);

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(3, ("\nONFI Probe failed\n"));
    nand_is_onfi = FALSE;
  }
  else
  {
    if (nand_is_onfi)
    {
      FLASHLOG(3, ("\nONFI NAND device detected\n"));
    }
  }
  return nand_is_onfi;
}

/*
 * Check if device ID is supported by the driver and return the device
 * parameters
 */
static struct flash_nand_params* nand_get_device_nand_params(
  flash_client_ctxt *client_ctxt,  uint8 * ids,
  struct flash_nand_cfg_data *nand_cfgs, int cfg_count )
{
  uint32 num_nands;
  struct flash_nand_params  *device;

  device = nand_cfgs[cfg_count].dev_params;
  num_nands = nand_cfgs[cfg_count].num_devices;

  /* Check if the device IDs are in the supported_nand_device list */
  if (FALSE == nand_check_if_enlisted_device(&device, num_nands, ids))
  {
     /* if not then check if the device ID can be decoded to match either
        samsung or toshiba  vendors*/
     if (FALSE == nand_check_if_supported_vendor_device(client_ctxt,
          &device, ids))
     {
       /* if not then check if the device is an onfi device  */
       if (FALSE == nand_check_if_onfi_device (client_ctxt, &device))
       {
          FLASHLOG(3, ("NAND Device not configured for this target\n"));
          device = NULL;
       }
     }
  }

  return device;
}

/*
 * Probe the NAND device on CS0 and nand ctlr 0 and do all
 * one time initialization for NAND driver
 */
static int nand_intialize_primary_hal_device(flash_client_ctxt *client_ctxt,
   uint8** ids)
{
  int     result = FLASH_DEVICE_DONE, cfg_count;
  enum flash_hal_status hal_status = FLASH_HAL_INVALID_DEVICE;
  struct flash_hal_dev_config nand_cfg;
  struct flash_nand_cfg_data *nand_cfgs;
  struct flash_hal_buffer_desc buff_desc;
  struct nand_dev_pdata *nand_pdata = GET_PDATA(client_ctxt);
  uint32 hal_id = FLASH_HAL_INVALID_DEVICE_ID;

  FLASHLOG(3, ("\nProbing for NAND device ...\n"));

  /* Search the BSP to see if it has any NAND device configured to be
     probed on the first configured chip select */
  cfg_count = nand_get_device_list_supported(&nand_cfg, &hal_status);

  /* Check if we are able to find list of device supported */
  if (-1 == cfg_count)
  {
    FLASHLOG(3, ("\nNo NAND device in Config Table\n"));
    result = FLASH_DEVICE_NOT_SUPPORTED;
    return result;
  }

  /* Initialize Flash HAL Client */
  flash_hal_dev_init(&nand_cfg, &hal_id, &hal_status);

  if (FLASH_HAL_SUCCESS != hal_status)
  {
    FLASHLOG(3, ("\nFailed to initialize flash HAL client\n"));
    result = FLASH_DEVICE_NOT_SUPPORTED;
    return result;
  }

  client_ctxt->client_data.which_cs = nand_cfg.chip_sel;
  client_ctxt->client_data.hal_id = hal_id;

  /* Initialize predefined HAL APIs for probe - Assume 2K device */
  flash_hal_set_page_size(hal_id, FLASH_HAL_2048B_PAGE);

  if (FLASH_DEVICE_DONE != nand_predef_init(client_ctxt))
  {
    FLASHLOG(3, ("\nFlash HAL predefined initialization failed!\n"));
    result = FLASH_DEVICE_FAIL;
    goto exitfunction;
  }

  *ids = (uint8 *) nand_pdata->raw_page_buffer;

  buff_desc.buff_vaddr = (uint32) nand_pdata->raw_page_buffer;
  buff_desc.buff_paddr = flash_vtop_get_physical(buff_desc.buff_vaddr);
  buff_desc.buff_size = 4;
  buff_desc.num_pages = 0;

  flash_dcache_inval(buff_desc.buff_vaddr, 4);

  /* Issue Device Reset and retrieve Device ID */
  if (FLASH_DEVICE_DONE != nand_read_id(hal_id, &buff_desc))
  {
    result = FLASH_DEVICE_FAIL;
    goto exitfunction;
  }
  
  flash_dcache_inval(buff_desc.buff_vaddr, 4);
  
  FLASHLOG(3, ("\nIDs from Device - ID0 : 0x%x ID1 : 0x%x\n", *((uint8*)*ids),
    *((uint8*)*ids+1)));

  flash_nand_get_configs(&nand_cfgs);

  runtime_device = nand_get_device_nand_params(client_ctxt, *ids, nand_cfgs,
    cfg_count);

  if (NULL == runtime_device)
  {
    FLASHLOG(3, ("\nDevice ID not supported by Driver!\n"));
    result = FLASH_DEVICE_FAIL;
    goto exitfunction;
  }

  client_ctxt->client_data.which_cs = nand_cfg.chip_sel;

exitfunction:
  if (FLASH_DEVICE_DONE != result)
  {
    if (FLASH_HAL_INVALID_DEVICE_ID != hal_id)
    {
      /* If alloted, release primary dev_id/hal_id */
      flash_hal_dev_deinit(hal_id);
      client_ctxt->client_data.hal_id = FLASH_HAL_INVALID_DEVICE_ID;
      nand_mem_deinit(client_ctxt);
    }
  }

  return result;
}

/*
 * Probe the NAND device and do all one time initialization for NAND driver
 */
int nand_probe(flash_client_ctxt *client_ctxt)
{
  int     result = FLASH_DEVICE_NOT_SUPPORTED;
  uint8   *primary_device_ids = NULL;
  struct nand_dev_pdata *nand_pdata = NULL;

 /* Following is a summary of the probe sequence followed by a diagram
    of the flash_client data structure for multi nand ctlrs

 Probe Sequence:
 --------------
  - Have Nand ctrl 0 probe CS0 for any device
     -- if device not found, return FAIL
     -- if found a device, do all one time initialization for NAND driver
     -- Initlaize runtime_devices.
     -- hal_id used and alloted for this device will be the primary hal_id

  *********************************************************************************/

  /* Allocate private data shared across clients */
  if (DAL_SUCCESS != DALSYS_Malloc(sizeof(struct nand_dev_pdata),
    (void ** )&nand_pdata))
  {
    FLASHLOG(3, ("\nMalloc failed\n"));
    result = FLASH_DEVICE_FAIL;
    return result;
  }
  client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data = nand_pdata;

  /* Initialize Memory */
  if (FLASH_DEVICE_DONE != nand_mem_init(client_ctxt))
  {
    FLASHLOG(3, ("\nFlash Memory initialization failed!\n"));
    result = FLASH_DEVICE_NO_MEM;
    return result;
  }

 /* The primary device hal_id is used for nand ctlr 0  using chip select 0
   * We MUST  find a device on CS0 wired to nand ctlr 0  */
  result = nand_intialize_primary_hal_device(client_ctxt, &primary_device_ids);
  if (FLASH_DEVICE_DONE != result)
  {
    result = FLASH_DEVICE_FAIL;
    goto exitfunction;
  }

  /* No need to call nand_mem_reinit() if DMA is disabled */
  if ((FLASH_DMA_ENABLED) && (FLASH_DEVICE_DONE == result))
  {
    result = nand_mem_reinit(client_ctxt, runtime_device->page_size,
      runtime_device->spare_size);
  }

  if (FLASH_DEVICE_DONE == result)
  {
    result = nand_configure(client_ctxt, runtime_device);
  }

  if (FLASH_DEVICE_DONE == result)
  {
    result = nand_init_client_info(client_ctxt);

    /* Re-initialize predefined HAL APIs */
    if (FLASH_DEVICE_DONE != nand_predef_init(client_ctxt))
    {
      FLASHLOG(3, ("\nFlash HAL predefined initialization failed!\n"));
      result = FLASH_DEVICE_NO_MEM;
      goto exitfunction;
    }

    if (FLASH_DEVICE_DONE == result)
    {
        result = nand_retrieve_mibib(client_ctxt);
    }
  }

  /* Set the client context in the flash debug structure */
  SET_FLASH_DBG_CLIENT_CTX(client_ctxt);

exitfunction:

  if (FLASH_DEVICE_DONE != result)
  {
    /* If alloted, release primary dev_id/hal_id */
    if (FLASH_HAL_INVALID_DEVICE_ID != client_ctxt->client_data.hal_id)
    {
      flash_hal_dev_deinit(client_ctxt->client_data.hal_id);
      client_ctxt->client_data.hal_id = FLASH_HAL_INVALID_DEVICE_ID;

      nand_mem_deinit(client_ctxt);
    }
  }

  return result;
}
