/*====================================================================
 *
 * FILE:        flash_dal_util.c
 *
 * SERVICES:    Utility functions for flash DAL
 *
 * DESCRIPTION: Utility functions for flash DAL
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_dal_util.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/05/12     sv      Add functions for cache maintenance
 * 05/21/11     bb      Flash driver Optimization
 * 10/21/10     sv      Add flash_busy_wait API 
 * 08/11/10     nr      Added stats framework support
 * 07/21/09     sv      Modified to use DALSYS_memcpy
 * 07/14/09     eo      Moved flash_extract_uuid() to this file
 * 05/11/09     bb      Support for babylon
 * 03/24/09     bb      Added safe string copy function
 * 11/26/08     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_dal_util.h"

/* Instance creation for holding flash debug info */
struct flash_debug_info flash_dbg;

/* Entries for virtual to physical address conversion */
#define FLASH_MAX_VTOP_ENTRIES  2

struct vtop_item
{
  uint32 vstart;   /* Start of the virtual addr */
  uint32 size;     /* Virtual address range     */
  int32  pstart;   /* Start of the physical addr */
};

struct vtop_list
{
  uint32 no_of_entries;
  struct vtop_item vtop_entry[FLASH_MAX_VTOP_ENTRIES];
};

static struct vtop_list virt_phys_list;
/*
 * This API resets number of entries in the list to 0
 */
void flash_vtop_init_list()
{
  virt_phys_list.no_of_entries = 0;
}

/*
 * This API resets number of entries in the list to a specific value
 */
void flash_vtop_set_list_count(uint32 count)
{
  virt_phys_list.no_of_entries = count;
}

/*
 * This API adds an entry to the list
 */
void flash_vtop_add_item(uint32 vaddr, uint32 phys_addr, uint32 size)
{
  struct vtop_item *vitem;

  /* Ensure we have not exceeded the max number of entries possible */
  if (FLASH_MAX_VTOP_ENTRIES > virt_phys_list.no_of_entries)
  {
    vitem = &virt_phys_list.vtop_entry[virt_phys_list.no_of_entries];

    vitem->vstart = vaddr;
    vitem->size = size;
    vitem->pstart = phys_addr;

    virt_phys_list.no_of_entries++;
  }
  else
  {
    FLASHLOG(3, ("Exceeded FLASH_MAX_VTOP_ENTRIES\n"));
  }
}

/*
 * This API looks for the entry that matches within which lies the
 * virt_addr and returns the corresponding physical address
 */
uint32 flash_vtop_get_physical(uint32 virt_addr)
{
  uint32 item_count = 0;
  uint32 phy_address = 0;
  struct vtop_item *vitem;

  while (item_count < virt_phys_list.no_of_entries)
  {
    vitem = &virt_phys_list.vtop_entry[item_count];

    /* Check if vitem has a valid start address.
       if so, determine the virtual address range in which virt_addr
       belongs.  Address range for each item is calculated as
       vitem->vstart  ....   (vitem->vstart + vitem->size)
    */
    if ((vitem->vstart != DALSYS_MEM_ADDR_NOT_SPECIFIED) &&
        (virt_addr >= vitem->vstart) &&
        (virt_addr < (vitem->vstart + vitem->size)))
    {
      /* calculate the phycial addr of virt_addr using the offset
         (virt_addr-vitem->vstart) from vitem->pstart
      */
      phy_address = vitem->pstart + (virt_addr-vitem->vstart);
    }
    item_count++;
  }

  return phy_address;
}

/*
 * Safe string copy function for flash
 */
void flash_strcpy(uint8 *dest, const uint8 *src, uint32 len)
{
  DALSYS_memcpy(dest, src, len);
  dest[len-1] ='\0';
}

/*
 *  This function will extract UUID from the page data read out from OTP area
 *  in flash device.
 */
int flash_extract_uuid(flash_client_ctxt *client_ctxt, void * page_data, 
  struct nand_uuid_params *uuid_params, void *uuid_data, uint32 uuid_buf_len, 
  uint32 *uuid_len)
{
  int result = FLASH_DEVICE_FAIL;
  uint8 *uuid = (uint8 *)uuid_data;
  uint32 *data = (uint32 *)page_data;
  uint32 total_page_size = client_ctxt->client_data.total_page_size_bytes;
  uint32 page_size = client_ctxt->client_data.page_size_bytes;
  uint32 ignore_upper_byte = FALSE;
  uint32 uuid_offset = 0;
  uint32 i;
  
  /* Check to see if page has any thing written in it. Assume that page passed
   * is always going to have spare data in it
   */
  for (i = 0; i < (total_page_size/sizeof(uint32)); i++)
  {
    if (0xFFFFFFFF != data[i])
    {
      result = FLASH_DEVICE_DONE;
      break;
    }
  }

  /* Some devices may not have a signature. Check complimentary relationship
   * Work with 32-byte chunks from the page to see if complementary property
   * holds.
   */
  if (FLASH_DEVICE_DONE == result)
  {
    uint32 j, k;
    uint32 uuid_pair_count = 0;

    uint32 *uuid_data_ptr;
    uint32 *uuid_prime_ptr;

    /* setup the uuid len from the config data. If the buffer is smaller then 
     * copy buffer len bytes */
    if (uuid_buf_len > uuid_params->uuid_len)
    {
      *uuid_len = uuid_params->uuid_len;
    }
    else
    {
      *uuid_len = uuid_buf_len;
    }

    /* Check for x8 readable layout first. */   
    for (i = 0; i < page_size / (2 * (*uuid_len)); i++ )
    {
      k = i * (2 *  (*uuid_len));
      uuid_data_ptr = (uint32 *)(data + k / sizeof(uint32));
      uuid_prime_ptr = (uint32 *)(data + (k + (*uuid_len)) / sizeof(uint32));

      for (j = 0; j < (*uuid_len)/sizeof(uint32); j++)
      {
        if (uuid_data_ptr[j] != ~uuid_prime_ptr[j])
        {
          break;
        }
      } /* for (j = 0; j < (*uuid_len)/sizeof(uint32); j++) */

      /* If we were able to match all the complete 16 byte set increment
       * the count
       */
      if (j == ((*uuid_len)/sizeof(uint32)))
      {
        /* Rememeber this offset from where UUID will be copied over */
        if (0 == uuid_pair_count)
        {
          uuid_offset = k;
        }
        uuid_pair_count++;
      } 
    } /* for (i = 0; i < page_size/(2 * (*uuid_len)); i++ ) */
    
    /* Check for x16 type x8 readable layout if a uuid pair not found. */   
    if (0 == uuid_pair_count)  
    {
       /* At this point, assume it's x16 type */
       ignore_upper_byte = TRUE;

      /* May be this is a x16 interface device and higher byte in word
       * are not programmed.
       */
      for (i = 0; i < page_size / (4 * (*uuid_len)); i++ )
      {
        k = i * (4 * (*uuid_len));
        uuid_data_ptr = (uint32 *)(data + k/sizeof(uint32));
        uuid_prime_ptr = (uint32 *)(data + (k + 2 * (*uuid_len))/sizeof(uint32));

        for (j = 0; j < (2 * (*uuid_len)) / sizeof(uint32); j++)
        {
          /* Since we know that higher order bytes are all going to be 0xFFs
           * whcich do not carry any useful information, we can alter those
           * to satisfy the complement relashionsip. That way we will not have 
           * to reconstruct the data for checking complement relationship.
           */
          if ((uuid_data_ptr[j] | 0xFF00FF00) != 
            ~(uuid_prime_ptr[j] & 0x00FF00FF))
          {
            break;
          }
        }
        /* If we were able to match the complete 16 byte set increment
         * the count
         */
        if (j == (2 * (*uuid_len)/sizeof(uint32)))
        {
          /* Rememeber tis offset from where UUID will be copied over */
          if (0 == uuid_pair_count)
          {
            uuid_offset = k;
          }

          uuid_pair_count++;
        } /* if (j == (2 * (*uuid_len)/sizeof(uint32)))*/
      } /* for (i = 0; i < page_size/(4 * (*uuid_len)); i++ )*/
    }  /* else of  if (ignore_upper_byte != TRUE) */

    if (0 != uuid_pair_count)
    {
      FLASHLOG(5,("%d good complement sets found\n", uuid_pair_count));
      FLASHLOG(5,("\n\nUUID: 0x "));
      
      /* If UUID was found populate user buffer with UUID data and 
       * print it out in the terminal window
       */ 

      /* Set variable 'j' to define read stride. For x16 parts which have 
       * the page programmed in x8 readable mode, hop 2 bytes every iteration.
       * Normally, hop just 1 byte.
       */
      j = (ignore_upper_byte == TRUE) ? 1 : 0;

      for (i = 0; i < (*uuid_len); i++)
      {
        uuid[i] = ((uint8 *)data + (i<<j) + uuid_offset)[0];
      }

      /* Set UUID length to indicate that there was a good one in the page */
      *uuid_len = i;
    } /* if (uuid_pair_count != 0)*/
    else
    {
      /* Set UUID length to '0' to indicate that there wasn't one in page */
      *uuid_len = 0;
      FLASHLOG(5,("No UUID found\n"));
    }
  } /*  if (FLASH_DEVICE_DONE == result) */

  return result;
}

/*
 *  This function does a wait for a desired time in us.
 */
void flash_busy_wait(uint32 wait_time_us)
{
  DALSYS_BusyWait(wait_time_us);
}

/* This function does cache invalidation on the given address */
void flash_dcache_clean(uint32 addr, uint32 len)
{
  DALSYS_CacheCommand(DALSYS_CACHE_CMD_CLEAN, (uint32)addr, len);
}

/* This function does cache clean on the given address */
void flash_dcache_inval(uint32 addr, uint32 len)
{
  DALSYS_CacheCommand(DALSYS_CACHE_CMD_INVALIDATE, (uint32)addr, len);
}
