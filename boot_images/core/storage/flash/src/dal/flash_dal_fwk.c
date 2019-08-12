/*=============================================================================
 *
 * FILE:      flash_dal_fwk.c
 *
 * DESCRIPTION: Function definitions specific to Flash DAL framework
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_dal_fwk.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/29/13     sv      Fix KW error - Memory leak issue
 * 09/05/12     bb/sv   Call entry layer APIs directly
 * 03/19/12     sv      Add SPI NOR power management support
 * 03/19/12     eo      Support for erase blocks by size
 * 09/15/10     bb      Added support for multiple device IDs
 * 05/11/10     eo      Added NOR API support
 * 12/28/09     sc      Added 2x support in DAL
 * 02/23/09     bb      Read/Write API changes with iovec design changes
 * 11/24/08     mm      Support for UUID
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALFramework.h"

#include "flash_dal.h"

/***********************************************************************
 *   Data structures
 ***********************************************************************/
/* Holds the list of the flash clients */
static flash_client_ctxt *static_flash_clients = NULL;

/***********************************************************************
 *    Functions
 ***********************************************************************/
static void flash_link_in_client (flash_client_ctxt *client_ctxt);
static void flash_link_out_client (flash_client_ctxt *client_ctxt);

int flash_fwk_attach(const char *pszArg, DALDEVICEID dev_id,
  DalDeviceHandle **handle_dal_device);
static uint32 flash_fwk_detach(DalDeviceHandle* pif);
static int flash_fwk_init(DalDeviceHandle *pif);
static int flash_fwk_deinit(DalDeviceHandle *pif);
static int flash_fwk_open(DalDeviceHandle *pif, uint32 mode);
static int flash_fwk_close(DalDeviceHandle *pif);
static int flash_fwk_get_daldevice_info(DalDeviceHandle *pif,
  DalDeviceInfo* info, uint32 size);

static uint32 flash_fwk_addref(flash_handle* pif);
static void flash_init_interface(flash_client_ctxt* client_ctxt);

/*
 * This API links in the flash client handle to static_flash_clients
*/
static void flash_link_in_client (flash_client_ctxt *client_ctxt)
{
  if (static_flash_clients == NULL)
  {
    static_flash_clients = client_ctxt;
  }
  else
  {
    flash_client_ctxt *temp_client_ctxt = static_flash_clients;

    while (temp_client_ctxt->next != NULL)
    {
      temp_client_ctxt = temp_client_ctxt->next;
    }

    temp_client_ctxt->next = client_ctxt;
    client_ctxt->next = NULL;
  }
}

/*
 * This API removes the flash client handle from static_flash_clients
*/
static void flash_link_out_client (flash_client_ctxt *client_ctxt)
{
  if (static_flash_clients != NULL)
  {
    flash_client_ctxt *temp_client_ctxt = static_flash_clients;

    if (client_ctxt == static_flash_clients)
    {
      static_flash_clients = static_flash_clients->next;
    }
    else
    {
      while (temp_client_ctxt->next != client_ctxt)
      {
        temp_client_ctxt = temp_client_ctxt->next;
      }
      temp_client_ctxt->next = temp_client_ctxt->next->next;
    }
  }
}

/*------------------------------------------------------------------------------
 *   Following functions are defined in base IQI CS Interface.
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice invoked from DAL framework
  struct DalDevice
  {
    Attach      =   flash_fwk_attach
    Detach      =   flash_fwk_detach,
    Init        =   flash_fwk_init,
    DeInit      =   flash_fwk_deinit,
    Open        =   flash_fwk_open,
    Close       =   flash_fwk_close,
    Info        =   flash_fwk_get_daldevice_info,
    PowerEvent  =   flash_fwk_handle_power_event,
    SysRequest  =   flash_fwk_handle_sys_request
  }
------------------------------------------------------------------------------*/

/*    This function does the following:
 *              1. Alocates memory for the flash client context,
 *              2. Intializes the flash handle function table vtbl,
 *              3. Links in the client ctx to static_flash_clients.
 *              4. Increments the number of clients (ref count)
 *              5. Attach device to DALFW
 */
int flash_fwk_attach(const char *pszArg, DALDEVICEID dev_id,
  DalDeviceHandle **flash_dal_handle)
{
  int err;
  static flash_drv_ctxt drv_ctxt = {{flash_dal_driver_init,
    flash_dal_driver_deinit}, 2, sizeof(flash_dev_ctxt)};
  flash_client_ctxt *client_ctxt =  NULL;

  /* Alocates memory for the flash client context */
  err = DALSYS_Malloc(sizeof(flash_client_ctxt),(void **)&client_ctxt);

  if (DAL_SUCCESS == err)
  {
    DALSYS_memset(client_ctxt, 0x0, sizeof(flash_client_ctxt));
    err = DALFW_AttachToDevice(dev_id,(DALDrvCtxt *)&drv_ctxt,
      (DALClientCtxt *)client_ctxt);

    if (DAL_SUCCESS == err)
    {
      /* Intialize the flash handle function table vtbl */
      flash_init_interface(client_ctxt);

      /* Increase the number of references to this device handle */
      flash_fwk_addref(&(client_ctxt->handle));
      *flash_dal_handle = (DalDeviceHandle *)&(client_ctxt->handle);

      /* Link in the client ctx to static_flash_clients */
      flash_link_in_client(client_ctxt);
    }
    else
    {
      DALSYS_Free(client_ctxt);
    }
  }

  return err;
}

/*
 * This API calls the DALFW_Release API to detach the device from DAL
 *  framework and remnoves the clien handle from the static_flash_clients
 */
static uint32 flash_fwk_detach(DalDeviceHandle *pif)
{
  flash_handle_t handle = (flash_handle *)pif;
  uint32 ref = DALFW_Release((DALClientCtxt *)handle->flash_client_ctxt);

  flash_link_out_client(handle->flash_client_ctxt);

  if(0 == ref)
  {
    /* Release the client ctxt */
    DALSYS_Free(handle->flash_client_ctxt);
  }

  return ref;
}

/* This API Establishes the DAL property handle for the device and
 * initializes teh NAND device.
 */
static int flash_fwk_init(DalDeviceHandle *pif)
{
  flash_handle_t handle = (flash_handle *)pif;
  flash_dev_ctxt *dev_ctxt = handle->flash_client_ctxt->flash_dev_ctxt;

  /* Establish the DAL property handle for this device */
  DALSYS_GetDALPropertyHandle(dev_ctxt->dev_id, dev_ctxt->handle_prop);

  return flash_dal_device_init(handle->flash_client_ctxt);
}

/* This uninitializes the flash driver */
static int flash_fwk_deinit(DalDeviceHandle *pif)
{
   flash_handle_t handle = (flash_handle *)pif;
   return flash_dal_device_deinit(handle->flash_client_ctxt);
}

/* This function is available in the client as a requirement of DAL
 * framework.  Currently it does not do any thing but invoke the
 * flash_dal_open which which returns DAL_SUCCESS
 */
static int flash_fwk_open(DalDeviceHandle *pif, uint32 mode)
{
  FLASHLOG(4, ("Flash Open Called\n"));
  return DAL_SUCCESS;
}

/* This function invokes the flash_dal_close which in turn
 * calls the NAND layer close function that frees any malloced
 * extra data for client and closes the hal device.
 */
static int flash_fwk_close(DalDeviceHandle *pif)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  FLASHLOG(4, ("Flash close Called\n"));

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->close)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->close(client_ctxt);
  }

  return status;
}

/* This function invoke the flash_dal_info which  sets the Version field
 *  of the DalDeviceInfo struct
 */
static int flash_fwk_get_daldevice_info(DalDeviceHandle *pif,
  DalDeviceInfo* info, uint32 size)
{
  FLASHLOG(4, ("Flash Info Called\n"));
  info->Version = DALFLASH_INTERFACE_VERSION;
  return DAL_SUCCESS;
}

/* This function is available in the client as a requirement
 *  of DAL framework for power management.  Currently it does
 *  not do any thing but invoke the flash_dal_power_event which
 *  calls the nand device layer power event API anfd this
 *  simply which returns FLASH_DEVICE_DONE
 */
static int flash_fwk_handle_power_event(DalDeviceHandle *pif,
  DalPowerCmd power_cmd, DalPowerDomain power_domain )
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->power_event)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->power_event(client_ctxt,
      power_cmd, power_domain);
  }

  return status;
}

/* This function is available to the client as a requirement
 * of DAL framework.
 */
static int flash_fwk_handle_sys_request(DalDeviceHandle *pif, DalSysReq req_idx,
  const void *src_buf, uint32 src_buf_len, void *dest_buf, uint32 dest_buf_len,
  uint32* DestBufLenReq)
{
  return DAL_ERROR;
}

/*  This API increments the reference count indicating the number of flash clients
*/
static uint32 flash_fwk_addref(flash_handle* pif)
{
  return DALFW_AddRef((DALClientCtxt *)(pif->flash_client_ctxt));
}

/*------------------------------------------------------------------------------
  Following functions are extended in flash_handle Interface available to flash
  clients

  open_partition      =  flash_fwk_open_partition,
  get_info            =  flash_fwk_get_info,
  get_status          =  flash_fwk_get_status,
  erase_blocks        =  flash_fwk_erase_blocks,
  erase_suspend       =  flash_fwk_erase_suspend,
  erase_resume        =  flash_fwk_erase_resume,
  block_set_state     =  flash_fwk_block_set_state,
  block_get_state     =  flash_fwk_block_get_state,
  write_pages         =  flash_fwk_write_pages,
  write_partial_page  =  flash_fwk_write_partial_page,
  read_pages          =  flash_fwk_read_pages,
  read_otp_page       =  flash_fwk_read_otp_page,
  copy_pages          =  flash_fwk_copy_pages
  get_uuid            = flash_fwk_get_uuid,
  erase_blocks_by_size = flash_fwk_erase_blocks_by_size,
  set_power_mode      =  flash_fwk_set_power_mode
------------------------------------------------------------------------------*/

/* This function invokes the flash_dal_open_partition which
 * in turn calls the open partition API of the flash device driver
 */
static int flash_fwk_open_partition(flash_handle *pif, const unsigned char
  *partition_name)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->open_partition)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->open_partition(client_ctxt,
      partition_name);
  }

  return status;
}

/* This function invokes the flash_dal_get_info which in turn calls
 * the get_info API of the flash driver
 */
static int flash_fwk_get_info(flash_handle *pif,
  enum flash_info_type info_type, void *flash_info)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->get_info)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->get_info(client_ctxt,
      info_type, flash_info);
  }

  return status;
}

/* This invoke the flash_dal_get_status which in turn invokes
 * flash driver get status API.
 */
static int flash_fwk_get_status(flash_handle *pif)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->get_status)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->get_status(client_ctxt);
  }

  return status;
}

/* This function invokes the flash_dal_erase_blocks which in turn
 * invokes the client context erase block API.
 */
static int flash_fwk_erase_blocks(flash_handle *pif,
  struct flash_block_vector *block_vectors, uint32 vector_count)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->erase_blocks)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->erase_blocks(client_ctxt,
      block_vectors, vector_count);
  }

  return status;
}

/* This function invokes the flash_dal_begin_erase_block which in turn
 * invokes the client context begin erase block API.
 */
static int flash_fwk_begin_erase_block(flash_handle *pif, uint32 block)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->begin_erase_block)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->begin_erase_block(
      client_ctxt, block);
  }

  return status;
}

/* This function invokes the flash_dal_erase_suspend which in turn invokes
 * the client context erase_suspend  API.
 */
static int flash_fwk_erase_suspend(flash_handle *pif)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->erase_suspend)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->erase_suspend(client_ctxt);
  }

  return status;
}

/* This function invokes the flash_dal_erase_resume which in turn invokes
 * the client context erase_resume  API.
 */
static int flash_fwk_erase_resume(flash_handle *pif)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->erase_resume)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->erase_resume(client_ctxt);
  }

  return status;
}

/* This function invokes the flash_dal_flash_dal_block_set_state which
 *  inturn invokes the client context flash_dal_block_set_state API.
 */
static int flash_fwk_block_set_state(flash_handle *pif, uint32 block,
  enum flash_block_state block_state)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->block_set_state)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->block_set_state(client_ctxt,
      block, block_state);
  }

  return status;
}

/* This function invokes the flash_dal_flash_dal_block_get_state which
 * inturn invokes the client context flash_dal_block_get_state API.
 */
static int flash_fwk_block_get_state(flash_handle *pif, uint32 block,
  enum flash_block_state *block_state)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->block_get_state)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->block_get_state(client_ctxt,
      block, block_state);
  }

  return status;
}

/* This function invokes the flash_dal_write_pages which in
 * turn invokes the client context write_pages API.
 */
static int flash_fwk_write_pages(
  flash_handle *pif, enum page_write_opcode write_type,
  dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->write_pages)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->write_pages(
      client_ctxt, write_type, buffer_desc, page_data_iovec);
  }

  return status;
}

/* This function invokes the flash_dal_write_partial_page which in turn
 * invokes the client context write_partial_page API.
 */
static int flash_fwk_write_partial_page(flash_handle *pif,
  dalsys_mem_desc_list *buffer_desc,
  const struct flash_partial_page_op *partial_page_op)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->write_partial_page)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->write_partial_page(
      client_ctxt, buffer_desc, partial_page_op);
  }

  return status;
}

/* This function invokes the flash_dal_write_2x_pages which in
 * turn invokes the client context write_2x_pages API.
 */
static int flash_fwk_write_2x_pages(
  flash_handle *pif, enum page_write_opcode write_type,
  dalsys_mem_desc_list *buffer_desc,
  struct flash_2x_page_iovec *page_2x_data_iovec)
{
  return FLASH_DEVICE_NOT_SUPPORTED;
}

/* This function invokes the flash_dal_read_pages which in turn invokes
 * the client context read_pages API.
 */
static int flash_fwk_read_pages(
  flash_handle *pif, enum page_read_opcode read_type,
  dalsys_mem_desc_list *buffer_desc,
  struct flash_page_iovec *page_data_iovec)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->read_pages)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->read_pages(
      client_ctxt, read_type, buffer_desc, page_data_iovec);
  }

  return status;
}

/* This function invokes the flash_dal_read_pointer which in turn invokes
 * the client context read_pointer API.
 */
static int flash_fwk_read_pointer(
  flash_handle *pif, void **read_addr, uint32 page)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->read_pointer)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->read_pointer(
      client_ctxt, read_addr, page);
  }

  return status;
}

/* This function invokes the flash_dal_copy_pages which in turn invokes
 * the client context copy_pages API.
 */
static int flash_fwk_copy_pages(flash_handle *pif,
  struct flash_copy_iovec *copy_vectors, uint32 vector_cnt)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->copy_pages)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->copy_pages(
      client_ctxt, copy_vectors, vector_cnt);
  }

  return status;
}

/* This function invokes the flash_dal_get_uuid which in turn invokes
 * the client context get_uuid API.
 */
static int flash_fwk_get_uuid(flash_handle *pif, void *uuid,
  uint32 uuid_buf_len, uint32* uuid_len)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->get_uuid)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->get_uuid(client_ctxt,
      uuid, uuid_buf_len, uuid_len);
  }

  return status;
}

/* This function invokes the flash_dal_erase_blocks_by_size which in turn
 * invokes the client context erase block by size API.
 */
static int flash_fwk_erase_blocks_by_size(flash_handle *pif,
  enum block_size_opcode block_size_type,
  struct flash_block_vector *block_vectors, uint32 vector_count)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->erase_blocks_by_size)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->erase_blocks_by_size(
      client_ctxt, block_size_type, block_vectors, vector_count);
  }

  return status;
}

/* This function invokes the flash_dal_set_power_mode which in turn invokes
 * the client context set_power_mode API.
 */
static int flash_fwk_set_power_mode(flash_handle *pif,
  enum flash_power_mode power_mode)
{
  int status = FLASH_DEVICE_FAIL;
  flash_client_ctxt *client_ctxt = ((flash_handle *)pif)->flash_client_ctxt;

  if (FLASH_DEV_ENTRY_VTBL(client_ctxt)->set_power_mode)
  {
    status = FLASH_DEV_ENTRY_VTBL(client_ctxt)->set_power_mode(client_ctxt,
      power_mode);
  }

  return status;
}

/* This function intializes the flash handle function table vtbl.
 * It fills in teh API for the DALDevice function table used by
 * the DAL framework as a part of attach, detach, open, close,
 * init, deint.  It also intializes the APIs for the client
 * interface
 */
static void flash_init_interface(flash_client_ctxt* client_ctxt)
{
  static const flash_handle_vtbl vtbl = {
    { flash_fwk_attach,
      flash_fwk_detach,
      flash_fwk_init,
      flash_fwk_deinit,
      flash_fwk_open,
      flash_fwk_close,
      flash_fwk_get_daldevice_info,
      flash_fwk_handle_power_event,
      flash_fwk_handle_sys_request
    },
    flash_fwk_addref,
    flash_fwk_open_partition,
    flash_fwk_get_info,
    flash_fwk_get_status,
    flash_fwk_erase_blocks,
    flash_fwk_begin_erase_block,
    flash_fwk_erase_suspend,
    flash_fwk_erase_resume,
    flash_fwk_block_set_state,
    flash_fwk_block_get_state,
    flash_fwk_write_pages,
    flash_fwk_write_partial_page,
    flash_fwk_write_2x_pages,
    flash_fwk_read_pages,
    flash_fwk_read_pointer,
    flash_fwk_copy_pages,
    flash_fwk_get_uuid,
    flash_fwk_erase_blocks_by_size,
    flash_fwk_set_power_mode
  };

  /*-----------------------------------------------------------------------
  Depending upon client type setup the vtables (entry points)
  -------------------------------------------------------------------------*/
  client_ctxt->handle.vtbl  = &vtbl;
  client_ctxt->handle.flash_client_ctxt = client_ctxt;
  client_ctxt->handle.flash_handle_id = DALDEVICE_INTERFACE_HANDLE_ID;
}
