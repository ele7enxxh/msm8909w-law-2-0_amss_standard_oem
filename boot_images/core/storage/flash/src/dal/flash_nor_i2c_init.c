/*=============================================================================
 *
 * FILE:      flash_nor_i2c_init.c
 *
 * DESCRIPTION: This file initializes the Flash NOR I2C EEPROM core driver.
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010-2011, 2013-2014 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_nor_i2c_init.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ---------   ---    ----------------------------------------------
 * 03/18/14     whc     Add different i2c clk support
 * 03/18/14     whc     Add OEM custom EEPROM support
 * 12/17/13     whc     Add RF EEPROM I2C support
 * 05/20/11     sv      Flash driver Optimization
 * 10/28/10     bn      Changed Init function to return pass/fail result
 * 10/11/10     bn      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nor_i2c.h"

/*-----------------------------------------------------------------------------
  Varibles global in this driver.
 ----------------------------------------------------------------------------*/
/* NOR I2C wrapper function pointers */
struct nor_i2c_wrapper nor_i2c_ops;

/*------------------------------------------------------------------------------
  Extern variable(s) and function(s).
 ------------------------------------------------------------------------------*/
/* I2C NOR base virtual table */
extern flash_vtbl nor_i2c_base_vtbl;

/* 
 *  Flash NOR I2C initialization
 */
static int nor_i2c_init(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_FAIL;
  flash_i2c_config_tbl *i2c_tbl;
  struct nor_i2c_dev_pdata *nor_i2c_pdata;

  nor_i2c_pdata = GET_NOR_PDATA(client_ctxt);
  
  i2c_tbl = (flash_i2c_config_tbl *)flash_get_property(
    FLASH_I2C_CONFIG_TABLE_ID);

  if (FLASH_PROPERTY_NOT_DEFINED != (uint32)i2c_tbl)
  {
    if (i2c_tbl && i2c_tbl->i2c_config)
    {
      i2c_tbl->i2c_config(&nor_i2c_ops);
      result = nor_i2c_ops.init(nor_i2c_pdata->config_data->base_address, nor_i2c_pdata->config_data->clk );

      if (FLASH_EXTDRV_I2C_DONE == result)
      {
        result = FLASH_DEVICE_DONE;
      }
      else
      {
        result = FLASH_DEVICE_FAIL;
      }
    }
  }

  return result;
}

/*
 * Wrapper function for flash I2C read operation
 */
 int nor_i2c_read_op(struct nor_i2c_xfer_buff_info *i2c_read_buf_info)
{
  if (FLASH_EXTDRV_I2C_DONE == nor_i2c_ops.read_op(i2c_read_buf_info))
  {
    return FLASH_DEVICE_DONE;
  }
  else
  {
    return FLASH_DEVICE_FAIL;  
  }
}

/* 
 * Wrapper function for flash I2C write operation
 */
int nor_i2c_write_op(struct nor_i2c_xfer_buff_info *i2c_write_buf_info)
{
  if (FLASH_EXTDRV_I2C_DONE == nor_i2c_ops.write_op(i2c_write_buf_info))
  {
    return FLASH_DEVICE_DONE;
  }
  else
  {
    return FLASH_DEVICE_FAIL;  
  }
}

/*
 * De-allocate all flash driver allocated memory
 */
static int nor_i2c_mem_deinit(flash_client_ctxt *client_ctxt)
{
  struct nor_i2c_dev_pdata *nor_i2c_pdata = GET_NOR_PDATA(client_ctxt);

  if (nor_i2c_pdata)
  {
    DALSYS_Free(nor_i2c_pdata);
  }
  
  client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data = 0;

  return FLASH_DEVICE_DONE;
}

/*
 * Close a NOR client
 */
static int nor_i2c_close(flash_client_ctxt *client_ctxt)
{
  return FLASH_DEVICE_DONE;
}

/*
 * De-initializes the NOR I2C driver
 */
static int nor_i2c_deinit (flash_client_ctxt *client_ctxt)
{  
  /* Free any malloced extra data for client */
  nor_i2c_mem_deinit(client_ctxt);

  FLASHLOG(3, ("Freed any device specific data (shared across clients)\n"));

  return FLASH_DEVICE_DONE;
}

/*
 * Initilializes the client information
 */
int nor_i2c_init_client_info(flash_client_ctxt *client_ctxt)
{
  uint32 block_size, page_size;  
  struct nor_dev_data *dev_data;
  struct flash_client_data *client_data = &client_ctxt->client_data;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  dev_data = &dev_ctxt->dev_info.u.nor;

  /* Init Client context */
  block_size = dev_data->pages_per_block;
  page_size = dev_data->page_size_bytes;

  /* Init Client context */
  client_data->block_count = dev_data->block_count;
  client_data->pages_per_block = dev_data->pages_per_block;
  client_data->page_size_bytes = dev_data->page_size_bytes;
  client_data->total_page_size_bytes = dev_data->page_size_bytes;
  client_data->base_address = dev_data->base_address;

  /* There is no partition support. We open the entire device now */
  client_data->partition_start_addr = 0;
  client_data->partition_limit_addr = dev_data->block_count;
  client_data->partition_attributes = 0;
  client_data->partition_block_start = 0;
  client_data->partition_block_limit = dev_data->block_count;
  client_data->partition_block_count = dev_data->block_count;
  client_data->partition_block_size_bytes = dev_data->page_size_bytes *
    dev_data->pages_per_block;

  /* Find the nth bit set to determine number of block size shifts */
  while (!(block_size & 0x1))
  {
    block_size >>= 1;
    ++client_data->block_size_shift;
  }

  client_data->page_size_shift  = 0;
  /* Find the nth bit set to determine number of page size shifts */
  while (!(page_size & 0x1))
  {
    page_size >>= 1;
    ++client_data->page_size_shift;
  }

  dev_ctxt->dev_base_vtbl = &nor_i2c_base_vtbl;

  /* Set Client Ctxt default Vtbl to the Base Vtbl in Device Ctxt */
  client_ctxt->device_type_vtbl = dev_ctxt->dev_base_vtbl;

  return FLASH_DEVICE_DONE;
}

/*
 * Open partition for a NOR I2C client
 */
static int nor_i2c_open_partition(flash_client_ctxt *client_ctxt,
  const unsigned char *partition_name)
{
  FLASHLOG(4, ("nor_i2c_open_partition called\n"));

  return nor_i2c_init_client_info(client_ctxt);
}

/*
 * Configure the device ctxt fields
 */
static int nor_i2c_configure_device_ctxt(flash_client_ctxt *client_ctxt,
  struct flash_nor_params *devices)
{
  int status = FLASH_DEVICE_DONE;
  uint32 unit_size;
  struct nor_dev_data *dev_data;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  /* Initialize one time device context Information */
  dev_ctxt->dev_base_vtbl = &nor_i2c_base_vtbl;
  dev_data = &dev_ctxt->dev_info.u.nor;

  dev_ctxt->dev_info.common.device_name = 
    (const char *)&devices->device_name[0];
  dev_ctxt->dev_info.common.type = devices->type;
  dev_ctxt->dev_info.common.tech = devices->tech;
  dev_ctxt->dev_info.common.width = (enum flash_width)devices->width;
  dev_ctxt->dev_info.common.write_style = devices->write_style;

  /* Copy the device name */
  flash_strcpy((uint8 *) dev_data->device_name,
   (uint8 *)&devices->device_name[0], 32);

  dev_data->num_ids = devices->num_ids;
  dev_data->dev_ids[0] = devices->dev_ids[0];
  dev_data->dev_ids[1] = devices->dev_ids[1];
  dev_data->dev_ids[2] = devices->dev_ids[2];
  dev_data->dev_ids[3] = devices->dev_ids[3];
  dev_data->family = devices->family;
  dev_data->block_count = devices->block_count;
  dev_data->pages_per_block = devices->pages_per_block;
  dev_data->page_size_bytes = devices->page_size_bytes;
  dev_data->device_size_MB = devices->device_size_MB;
  dev_data->wbuffer_size_bytes = devices->wbuffer_size_bytes;
  dev_data->flags1 = devices->flags1;
  dev_data->clk = devices->clk;
  dev_data->base_address = devices->base_address;

  unit_size = dev_data->page_size_bytes;
  
  /* Find the nth bit set to determine number of page size shifts */
  while (!(unit_size & 0x1))
  {
    unit_size >>= 1;
    ++client_ctxt->client_data.page_size_shift;
  }

  unit_size = dev_data->pages_per_block;
  
  /* Find the nth bit set to determine number of block size shifts */
  while (!(unit_size & 0x1))
  {
    unit_size >>= 1;
    ++client_ctxt->client_data.block_size_shift;
  }

  /* Initialize client info */
  status = nor_i2c_init_client_info(client_ctxt);

  return status;
}

/*
 * Configure the pdata fields
 */
static int nor_i2c_configure_pdata(flash_client_ctxt *client_ctxt,
  struct flash_nor_params *devices)
{
  struct nor_i2c_dev_pdata *nor_i2c_pdata;

  nor_i2c_pdata = GET_NOR_PDATA(client_ctxt);
  nor_i2c_pdata->config_data = devices;

  return FLASH_DEVICE_DONE;
}

/*
 * Configure the client and device context info.
 */
static int nor_i2c_configure(flash_client_ctxt *client_ctxt, 
  struct flash_nor_params *devices)
{
  int result = FLASH_DEVICE_DONE;
  struct nor_i2c_dev_pdata *nor_i2c_pdata;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  /* Allocate private data shared across clients */
  if (dev_ctxt->dev_info.common.dev_specific_data == NULL)
  {
    if (DAL_SUCCESS != DALSYS_Malloc(sizeof(struct nor_i2c_dev_pdata),
      (void ** )&dev_ctxt->dev_info.common.dev_specific_data))
    {
      FLASHLOG(3, ("\nMalloc failed\n"));
      return FLASH_DEVICE_FAIL;
    }
  }

  /* Set default device vtbl so that nor_i2c_entry can redirect
     to the open_partition API */
  nor_i2c_base_vtbl.close          = nor_i2c_close;
  nor_i2c_base_vtbl.deinit         = nor_i2c_deinit;
  nor_i2c_base_vtbl.open_partition = nor_i2c_open_partition;
  dev_ctxt->dev_base_vtbl          = &nor_i2c_base_vtbl;

  nor_i2c_pdata = GET_NOR_PDATA(client_ctxt);
  nor_i2c_pdata->config_data = devices;

  /* Initialize one time device context Information */
  result = nor_i2c_configure_device_ctxt(client_ctxt, devices);

  if (FLASH_DEVICE_DONE != nor_i2c_configure_pdata(client_ctxt, devices))
  {
    result = FLASH_DEVICE_FAIL;
  }

  return result;
}

/*
 * Probe the I2C device and do all one time initialization for I2C flash driver
 */
int nor_i2c_probe(flash_client_ctxt *client_ctxt)
{
  int result = FLASH_DEVICE_NOT_SUPPORTED;
  uint32 cfg_count = 0;
  struct nor_i2c_dev_pdata *nor_i2c_pdata;
  struct flash_nor_cfg_data *nor_cfgs;
  struct nor_i2c_probe_cfgs i2c_probe_cfg;
  struct flash_nor_params *devices = NULL;
  flash_dev_ctxt *dev_ctxt = client_ctxt->flash_dev_ctxt;

  nor_i2c_pdata = GET_NOR_PDATA(client_ctxt);
  i2c_probe_cfg.dev_type = FLASH_UNKNOWN;

  /* Allocate private data shared across clients */
  if (nor_i2c_pdata == NULL)
  {
    if (DAL_SUCCESS != DALSYS_Malloc(sizeof(struct nor_i2c_dev_pdata),
      (void ** )&dev_ctxt->dev_info.common.dev_specific_data))
    {
      FLASHLOG(3, ("\nMalloc failed\n"));
      return FLASH_DEVICE_FAIL;
    }
    nor_i2c_pdata = GET_NOR_PDATA(client_ctxt);
  }

  /* Get the NOR configurations */
  flash_nor_i2c_get_configs(&nor_cfgs);

  /* Search the BSP to see if it has any NOR device configured to be probed */
  for (cfg_count = 0; (FLASH_UNKNOWN != nor_cfgs[cfg_count].dev_type); cfg_count++)
  {
    if ((dev_ctxt->dev_id == DALDEVICEID_FLASH_DEVICE_2) && (cfg_count == 0))
    {
      if (FLASH_NOR_I2C == nor_cfgs[cfg_count].dev_type) 
      {
        devices = nor_cfgs[cfg_count].dev_params;
        i2c_probe_cfg.dev_type = nor_cfgs[cfg_count].dev_type;      
        break;
      }
    }
	if ((dev_ctxt->dev_id == DALDEVICEID_FLASH_DEVICE_3) && (cfg_count == 1))
    {
      if (FLASH_NOR_I2C == nor_cfgs[cfg_count].dev_type) 
      {
        devices = nor_cfgs[cfg_count].dev_params;
        i2c_probe_cfg.dev_type = nor_cfgs[cfg_count].dev_type;      
        break;
      }
    }
//	if ((dev_ctxt->dev_id == DALDEVICEID_FLASH_DEVICE_4) && (cfg_count == 2))
//    {
//      if (FLASH_NOR_I2C == nor_cfgs[cfg_count].dev_type) 
//      {
//        devices = nor_cfgs[cfg_count].dev_params;
//        i2c_probe_cfg.dev_type = nor_cfgs[cfg_count].dev_type;      
//        break;
//      }
//    }
  }

  if (FLASH_UNKNOWN == i2c_probe_cfg.dev_type) 
  {
    /* Couldn't find a valid configuration */
    return result;
  }
  
  result = nor_i2c_configure(client_ctxt, devices);

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(3, ("\nNOR I2C Initialization failed!\n"));
    return FLASH_DEVICE_FAIL;  
  }

  /* initialize the I2C flash core driver */
  result = nor_i2c_init(client_ctxt);

  if (FLASH_DEVICE_DONE != result)
  {
    nor_i2c_close(client_ctxt);
    nor_i2c_mem_deinit(client_ctxt);
  }

  return result;
}
