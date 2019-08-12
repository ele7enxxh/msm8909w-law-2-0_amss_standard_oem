/*=======================================================================
 * FILE:        nor_wrapper.c
 *
 * SERVICES:    None
 *
 * GENERAL DESCRIPTION
 *    This file implements wrapper functions for
 *    read/write APIs in flash driver.
 *
 * Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
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
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/nor_wrapper.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 04/06/11     jz      Added readback verify support for NOR programming
 * 09/10/10     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALFramework.h"
#include "flash.h"
#include "flash_log.h"

/* DAL Memory descriptor for page operations */
static DALSYS_MEM_DESC_LIST_OBJECT(page_buff_desc, 1);

static DALBOOL buff_desc_initialized = FALSE;
static struct flash_page_iovec page_data_iovec;
static struct flash_partial_page_op partial_page_op;

/* The following flag is controlled by .cmm script */
uint32 readback_verify_enabled = 0;

static void init_page_param(struct flash_info *dev_info,
  uint32 page, uint32 page_count, struct flash_page_iovec *page_data)
{
  page_data->main_size_bytes =
    dev_info->partition.page_size_bytes;
  /* No Spare area in NOR */
  page_data->spare_size_bytes = 0;

  page_data->start_page = page;
  page_data->total_page_count = page_count;
}

static void init_partial_page_param(struct flash_info *dev_info,
  uint32 page, uint32 offset, uint32 byte_count, 
  struct flash_partial_page_op *partial_page_data)
{
  partial_page_data->page = page;
  partial_page_data->byte_offset = offset;
  partial_page_data->write_size_bytes = byte_count;
}

static void init_buffer_descriptor(struct flash_info *dev_info,
  uint32 buff_addr, uint32 buff_size, dalsys_mem_desc_list *buffer_desc,
  uint32 num_pages)
{
  DALSysMemDescBuf *desc_buff;

  if (FALSE == buff_desc_initialized)
  {
    DALFW_MemDescInit(NULL, (DALSysMemDescList *) buffer_desc, 1);
    buff_desc_initialized = TRUE;
  }

  desc_buff = DALFW_MemDescBufPtr(buffer_desc, 0);

  buffer_desc->dwNumDescBufs = 1;

  desc_buff->VirtualAddr = buff_addr;
  desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
  desc_buff->size = buff_size;
  desc_buff->user = num_pages;
}

int flash_prog_read_pages(flash_handle_t handle, struct flash_info *dev_info, 
  uint32 start_page, uint32 num_pages, void *buffer)
{
  int result = FLASH_DEVICE_DONE;
  uint32 page_size;
  /* Initialize the page data IOVEC structure */
  init_page_param(dev_info, start_page, num_pages, &page_data_iovec);

  /* Initialize buffer descriptors */
  page_size = dev_info->partition.page_size_bytes;
  init_buffer_descriptor(dev_info, (uint32) buffer, num_pages * page_size, 
    (DALSysMemDescList *) &page_buff_desc, num_pages);

  result = flash_read_pages(handle, FLASH_READ_MAIN,
    (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  return result;
}

static uint32 flash_prog_readback_verify(flash_handle_t handle,
  struct flash_info *dev_info,
  uint32 start_page, uint32 num_pages, void *buffer)
{
  int result = FLASH_DEVICE_DONE;
  uint32 page;
  uint32 page_size;
  static unsigned char readback_page_buf[MAX_PAGE_SIZE]; 
  uint8 *wbuf = buffer;
  uint32 i;
  unsigned char *vsrc1, *vsrc2;

  page = start_page;
  page_size = dev_info->partition.page_size_bytes;
  
  while (page < (start_page+num_pages))
  {
    DALSYS_memset(readback_page_buf, 0xFF, page_size);
    if (flash_prog_read_pages(handle, dev_info, page, 1, readback_page_buf)
      != FLASH_DEVICE_DONE)
    {
      result = FLASH_DEVICE_FAIL;
      FLASHLOG(3, ("Readback failed for page %u \n", page));
      return result;
    }

    if (memcmp(wbuf, readback_page_buf, page_size) != 0)
    {
      FLASHLOG(1, ("Error: verify failure\n"));
      FLASHLOG(1, ("src at 0x%08X  readback at 0x%08X\n",
        (unsigned int)wbuf, (unsigned int)readback_page_buf));
      FLASHLOG(1, ("Page = %d \n", page));

      vsrc1 = wbuf;
      vsrc2 = readback_page_buf;
    
      for (i=0; i < page_size; i++)
      {
        if (*vsrc1++ != *vsrc2++)
        {
          FLASHLOG(1, ("Verify failed at index %ld\n", (unsigned long int)i));
          result = FLASH_DEVICE_FAIL;
          return result;
        }
      }
    }
    
    page++;
    wbuf += page_size;
  }
  
  return result;
}

int flash_prog_write_pages(flash_handle_t handle, struct flash_info *dev_info,
  uint32 start_page, uint32 num_pages, void *buffer)
{
  int result = FLASH_DEVICE_DONE;
  uint32 page_size;
  /* Initialize the page data IOVEC structure */
  init_page_param(dev_info, start_page, num_pages, &page_data_iovec);
 
  /* Initialize buffer descriptors */
  page_size = dev_info->partition.page_size_bytes;
  init_buffer_descriptor(dev_info, (uint32) buffer, num_pages * page_size, 
    (DALSysMemDescList *) &page_buff_desc, num_pages);

  result = flash_write_pages(handle, FLASH_WRITE_MAIN,
    (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  if (result == FLASH_DEVICE_DONE && readback_verify_enabled)
  {
    result = flash_prog_readback_verify(handle, dev_info,
      start_page, num_pages, buffer);
  }
  
  return result;
}

int flash_prog_write_partial_page(flash_handle_t handle, 
  struct flash_info *dev_info, uint32 page, uint32 offset, 
  uint32 num_bytes, void *buffer)
{
  int result = FLASH_DEVICE_DONE;

  /* Initialize the page data IOVEC structure */
  init_partial_page_param(dev_info, page, offset, num_bytes, &partial_page_op);

  /* Initialize buffer descriptors */
  init_buffer_descriptor(dev_info, (uint32) buffer, num_bytes,
    (DALSysMemDescList *) &page_buff_desc, 1);

  result = flash_write_partial_page(handle, 
    (DALSysMemDescList *) &page_buff_desc, &partial_page_op);

  return result;
}

