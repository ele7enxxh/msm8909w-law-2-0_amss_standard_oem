/***********************************************************************
 * fs_device_read_pointer_cache.h
 *
 * Device layer read-pointer API cache.
 * Copyright (C) 2012 Qualcomm Technologies, Inc.
 *
 * APIs to use device layer read-pointer operation cache.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_device_read_pointer_cache.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-12-02   dks   Create

===========================================================================*/

#ifndef __FS_DEVICE_READ_POINTER_CACHE_H__
#define __FS_DEVICE_READ_POINTER_CACHE_H__

#include "fs_config_i.h"
#include "fs_device_types.h"
#include "fs_sys_types.h"

#ifdef FEATURE_EFS_EFS2_ON_SPI_NOR

/* Initialize the cache */
void fs_device_read_pointer_cache_init (void);

/* Search for a 'page' in the cache and return address to the buffer
   containing page data. */
void * fs_device_read_pointer_cache_find_page (fs_page_id page);

/* Find the most suitable buffer in the cache to put new data into and return
   address to that buffer. */
void *fs_device_read_pointer_cache_find_empty_buff (fs_page_id page);

/* Update the page data if present to new data. */
void fs_device_read_pointer_cache_update_page_data (fs_page_id page,
                           void *new_data, fs_off_t offset, fs_size_t size);

/* Invalidate the cache data. Should be used after an erase operation. */
void fs_device_read_pointer_cache_invalidate_cache (void);

/* Invalidate buffer containing specified page if present in cache. */
void fs_device_read_pointer_cache_invalidate_page (fs_page_id page);

#else /* !FEATURE_EFS_EFS2_ON_SPI_NOR */

static inline void
fs_device_read_pointer_cache_init (void)
{
  return;
}

static inline void *
fs_device_read_pointer_cache_find_page (fs_page_id page)
{
  (void) page;
  return NULL;
}

/* Finds the most suitable buffer in the cache to put new data into and returns
   address to the buffer. */
static inline void *
fs_device_read_pointer_cache_find_empty_buff (fs_page_id page)
{
  (void) page;
  return NULL;
}

/* Invalidate the buffer contents containing a certain page. */
static inline void
fs_device_read_pointer_cache_update_page_data (fs_page_id page, void *new_data,
                                               fs_off_t offset, fs_size_t size)
{
  (void) page;
  (void) new_data;
  (void) offset;
  (void) size;
  return;
}

/* Invalidate the cache data. Can be used after an erase operation. */
static inline void
fs_device_read_pointer_cache_invalidate_cache (void)
{
  return;
}

/* Invalidate buffer containing specified page if present in cache. */
static inline void
fs_device_read_pointer_cache_invalidate_page (fs_page_id page)
{
  (void) page;
  return;
}

#endif /* FEATURE_EFS_EFS2_ON_SPI_NOR */

#endif /* not __FS_DEVICE_READ_POINTER_CACHE_H__ */
