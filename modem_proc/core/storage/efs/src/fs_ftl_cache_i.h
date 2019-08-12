#ifndef __FTL_CACHE_I_H__
#define __FTL_CACHE_I_H__
/******************************************************************************
 * fs_ftl_cache_i.h
 *
 * This file declares the internal data and functions used by the FTL cache.
 *
 * Copyright (C) 2006-2010 QUALCOMM Technologies, Inc.  All Rights Reserved
 *
 *****************************************************************************/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_ftl_cache_i.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2010-09-01   wek     Remove compile time calculations based on page size.
2010-07-30   wek     Dynamically allocate all buffers dependent on page size.
2010-05-19   vr      Bugfix to make FTL_FLASH_MAX_PAGE_SIZE equal EFS page_size
2010-02-08   rp      CMI compliance, remove #ifdef from API files.
2009-06-12   ebb     Moved rex_xxx calls to fs_osal.h abstraction.
2009-04-01   yg      Featurization cleanup related changes.
2008-04-28   np      Added support for multiple partition in ftl.
2007-09-13   sch     Moved definition of MAX_DEV_PAGES_PER_BLOCK to fs_config.h
2007-07-25   umr     Added support for 4k Page size.
2007-05-10   yg      Modified configs so these can be overrided from custefs.h.
2006-11-30   yg      Change to enable overriding default cache page count.
2006-09-28   yg      Changed age field to uint32.
2006-09-16   yg      Fixed compilation issue for non FTL enabled builds.
2006-08-31   dlb     Fix RCS header.
2006-07-21   yg      Initial file for FTL cache layer
=============================================================================*/

/*
 *    SECTOR : Is a 512 byte page unit.
 *    Page   : Is the smallest writable unit of the flash device (can be 2k).
 *
 */
#include "fs_osal.h"
#include "fs_ftl_cache.h"
#include "fs_ftl_i.h"
#include "fs_config_i.h"

#ifdef FEATURE_EFS_FTL

/* This is in order to support max 2k page devices by default */
#ifdef FS_FTL_FLASH_MAX_PAGE_SIZE
  #define FTL_FLASH_MAX_PAGE_SIZE            FS_FTL_FLASH_MAX_PAGE_SIZE
#else
  #define FTL_FLASH_MAX_PAGE_SIZE            FS_MAX_PAGE_SIZE
#endif

/* If a bigger device with more pages per block need to be supported
 * Then define this following macro appropriately in the config file. */
#ifdef FS_FTL_FLASH_MAX_BLOCK_SIZE
  #define FTL_FLASH_MAX_BLOCK_SIZE            FS_FTL_FLASH_MAX_BLOCK_SIZE
#else
  #define FTL_FLASH_MAX_BLOCK_SIZE            64
#endif

#define FTL_MAX_SECTORS_PER_BLOCK   (FTL_FLASH_MAX_PAGE_SIZE * \
                                     FTL_FLASH_MAX_BLOCK_SIZE / \
                                     DEFAULT_MS_SECTOR_SIZE)

/*
 * For full explanation refer to the FTL design document 80-VD625-1_A1.pdf
 *
 * The number of cached pages controls how many pages can be cached. This
 * number can be varied depending on the size of the underlying flash part's
 * physical block. It is recommended to have one full physical block worth of
 * cached pages plus some more cached pages so that any FAT updates or sector
 * updates would have some cache room available. This would increase the
 * performance of the overall system, since otherwise a flush would be
 * performed if a write needs a free cache page and enough free cache pages
 * are not available.
 *
 *  Recommended value for the number of cached pages = PB + (2 * FAT)
 * where
 * PB = Cache page count equal to Flash physical block
 * FAT = Cache page count equal to maximum possible cluster size for FAT for
 * the partition or atleast 50% of block size for small block device */
#ifdef FS_FTL_CACHE_PAGE_COUNT
  #define FTL_CACHE_PAGE_COUNT     FS_FTL_CACHE_PAGE_COUNT
#else
  /* This could be something const or relative size of the total cache size */
  #define MAX_ADDITIONAL_SECTOR_CACHE_COUNT      32


  #define FTL_CACHE_PAGE_COUNT  (MAX_ADDITIONAL_SECTOR_CACHE_COUNT \
                                 + FTL_MAX_SECTORS_PER_BLOCK)
#endif

#define MAX_CACHE_BLOCKS                 4

#define INVALID_BLOCK                    0xFFFF

/* How many maximum buffers FTL can handle. Since count is not passed
 * and the length is determined by the invalid identifier, we need
 * atlest 1 more than what FTL can really handle */
#define MAX_FTL_BUFFER_LIST      (FTL_MAX_SECTORS_PER_BLOCK + 1)

typedef struct __cached_page_type
{
  struct __cached_page_type * next;

  uint32   sector_num;
  uint8    dirty;
  uint8    attrib;
  uint16   dummy;
  uint8    buffer [DEFAULT_MS_SECTOR_SIZE];
}
cached_page_type;

typedef struct
{
  uint16  block_number;
  uint16  sector_count;
  uint32  dirty;
  uint32  age;
  cached_page_type * cached_pages;
}
block_cache_type;

typedef struct{
  /*
   *     Cached pages pool: the actual cache pages. Align it so that if the
   *     access to these buffers involves DM, then 32 byte misaligned buffers
   *     do not work or work slow because of unnecessary memcpy.
  **/
  cached_page_type   *cache_pages_pool;

  /* ftl raw_device handle.
  **/
  ftl_raw_device_t ftl_raw_device;
  /*
   *    Cached block: holders for the cached pages with block number tag.
  **/
  block_cache_type   cached_block[MAX_CACHE_BLOCKS];

  /*
   *    Free pool, which can be used to instantaneously access the free page
  **/
  cached_page_type *free_cache_pool;

  /*
   *   Do we have the poll timer active? while writing we can make sure we
   *   start the poll timer and after syncing everything, we can turn off the
   *   timer.
  **/
  uint8 sync_timer_active;

  /*
   *    Maximum number of sectors available in FTL device externally
  **/
  uint32 ftl_dev_sector_count;

  /*
   *    Array of buffer pointers to pass to FTL write function.
  **/
  cached_buffer_list_type cached_ftl_buffers[MAX_FTL_BUFFER_LIST];

  /*
   *   Running age to track the oldest block available in the Cache
  **/
  uint32 oldest_block_age;

  /*  This can be used to monitor the number of writes happened since last
   *  poll. If we want to flush if no activity this can be used */
   uint32 write_count_since_last_sync_poll;

  /*
   *   Since sync will be done in a different task, so we need to handle
   *    synchronization between them.
  **/
  fs_os_crit_sect_type fs_ftl_cache_crit_sec;

} ftl_cache_data;

#define INVALID_CACHE_PAGE             ((cached_page_type *)0)
#define INVALID_BLOCK_CACHE            ((block_cache_type *)0)

#define CACHED_PAGE_IS_VALID(x)        ((x) != INVALID_CACHE_PAGE)
#define CACHED_PAGE_IS_NOT_VALID(x)    ((x) == INVALID_CACHE_PAGE)

#define BLOCK_CACHE_IS_VALID(x)        ((x) != INVALID_BLOCK_CACHE)
#define BLOCK_CACHE_IS_NOT_VALID(x)    ((x) == INVALID_BLOCK_CACHE)

#endif /* FEATURE_EFS_FTL */

#endif /* __FTL_CACHE_I_H_ */

