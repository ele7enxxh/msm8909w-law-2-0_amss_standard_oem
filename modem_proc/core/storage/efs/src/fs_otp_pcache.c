/***********************************************************************
 * fs_otp_pcache.c
 *
 * EFS OTP (One-Time-Programmable) Page-Cache module.
 * Copyright (C) 2011, 2013 QUALCOMM Technologies, Inc.
 *
 * This file reads OTP pages from the flash partition and caches them
 * for the EFS OTP module to read and write to.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_otp_pcache.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2011-08-17   rp    Create

===========================================================================*/

#include "fs_otp_pcache.h"

#ifdef FEATURE_EFS_OTP

#include "fs_err.h"
#include "fs_util.h"

#include "assert.h"

#include <string.h>


void
fs_otp_pcache_init (struct fs_otp_pcache *pcache, efs_device_t dev,
                    uint32 dev_total_pages)
{
  pcache->dev = dev;
  pcache->dev_total_pages = dev_total_pages;
  pcache->dev_page_size = fs_device_page_size (dev);
}


static int
fs_otp_pcache_lookup (struct fs_otp_pcache *pcache, uint32 page_nbr)
{
  int i;

  for (i = 0; i < FS_OTP_MAX_PCACHE_PAGES; ++i)
  {
    if (pcache->status[i].is_in_use &&
        pcache->status[i].page_nbr == page_nbr)
    {
      ++pcache->status[i].age;
      break;
    }
  }

  if (i == FS_OTP_MAX_PCACHE_PAGES)
    i = -1;

  return i;
}

/* This function adds a page in the page-cache. The last argument
 * is_for_write is used to tell if this page will be written or not. */
static int
fs_otp_pcache_add_page (struct fs_otp_pcache *pcache, uint32 page_nbr,
                        int is_for_write)
{
  int i, oldest_age_slot = -1;
  uint32 oldest_age = 0;
  int result;

  for (i = 0; i < FS_OTP_MAX_PCACHE_PAGES; ++i)
  {
    ASSERT (pcache->status[i].page_nbr != page_nbr);

    if (pcache->status[i].is_for_write)
      continue; /* Skip slots opened for write. */

    if (oldest_age_slot == -1)
    {
      oldest_age = pcache->status[i].age;
      oldest_age_slot = i;
    }

    if (pcache->status[i].age < oldest_age)
    {
      oldest_age = pcache->status[i].age;
      oldest_age_slot = i;
    }
  }

  i = oldest_age_slot;
  if (i == -1)
  {
    FS_ERR_FATAL ("no free slots.", 0, 0, 0);
  }

  pcache->status[i].is_in_use = 1;
  pcache->status[i].is_for_write = is_for_write;
  pcache->status[i].page_nbr = page_nbr;
  pcache->status[i].age = 0;

  result = fs_device_read_page (pcache->dev, page_nbr, pcache->pages[i],
                                FS_FOP_USER, FS_FLASH_FAIL_OK);
  if (result != FS_DEVICE_RESULT_SUCCESS)
  {
    if (result == FS_DEVICE_FAIL_PAGE_ERASED)
    {
      memset (pcache->pages[i], 0xFF, pcache->dev_page_size);
    }
    else
    {
      FS_ERR_FATAL ("[%d]otp read page fail", page_nbr, 0, 0);
    }
  }
  else
  {
    /* This page is already written once, so make sure we don't
     * open this same page with the intent to write again. */
    if (is_for_write)
    {
      FS_ERR_FATAL ("[%d,%d] attempt to over write page", page_nbr, result, 0);
    }
  }

  return i;
}

void
fs_otp_pcache_read_bytes (struct fs_otp_pcache *pcache, uint32 offset,
                          uint32 bytes_count, uint8 *buffer)
{
  uint32 page_nbr, page_offset, next_page_offset, avail_bytes;
  uint8 *src_addr;
  int i;

  while (bytes_count > 0)
  {
    page_nbr = offset / pcache->dev_page_size;
    page_offset = offset % pcache->dev_page_size;

    i = fs_otp_pcache_lookup (pcache, page_nbr);
    if (i == -1)
    {
      i = fs_otp_pcache_add_page (pcache, page_nbr, 0);
    }
    ASSERT ((i >= 0) && (i < FS_OTP_MAX_PCACHE_PAGES));

    next_page_offset = (page_nbr + 1) * pcache->dev_page_size;
    avail_bytes = next_page_offset - offset;
    if (avail_bytes > bytes_count)
      avail_bytes = bytes_count;

    bytes_count -= avail_bytes;
    offset += avail_bytes;

    src_addr = pcache->pages[i] + page_offset;
    fs_memscpy (buffer, avail_bytes, src_addr, avail_bytes);
    buffer += avail_bytes;
  }
}

void
fs_otp_pcache_write_bytes (struct fs_otp_pcache *pcache, uint32 offset,
                           uint32 bytes_count, uint8 *buffer)
{
  uint32 page_nbr, page_offset, next_page_offset, avail_bytes;
  uint8 *dest_addr;
  int i, write_page, result;

  while (bytes_count > 0)
  {
    page_nbr = offset / pcache->dev_page_size;
    page_offset = offset % pcache->dev_page_size;
    ASSERT (page_nbr < pcache->dev_total_pages);

    i = fs_otp_pcache_lookup (pcache, page_nbr);
    if (i == -1)
    {
      i = fs_otp_pcache_add_page (pcache, page_nbr, 1);
    }
    ASSERT ((i >= 0) && (i < FS_OTP_MAX_PCACHE_PAGES));

    pcache->status[i].is_for_write = 1;
    write_page = 0;
    next_page_offset = (page_nbr + 1) * pcache->dev_page_size;
    avail_bytes = next_page_offset - offset;
    if (avail_bytes > bytes_count)
      avail_bytes = bytes_count;
    else
      write_page = 1;

    bytes_count -= avail_bytes;
    offset += avail_bytes;

    dest_addr = pcache->pages[i] + page_offset;
    fs_memscpy (dest_addr, pcache->dev_page_size - page_offset, buffer,
                avail_bytes);
    buffer += avail_bytes;

    if (write_page)
    {
      result = fs_device_write_page (pcache->dev, page_nbr, pcache->pages[i],
                                     FS_FOP_USER);
      if (result != FS_DEVICE_RESULT_SUCCESS)
      {
        FS_ERR_FATAL ("[%d] OTP write failed", 0, 0, 0);
      }
      pcache->status[i].is_for_write = 0;
    }
  }
}


void
fs_otp_pcache_write_changed_pages (struct fs_otp_pcache *pcache)
{
  int i, result;

  for (i = 0; i < FS_OTP_MAX_PCACHE_PAGES; ++i)
  {
    if (pcache->status[i].is_in_use && pcache->status[i].is_for_write)
    {
      result = fs_device_write_page (pcache->dev, pcache->status[i].page_nbr,
                                     pcache->pages[i], FS_FOP_USER);
      if (result != FS_DEVICE_RESULT_SUCCESS)
      {
        FS_ERR_FATAL ("[%d] OTP write failed", 0, 0, 0);
      }
      pcache->status[i].is_for_write = 0;
    }
  }

}

#endif /* FEATURE_EFS_OTP */
