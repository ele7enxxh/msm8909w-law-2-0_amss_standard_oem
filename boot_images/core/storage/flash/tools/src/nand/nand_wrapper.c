/*=======================================================================
 * FILE:        nand_wrapper.c
 *
 * SERVICES:    None
 *
 * GENERAL DESCRIPTION
 *    This file implements wrapper functions for
 *    read/write APIs in flash driver.
 *
 * Copyright (c) 2010-2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_wrapper.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/28/15     sb      Remove KW Banned Functions
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 04/15/10     jz      Code cleanups
 * 02/18/09     jz      Renamed nand_ to flash_prog_ for nand tool
 * 11/26/08     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "nand_wrapper.h"
#include "stringl/stringl.h"

/* DAL Memory descriptor for page operations */
static DALSYS_MEM_DESC_LIST_OBJECT(page_buff_desc, 2);

static DALBOOL buff_desc_initialized = FALSE;

/*
 * Initializes page data parameters
 */
static __inline void init_page_param(struct flash_info *dev_info,
  uint32 op_type, uint32 page, uint32 page_count,
  struct flash_page_iovec *page_data, uint32 spare_udata_blen)
{
  switch (op_type)
  {
    case FLASH_READ_RAW:
    //case FLASH_WRITE_RAW:
      page_data->main_size_bytes =
        dev_info->partition.total_page_size_bytes;
      page_data->spare_size_bytes = 0;
      break;

    case FLASH_READ_MAIN:
    //case FLASH_WRITE_MAIN:
      page_data->main_size_bytes =
        dev_info->partition.page_size_bytes;
      page_data->spare_size_bytes = 0;
      break;

    case FLASH_READ_MAIN_SPARE:
    //case FLASH_WRITE_MAIN_SPARE:
      page_data->main_size_bytes =
        dev_info->partition.page_size_bytes;
      page_data->spare_size_bytes = spare_udata_blen;
      break;

    case FLASH_READ_SPARE:
    //case FLASH_WRITE_SPARE:
      page_data->main_size_bytes = 0;
      page_data->spare_size_bytes = spare_udata_blen;
      break;
  }

  page_data->start_page = page;
  page_data->total_page_count = page_count;
}


/*
 * Initializes IOVEC buffer parameters
 */
static __inline void init_buffer_descriptor(
   struct flash_page_iovec *page_data,
   uint32 op_type, uint32 buff_addr, int32 spare_buff_addr,
   dalsys_mem_desc_list *buffer_desc)
{
  DALSysMemDescBuf *desc_buff;
  DALSysMemDescBuf *spare_desc_buff;

  if (FALSE == buff_desc_initialized)
  {
    DALFW_MemDescInit(NULL, (DALSysMemDescList *) buffer_desc, 2);
    buff_desc_initialized = TRUE;
  }

  desc_buff = DALFW_MemDescBufPtr(buffer_desc, 0);
  spare_desc_buff = DALFW_MemDescBufPtr(buffer_desc, 1);

  buffer_desc->dwNumDescBufs = 2;

  desc_buff->VirtualAddr = buff_addr;
  desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
  desc_buff->size =
    page_data->main_size_bytes * page_data->total_page_count;
  desc_buff->user = 1;

  spare_desc_buff->VirtualAddr = spare_buff_addr;
  spare_desc_buff->PhysicalAddr = spare_desc_buff->VirtualAddr;
  spare_desc_buff->size =
    page_data->total_page_count * page_data->spare_size_bytes;
  spare_desc_buff->user = 1;
}

/*
 * Wrapper function for flash read pages driver API
 */
int flash_prog_read_page(flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 read_type, void *buffer)
{
  int result = FLASH_DEVICE_DONE;
  struct flash_page_iovec page_data_iovec;

  /* Initialize the page data IOVEC structure */
  init_page_param(dev_info, read_type, page, 1, &page_data_iovec, 0);

  /* Initialize buffer descriptors */
  init_buffer_descriptor(&page_data_iovec, read_type,
    (uint32) buffer, NULL, (DALSysMemDescList *) &page_buff_desc);

  result = flash_read_pages(handle, (enum page_read_opcode) read_type,
    (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  return result;
}

/*
 * Wrapper function for flash write pages driver API
 */
int flash_prog_write_page(flash_handle_t handle, struct flash_info *dev_info,
   uint32 page, uint32 write_type, void *buffer)
{
  int result = FLASH_DEVICE_DONE;
  struct flash_page_iovec page_data_iovec;

  /* Initialize the page data IOVEC structure */
  init_page_param(dev_info, write_type, page, 1, &page_data_iovec, 0);

  /* Initialize buffer descriptors */
  init_buffer_descriptor(&page_data_iovec, write_type,
    (uint32) buffer, NULL, (DALSysMemDescList *) &page_buff_desc);

  result = flash_write_pages(handle, (enum page_write_opcode) write_type,
    (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  return result;
}

/*
 * Wrapper function for flash write pages with udata
 */
int flash_prog_write_page_and_udata (flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 write_type, void *buffer, void *spare_udata,
  uint32 spare_udata_blen)
{
  int result = FLASH_DEVICE_DONE;
  struct flash_page_iovec page_data_iovec;

  /* Initialize the page data IOVEC structure */
  init_page_param(dev_info, write_type, page, 1,&page_data_iovec, spare_udata_blen);

  /* Initialize buffer descriptors */
  init_buffer_descriptor(&page_data_iovec, write_type,
    (uint32) buffer, (uint32) spare_udata, (DALSysMemDescList *) &page_buff_desc);

  result = flash_write_pages(handle, (enum page_write_opcode) write_type,
    (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  return result;
} /* End of flash_nand_write_page_and_udata */

/*
 * Wrapper function for flash read pages with udata
 */
int flash_prog_read_page_and_udata (flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 read_type, void *buffer, void *spare_udata,
  uint32 spare_udata_blen)
{
  int result = FLASH_DEVICE_DONE;     /* return status of read page operation */
  struct flash_page_iovec page_data_iovec;

  /* Initialize the page data IOVEC structure */
  init_page_param(dev_info, read_type, page, 1,&page_data_iovec, spare_udata_blen);

  /* Initialize buffer descriptors */
  init_buffer_descriptor(&page_data_iovec, read_type,  (uint32) buffer,
       (uint32)spare_udata, (DALSysMemDescList *) &page_buff_desc);

  result = flash_read_pages(handle, (enum page_read_opcode) read_type,
    (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  return result;

} /* End of flash_nand_read_page_and_udata */

/*
 * Wrapper for memcpy API. Introduced as a
 * Workaround for unaligned memcpy failure with RVCT 4.0
 */
void* nand_memcpy(void *out, const void *in, uint32 n)
{
  uint32 s_addr, d_addr;

  s_addr = (uint32) out;
  d_addr = (uint32) in;

  if ((s_addr & 0x3) || (d_addr & 0x3))
  {
    uint32 i;

    for (i=0; i<n; ++i)
    {
      *((char*)out+i) = *((char*)in+i);
    }
  }
  else
  {
    memscpy(out, n, in, n);
  }

  return out;
}

