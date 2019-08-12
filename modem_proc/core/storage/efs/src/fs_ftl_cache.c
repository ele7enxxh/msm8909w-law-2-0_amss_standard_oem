/******************************************************************************
 * fs_ftl_cache.c
 *
 * This file implements the cache Layer for the FTL to improve performance of
 *   FS + FTL layer.
 *
 * Copyright (C) 2006-2013 QUALCOMM Technologies, Inc.  All Rights Reserved
 *
 *****************************************************************************/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_ftl_cache.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2013-10-09   nr      Replace banned memory functions with safer versions.
2012-03-20   wek     Cleanup customer header file inclusion.
2012-03-08   wek     Rename macros and remove header files to compile for boot.
2011-06-01   dks     Migrate from TMC to rcinit.
2010-12-22   wek     Remove FTL from memory allocation from the heap.
2010-10-06   rp      Initialize the FTL Sync timer before using it.
2010-07-30   wek     Dynamically allocate all buffers dependent on page size.
2010-05-10   rp      Removed inclusion of EFS1 deprecated files.
2010-04-28   vr      Relocate fs_task from EFS1 to EFS2
2009-06-12   ebb     Moved rex_xxx calls to fs_osal.h abstraction.
2009-04-01   yg      Featurization cleanup related changes.
2008-11-03   rp      Introduce device abstraction.
2008-05-27   yg      ftl_cache_init initialize only for the device.
2008-04-28   np      Added support for multiple FTL partitions.
2008-03-06   yg      Included fs_ftl.h header file.
2007-08-17   yg      Used new align macro.
2007-05-10   yg      Catch mis configured constant well ahead.
2007-02-05   yg      Fixed Compiler warning.
2006-11-27   yg      Aligned buffers to 32 byte for interaction with DM.
2006-11-22   yg      Corrected feature comment.
2006-10-11   yg      Added counters support.
2006-10-10   yg      Don't force periodic cache flush.
2006-09-28   yg      Changed age & its handling to uint32 range.
2006-09-20   dlb     Lint cleanup.
2006-09-14   sh      Remove unused Benchmark hook.
2006-08-31   dlb     Fix RCS header.
2006-04-10   yg      Initial file for FTL Cache layer
=============================================================================*/

#include "comdef.h"
#include "fs_config_i.h"
#include "fs_err.h"
#include "fs_util.h"

#include <string.h>

#ifdef FEATURE_EFS_FTL

#include "assert.h"
#include "fs_ftl.h"
#include "fs_sys_types.h"
#include "fs_ftl_cache.h"
#include "fs_ftl_cache_i.h"
#include "fs_osal.h"
#include "fs_task_i.h"

#include "fs_device_ftl_flash.h"
#include "fs_sys_types.h"

#include "task.h"


#define FS_FTL_SYNC_TIME      5000


/*   Convert Sector number to block number based on the number of
 *   512 byte pages that a physical block can accomodate.
 */
#define SECTOR_TO_PHY_BLK(x) ((x) / ftl_dev_sectors_per_block)

/*
 *   Number of sectors (512 byte) in a physical block of flash device
**/
static uint32 ftl_dev_sectors_per_block;

/* The ftl caching information for each partition is stored below.*/
static ftl_cache_data cpartitions[MAX_FTL_PARTITIONS];

/*
 *     Cached pages pool: the actual cache pages. Align it so that if the
 *     access to these buffers involves DM, then 32 byte misaligned buffers
 *     do not work or work slow because of unnecessary memscpy.
**/
static FS_ALIGN cached_page_type
       cached_pool_buffer[MAX_FTL_PARTITIONS][FTL_CACHE_PAGE_COUNT];


/* FTL Cache Sync timer. */
static fs_os_timer_type fs_ftl_sync_timer;


/******************************************************************************
*
*      Local private function prototypes
*
******************************************************************************/

static cached_page_type * lookup_sector_in_cache (ftl_cache_data *pt,
                                                  uint32 sector);
static block_cache_type * lookup_block_in_cache (ftl_cache_data *pt,
                                                 uint32 block);
static cached_page_type * lookup_sector_in_cache_list (uint32 sector,
                                       cached_page_type *cache_list);
static cached_page_type * get_free_cache_page (ftl_cache_data *pt);

static cached_page_type * insert_page_into_list (cached_page_type *srcpagelst,
                                           cached_page_type *dest_page_list);
static block_cache_type * alloc_cached_block (ftl_cache_data *pt);
static int flush_block_cache (ftl_cache_data *pt, block_cache_type *blkcache);
static int add_page_to_cache (ftl_cache_data *pt, cached_page_type *new_page);
static uint32 cache_sync_oldest_block (ftl_cache_data *pt);
static boolean compare_less_than (uint32 x, uint32 y);


/*===========================================================================

FUNCTION FS_FTL_SYNC_TIMER_INIT

DESCRIPTION
   Initialize and start the FTL sync timer.

===========================================================================*/
void
fs_ftl_sync_timer_init (void)
{
  fs_os_def_timer (&fs_ftl_sync_timer, fs_os_self (), FS_FTL_SYNC_SIG);

  fs_ftl_start_sync_timer ();
}

/*===========================================================================

FUNCTION FS_FTL_START_SYNC_TIMER

DESCRIPTION
   Start the FTL cache sync timer to poll to check if no write activity
   is found since last poll, it can sync the cache contents into flash.

===========================================================================*/
void
fs_ftl_start_sync_timer (void)
{
  (void) fs_os_set_timer (&fs_ftl_sync_timer, FS_FTL_SYNC_TIME);
}

/*===========================================================================

FUNCTION FS_FTL_START_SYNC_TIMER

DESCRIPTION
   Handle the sinal received from the FTL cache sync timer

===========================================================================*/
void
fs_ftl_sync_timer_signal_received (void)
{
  (void) fs_os_clr_sigs (fs_os_self(), FS_FTL_SYNC_SIG);

  if (ftl_cache_periodic_sync () != 0)
  {
    fs_ftl_start_sync_timer ();
  }
}




/******************************************************************************
FUNCTION      ftl_cache_init

DESCRIPTION   Initialize the FTL Cache layer. This will reinitialize the cache
              buffers losing the contents that are already cached. So the
              cache should be sync'd before initializing.

PARAMETERS

RETURN VALUE
******************************************************************************/
int
ftl_cache_init (int16 driveno)
{
  ftl_cache_data *pt;

  ASSERT (driveno >= 0);
  if (driveno >= MAX_FTL_PARTITIONS)
    return -1;

  pt = &cpartitions[driveno];

  /* Initialize the critical section and the cache buffers for the drive */
  fs_os_init_crit_sect (&pt->fs_ftl_cache_crit_sec);
  pt->ftl_raw_device = FTL_NO_DEVICE;
  pt->cache_pages_pool = cached_pool_buffer[driveno];

  return 0;
}

/******************************************************************************
FUNCTION      ftl_cache_open

DESCRIPTION   Open a cached FTL partition device. Cache should have
              initialized already before calling open.

PARAMETERS
              driveno  : ftl drive number. Drive number 0 usaually notes ftl
              partition path "0:FTL".

RETURN VALUE  NULL on failure.
******************************************************************************/
ftl_cached_device_t
ftl_cache_open (int16 driveno)
{
  uint32 index, blk_cnt, pg_per_blk;
  cached_page_type *previous_cache_page;
  uint16 pg_sz;
  ftl_cache_data *pt;

  ASSERT (driveno >= 0);
  if (driveno >= MAX_FTL_PARTITIONS)
    return FTL_CACHED_NO_DEVICE;

  pt = &cpartitions[driveno];
  fs_os_enter_crit_sect (&pt->fs_ftl_cache_crit_sec);
  if (pt->ftl_raw_device != FTL_NO_DEVICE) {
    /* The device is already in use.*/
    fs_os_leave_crit_sect (&pt->fs_ftl_cache_crit_sec);
    return FTL_CACHED_NO_DEVICE;
  }

  /* Initialize lower FTL layer */
  pt->ftl_raw_device = ftl_raw_open (driveno);
  fs_os_leave_crit_sect (&pt->fs_ftl_cache_crit_sec);

  /* Initialize lower FTL layer */
  if (pt->ftl_raw_device == FTL_NO_DEVICE)
  {
    return FTL_CACHED_NO_DEVICE;
  }

  /* Get FTL device level constants for cache usage configuration */
  if (ftl_get_raw_dev_params (pt->ftl_raw_device, &blk_cnt, &pg_per_blk,
                             &pg_sz, &pt->ftl_dev_sector_count) == FTL_FAILURE)
  {
    return FTL_CACHED_NO_DEVICE;
  }

  /* calculate how many sectors are needed to cache 1 physical flash block */
  ftl_dev_sectors_per_block = pg_per_blk * (pg_sz / DEFAULT_MS_SECTOR_SIZE);

  /* If the configuration is set wrong for the current device fail now */
  if (ftl_dev_sectors_per_block > MAX_FTL_BUFFER_LIST)
  {
    FS_ERR_FATAL("Misconfigured constant for this flash device", 0, 0, 0);
  }

  /* Initialize our cached pages and place them in free pool */
  previous_cache_page = INVALID_CACHE_PAGE;
  for (index = 0; index < FTL_CACHE_PAGE_COUNT; ++index)
  {
    pt->cache_pages_pool[index].dirty = 0;
    pt->cache_pages_pool[index].sector_num = INVALID_PAGE;
    pt->cache_pages_pool[index].next = previous_cache_page;

    previous_cache_page = &pt->cache_pages_pool[index];
  }
  pt->free_cache_pool = previous_cache_page;

  /* Age will start now */
  pt->oldest_block_age = 0;

  /* Initialize the block holders */
  for (index = 0; index < MAX_CACHE_BLOCKS; ++index)
  {
    pt->cached_block[index].dirty         = 0;
    pt->cached_block[index].sector_count  = 0;
    pt->cached_block[index].age           = 0;
    pt->cached_block[index].block_number  = INVALID_BLOCK;
    pt->cached_block[index].cached_pages  = INVALID_CACHE_PAGE;
  }

  /* Initialize our polling and sync routine variables */
  pt->write_count_since_last_sync_poll = 0;
  pt->sync_timer_active = 0;

  return (ftl_cached_device_t) pt;
}

/**********************************************************************
FUNCTION      fs_ftl_cache_close

DESCRIPTION   Clear all information related a specific partition.

PARAMETERS
              dev       : FTL cached partition device handler.
RETURN VALUE
******************************************************************************/
void
fs_ftl_cache_close (ftl_cached_device_t dev)
{
  ftl_cache_data *pt;

  ASSERT (dev != FTL_CACHED_NO_DEVICE);
  pt = (ftl_cache_data *) dev;
  fs_os_enter_crit_sect (&pt->fs_ftl_cache_crit_sec);
  ftl_raw_close (((ftl_cache_data *)dev)->ftl_raw_device);
  pt->ftl_raw_device = FTL_CACHED_NO_DEVICE;
  fs_os_leave_crit_sect (&pt->fs_ftl_cache_crit_sec);
}

/******************************************************************************
FUNCTION      ftl_cache_read

DESCRIPTION   Read specified number of sectors from mentioned sector onwards.
              Get the contents from either cache or directly from flash
              depending on the availability.

PARAMETERS
          dev       : FTL cached partition device handler.
          sector    : Starting Sector number to read from.
          buffer    : buffer where read data has to be placed.
          count     : Number of sectors to read starting from mentioned sector.

RETURN VALUE
              0 : Success
     Error code : Failure
******************************************************************************/
int
ftl_cache_read (ftl_cached_device_t dev, uint32 sector, byte *buffer,
                uint16 count)
{
  cached_page_type *temp_cached_page;
  ftl_cache_data *pt;

  ASSERT (dev != FTL_CACHED_NO_DEVICE);
  pt = (ftl_cache_data *) dev;
  /* First do some sanity check */
  if ((sector >= pt->ftl_dev_sector_count)
      || ((sector + count) > pt->ftl_dev_sector_count))
  {
    return FTL_FAILURE;
  }

  ftl_device_read_counter (sector, count, FTL_OP_CACHE);

  /* Enter critical section */
  fs_os_enter_crit_sect (&pt->fs_ftl_cache_crit_sec);

  /*   First let's read everything from FTL assuming we don't have
   *   anything in cache.
   */
  ftl_raw_read(pt->ftl_raw_device, sector, buffer, count);

  /*   Now go on looking for the sectors one at a time until we searched for
   *   all and overwrite if we have latest copy of the sector in cache
   */
  while (count > 0)
  {
    /* Check if we can find this particular sector in cache */
    temp_cached_page = lookup_sector_in_cache (pt, sector);

    /* If we found the cache page, then copy it to the buffer */
    if (CACHED_PAGE_IS_VALID(temp_cached_page))
    {
      fs_memscpy (buffer, DEFAULT_MS_SECTOR_SIZE, temp_cached_page->buffer,
                  DEFAULT_MS_SECTOR_SIZE);
    }

    /* update for next sector search */
    buffer += DEFAULT_MS_SECTOR_SIZE;
    --count;
    ++sector;
  }

  /* Release lock */
  fs_os_leave_crit_sect (&pt->fs_ftl_cache_crit_sec);

  return FTL_SUCCESS;
}

/******************************************************************************
FUNCTION      ftl_cache_write

DESCRIPTION   Cached Write multiple logical sectors starting from
              the specified one.

PARAMETERS
          dev      : FTL cached partition device handler.
          sector   : Starting Sector number to write to.
          buffer   : buffer where new data to be written is available.
          count    : Number of sectors to write starting from mentioned sector.

RETURN VALUE  Success / Failure
******************************************************************************/
int
ftl_cache_write (ftl_cached_device_t dev, uint32 sector, byte *buffer,
                 uint16 count)
{
  cached_page_type *temp_cached_page;
  uint32 got_new_page;
  ftl_cache_data *pt;

  ASSERT (dev != FTL_CACHED_NO_DEVICE);
  pt = (ftl_cache_data *) dev;

  /* First do some sanity check */
  if ((sector >= pt->ftl_dev_sector_count)
      || ((sector + count) > pt->ftl_dev_sector_count))
  {
    return FTL_FAILURE;
  }
  ftl_device_write_counter (sector, count, FTL_OP_CACHE);

  /* Enter critical section */
  fs_os_enter_crit_sect (&pt->fs_ftl_cache_crit_sec);

  if (pt->sync_timer_active == 0)
  {
    fs_ftl_start_sync_timer ();
    pt->sync_timer_active = 1;
  }

  pt->write_count_since_last_sync_poll++;

  temp_cached_page  = INVALID_CACHE_PAGE;

  while (count > 0)
  {
    got_new_page = 0;

    /* Check if we have this already cached */
    temp_cached_page = lookup_sector_in_cache (pt, sector);
    if (CACHED_PAGE_IS_NOT_VALID (temp_cached_page))
    {
      /* So its not cached so far allocate a new page */
      temp_cached_page = get_free_cache_page (pt);

      if (CACHED_PAGE_IS_NOT_VALID (temp_cached_page))
      {
        /* Do something to sync and free up the cache */
        FS_ERR_FATAL("Ran out of cache pages...??%d, %d, %d\n", 0, 0, 0);
      }

      /* With this we can add this page into cache */
      got_new_page = 1;

      /* Update its contents to represent this sector */
      temp_cached_page->sector_num = sector;
      temp_cached_page->dirty = 1;
    }

    /* Copy new data into the cached sector buffer */
    fs_memscpy (temp_cached_page->buffer, DEFAULT_MS_SECTOR_SIZE, buffer,
                DEFAULT_MS_SECTOR_SIZE);

    /* If we allocated new page then add it into the cache */
    if (got_new_page != 0)
    {
      add_page_to_cache (pt, temp_cached_page);
    }

    /* Update to process the next sector */
    ++sector;
    --count;
    buffer += DEFAULT_MS_SECTOR_SIZE;
  }

  /* Release lock */
  fs_os_leave_crit_sect (&pt->fs_ftl_cache_crit_sec);

  return FTL_SUCCESS;
}

/******************************************************************************
FUNCTION      ftl_cache_periodic_sync

DESCRIPTION   Do periodic sync. This call will sync one oldest cached block
              and return if there are more blocks to sync.


RETURN VALUE
              0 : If no more blocks are cached.
              1 : If there are more blocks in cache need to be flushed.
******************************************************************************/
int
ftl_cache_periodic_sync (void)
{
  int i;
  uint32 remaining_blocks_to_flush, c_remaining_blocks_to_flush;
  ftl_cache_data *pt;

  remaining_blocks_to_flush = 0;
  for (i = 0; i < MAX_FTL_PARTITIONS; i++) {
    pt = &cpartitions[i];
    if (pt->ftl_raw_device != FTL_NO_DEVICE) {
      fs_os_enter_crit_sect (&pt->fs_ftl_cache_crit_sec);
      if (pt->write_count_since_last_sync_poll == 0)
      {
        c_remaining_blocks_to_flush = cache_sync_oldest_block (pt);
        remaining_blocks_to_flush += c_remaining_blocks_to_flush;
        if (c_remaining_blocks_to_flush == 0) {
          pt->sync_timer_active = 0;
        }
      }
      else
      {
      /* We had write after our last poll, so let's just reset this
        * and continue polling */
        pt->write_count_since_last_sync_poll = 0;

      /* returning back telling we have some blocks to flush, so
        * that the timer is started again to check and sync later */
        remaining_blocks_to_flush = 1;
      }
      fs_os_leave_crit_sect (&pt->fs_ftl_cache_crit_sec);
    }
  }

  if (remaining_blocks_to_flush > 0)
    return 1;
  else
    return 0;
}

/******************************************************************************
FUNCTION      ftl_cache_force_sync_all

DESCRIPTION
            Sync all the cache contents to flash. This would be done before
            power down.

PARAMETERS
          None

RETURN VALUE
          0 : Success
******************************************************************************/
int ftl_cache_force_sync_all (void)
{
  int i;

  /* Sync until we have no more un sync'd cached blocks in memory  */

  for (i = 0; i < MAX_FTL_PARTITIONS; i++) {
    if (cpartitions[i].ftl_raw_device != FTL_NO_DEVICE) {
      ftl_cache_data *pt = &cpartitions[i];
      fs_os_enter_crit_sect (&pt->fs_ftl_cache_crit_sec);
      while (cache_sync_oldest_block (pt))
        ;
      fs_os_leave_crit_sect (&pt->fs_ftl_cache_crit_sec);
      pt->sync_timer_active = 0;

    }
  }

  return 0;
}

/******************************************************************************
FUNCTION      ftl_cache_get_size

DESCRIPTION   Gets the geometry of the FTL device.

PARAMETERS
          pt        : ftl cached partition internal data pointer.
          pblk_cnt  : Pointer to location to store the maximum available number
                        of Sectors in this device (# of pages in flash term).
          ppg_size  : Pointer to location to store the page size.

RETURN VALUE  0
******************************************************************************/
int
ftl_cache_get_size (ftl_cached_device_t dev, uint32 *pblk_cnt,
                    uint16 *ppg_size)
{

  ftl_cache_data *pt = (ftl_cache_data *) dev;

  ASSERT (pt != NULL);
  if (pblk_cnt)
  {
    *pblk_cnt = pt->ftl_dev_sector_count;
  }

  if (ppg_size)
  {
    *ppg_size = DEFAULT_MS_SECTOR_SIZE;
  }
  return FTL_SUCCESS;
}

/*=============================================================================
*
*              L o c a l    f u n c t i o n s
*
=============================================================================*/


/******************************************************************************
FUNCTION      compare_less_than

DESCRIPTION   Compare and check if the first parameter is less than second
              parameter after taking care of wrap around of integer.
              This function is customized for "BYTE ONLY", to make it
              compatible with other data type, change the parameter type
              and the range values to the respective first and last
              quarter range of the data type chosen.

PARAMETERS
          x, y    : uint8 values to compare with wrap around taken
                    into consideration.

RETURN VALUE
          Boolean value if x is less than y with wrap around taken care of.
******************************************************************************/

#define FIRST_QUARTER_END     0x40000000
#define LAST_QUARTER_START    0xC0000000

static boolean
compare_less_than (uint32 x, uint32 y)
{
  if (((x < FIRST_QUARTER_END) && (y >= LAST_QUARTER_START))
      || ((y < FIRST_QUARTER_END) && (x >= LAST_QUARTER_START)))
  {
    return y < x;
  }

  return x < y;
}

/******************************************************************************
FUNCTION      cache_sync_oldest_block

DESCRIPTION   Sync the oldest block around in the cache.

PARAMETERS    pt   : ftl cached partition internal data pointer.

RETURN VALUE  Remaining blocks to flush in the cache.
******************************************************************************/
static uint32
cache_sync_oldest_block (ftl_cache_data *pt)
{
  uint32 index, dirty_block_count = 0;

  block_cache_type *oldest_block_in_cache = INVALID_BLOCK_CACHE;

  for (index = 0; index < MAX_CACHE_BLOCKS; ++index)
  {
    if ((pt->cached_block[index].block_number != INVALID_BLOCK)
        && (pt->cached_block[index].sector_count > 0)
        && (pt->cached_block[index].dirty != 0)
        && CACHED_PAGE_IS_VALID (pt->cached_block[index].cached_pages))
    {
      ++dirty_block_count;

      if (BLOCK_CACHE_IS_NOT_VALID (oldest_block_in_cache)
          || (compare_less_than (pt->cached_block[index].age,
                                 oldest_block_in_cache->age)))
      {
        oldest_block_in_cache = &pt->cached_block[index];
      }
    }
  }

  if (BLOCK_CACHE_IS_VALID (oldest_block_in_cache))
  {
    flush_block_cache (pt, oldest_block_in_cache);
    --dirty_block_count;
  }

  return dirty_block_count;
}

/******************************************************************************
FUNCTION      lookup_sector_in_cache

DESCRIPTION   Lookup if the sector in question is available in block cache.

PARAMETERS
          pt        : ftl cached partition internal data pointer.
          sector    : Sector number.

RETURN VALUE
          Cached Page         : Success (sector available in the cache)
          INVALID_CACHE_PAGE  : Failure
******************************************************************************/
static cached_page_type*
lookup_sector_in_cache (ftl_cache_data *pt, uint32 sector)
{
  uint32 sector_blk;
  cached_page_type *temp_cached_page = INVALID_CACHE_PAGE;
  block_cache_type *temp_cached_block;

  /* Check if the block for this sector is cached */
  sector_blk = SECTOR_TO_PHY_BLK (sector);
  temp_cached_block = lookup_block_in_cache (pt, sector_blk);

  /* Found block in the cache..?? */
  if (BLOCK_CACHE_IS_VALID (temp_cached_block))
  {
    temp_cached_page =
      lookup_sector_in_cache_list (sector, temp_cached_block->cached_pages);
  }

  return temp_cached_page;
}

/******************************************************************************
FUNCTION      lookup_sector_in_cache_list

DESCRIPTION   Lookup if the sector in question is available in cache list.

PARAMETERS
          sector    : Sector number.
          cache_list: list to search in for the sector in question.

RETURN VALUE
          Cached Page         : Success (sector available in the list)
          INVALID_CACHE_PAGE  : Failure
******************************************************************************/
static cached_page_type*
lookup_sector_in_cache_list (uint32 sector, cached_page_type *cache_list)
{
  cached_page_type *temp_cached_page = cache_list;

  while (CACHED_PAGE_IS_VALID(temp_cached_page))
  {
    /*   All our lists are sorted lists, so if we are
     *   already out of range, then don't search further.
     */
    if (temp_cached_page->sector_num > sector)
    {
      return INVALID_CACHE_PAGE;
    }

    /* If we found what we wanted, just return it */
    if (temp_cached_page->sector_num == sector)
    {
      return temp_cached_page;
    }

    /* Get the next one in link */
    temp_cached_page = temp_cached_page->next;
  }

  /* We coudn't find it */
  return INVALID_CACHE_PAGE;
}

/******************************************************************************
FUNCTION      get_free_cache_page

DESCRIPTION
          Lookup if there is any free cache page that can be used.
          - If a free page is available, then return it.
          - If a free page is not available, then flush oldest block
          - After flushing the block, free page will be available, return that.

PARAMETERS
          pt         : ftl cached partition internal data pointer.

RETURN VALUE
          Cached Page         : Success, got a free cache page
          INVALID_CACHE_PAGE  : Failure
******************************************************************************/
static cached_page_type*
get_free_cache_page (ftl_cache_data *pt)
{
  cached_page_type *temp_cached_page;

  /* If our free pool doesn't have any page then we have to flush a block */
  if (CACHED_PAGE_IS_NOT_VALID (pt->free_cache_pool))
  {
    cache_sync_oldest_block (pt);
  }   /* No free pages were found in free pool */

  temp_cached_page = pt->free_cache_pool;

  if (CACHED_PAGE_IS_VALID(temp_cached_page))
  {
    pt->free_cache_pool = temp_cached_page->next;

    /* Clear off the link, we don't want to have any links */
    temp_cached_page->next = INVALID_CACHE_PAGE;
  }
  else
  {
    FS_ERR_FATAL("No free pages even after a flush. %d%d%d\n", 0, 0, 0);
  }

  return temp_cached_page;
}

/******************************************************************************
FUNCTION      lookup_block_in_cache

DESCRIPTION
          Check if we have a block assigned in cache.

PARAMETERS
          pt        : ftl cached partition internal data pointer.
          block     : block number to look for in the block cache.

RETURN VALUE
          Cached block         : Success, Block is availble in cache
          INVALID_BLOCK_CACHE  : Failure, block is NOT available in cache
******************************************************************************/
static block_cache_type*
lookup_block_in_cache (ftl_cache_data *pt, uint32 block)
{
  int index;
  block_cache_type *temp_cached_block = INVALID_BLOCK_CACHE;

  for (index = 0; index < MAX_CACHE_BLOCKS; ++index)
  {
    if (pt->cached_block[index].block_number == block)
    {
      temp_cached_block = &pt->cached_block[index];
      break;
    }
  }

  return temp_cached_block;
}

/******************************************************************************
FUNCTION      insert_page_into_list

DESCRIPTION
          Insert a page into the destination page list.

PARAMETERS
          src_page       : Source page to insert.
          dest_page_list : Destination page list to insert into.

RETURN VALUE
          Head of the destination list : It might have changed.
******************************************************************************/
static cached_page_type*
insert_page_into_list (cached_page_type *src_page,
                       cached_page_type *dest_page_list)
{
  cached_page_type *next_page, *prev_page;

  if (CACHED_PAGE_IS_NOT_VALID(src_page))
  {
    FS_ERR_FATAL("Invalid list to merge.. !!?? %d %d %d\n", 0, 0, 0);
  }

  prev_page = INVALID_CACHE_PAGE;
  next_page = dest_page_list;

  while (CACHED_PAGE_IS_VALID(next_page) &&
         (next_page->sector_num < src_page->sector_num))
  {
    prev_page = next_page;
    next_page = next_page->next;
  }

  src_page->next = next_page;

  if (CACHED_PAGE_IS_VALID(prev_page))
  {
    prev_page->next = src_page;
    return dest_page_list;
  }
  else
  {
    return src_page;
  }
}

/******************************************************************************
FUNCTION      alloc_cached_block

DESCRIPTION
          Allocate a cache block if available.
          If not available, flush a block (that is not being written now) then
            allocate the flushed block.

PARAMETERS
          pt   : ftl cached partition internal data pointer.

RETURN VALUE
          Cached block         : Success, got a free cached block
          INVALID_BLOCK_CACHE  : Failure
******************************************************************************/
static block_cache_type*
alloc_cached_block (ftl_cache_data *pt)
{
  int index;
  block_cache_type *temp_cached_block = INVALID_BLOCK_CACHE,
                   *blk_good_to_flush = INVALID_BLOCK_CACHE;

  ASSERT (pt != NULL);

  /* search for any NON dirty blocks with max number of sectors in it
   * This search will make sure we re use any of the recently flushed
   * full block, which is the normal case if we are getting huge volume
   * of writes into contiguous blocks.
   */
  for (index = 0; index < MAX_CACHE_BLOCKS; ++index)
  {
    if (pt->cached_block[index].dirty == 0)
    {
      if ((pt->cached_block[index].sector_count == 0)
          && (pt->cached_block[index].block_number == INVALID_BLOCK)
          && (CACHED_PAGE_IS_NOT_VALID (pt->cached_block[index].cached_pages)))
      {
        temp_cached_block = &pt->cached_block[index];
        break;
      }
    }
    /* If the block is dirty, then try to check if this is a good candidate
     * to flush in the event of we not having any free block caches */
    else
    {
      if (BLOCK_CACHE_IS_NOT_VALID (blk_good_to_flush) ||
          compare_less_than (pt->cached_block[index].age,
                             blk_good_to_flush->age))
      {
        blk_good_to_flush = &pt->cached_block[index];
      }
    }
  }

  /* Check if we got something from above search, then let's use it */
  if (BLOCK_CACHE_IS_VALID (temp_cached_block))
  {
    return temp_cached_block;
  }

  /*  Okay, we are here means we didn't find any NON dirty blocks out in the
   *  block cache. But we should have a block that is a good candidate for
   *  flushing.
   */

  if (BLOCK_CACHE_IS_NOT_VALID(blk_good_to_flush))
  {
    FS_ERR_FATAL("Didn't find any good candidate for flush?? %d%d%d\n",0,0,0);
  }

  if (flush_block_cache (pt, blk_good_to_flush) == 0)
  {
    return blk_good_to_flush;
  }

  return 0;
}

/******************************************************************************
FUNCTION      flush_block_cache

DESCRIPTION
          Flush the block cache passed in. If the pages in block cache are not
          completely contiguous then multiple FTL write will happen. Since we
          don't know if there are any free pages available to read and flush
          full block. Instead of slow multiple FTL writes.

          This routine will return the cache pages back into free pool
          and marks the block as unused.

PARAMETERS
          pt         : ftl cached partition internal data pointer.
          blk_cache  : Block cache to flush to flash.

RETURN VALUE
          0   : Success
          -1  : Failure
******************************************************************************/
static int
flush_block_cache (ftl_cache_data *pt, block_cache_type *blk_cache)
{
  uint32 count, start_sector;
  cached_page_type *temp_cached_page, *prev_page;

  ASSERT (pt != NULL);
  temp_cached_page = blk_cache->cached_pages;

  /* Some sanity check */
  if (CACHED_PAGE_IS_NOT_VALID (temp_cached_page)
      || (blk_cache->sector_count == 0)
      || (blk_cache->dirty == 0))
  {
    FS_ERR_FATAL("Cannot flush a block with no dirty pages... %d %d %d",0,0,0);
  }

  /*   Get all the pages available in the block into the buffer list in a way
   *   that FTL can understand to flush in one pass.
   */
  count = 0;
  while (CACHED_PAGE_IS_VALID (temp_cached_page))
  {
    /* Update the values into our array which is used to convey to FTL */
    pt->cached_ftl_buffers[count].sector_num = temp_cached_page->sector_num;
    pt->cached_ftl_buffers[count].data_ptr   = temp_cached_page->buffer;

    temp_cached_page = temp_cached_page->next;
    count++;
  }

  /* Another sanity check if we overflow */
  if (count >= MAX_FTL_BUFFER_LIST)
  {
    FS_ERR_FATAL("Insufficient write buffer array... %d %d %d\n", count,
                      MAX_FTL_BUFFER_LIST, 0);
  }

  /* Will be an indication of termination point */
  pt->cached_ftl_buffers[count].data_ptr = 0;
  pt->cached_ftl_buffers[count].sector_num = INVALID_PAGE;

  /* Another sanity check */
  if (count != blk_cache->sector_count)
  {
    FS_ERR_FATAL("count doesn't match...!? %d %d %d\n", 0, 0, 0);
  }

  /* Just give info as first sector in this block. The pages will be sorted
   * properly to their places with the information provided in the array
   * which we just prepared */
  start_sector = blk_cache->block_number * ftl_dev_sectors_per_block;

  /* Send it to FTL and let it handle writing */
  if (ftl_raw_write (pt->ftl_raw_device, start_sector, pt->cached_ftl_buffers,
                     count) != FTL_SUCCESS)
  {
    FS_ERR_FATAL("Write Failure... %d %d %d\n", 0, 0, 0);
  }

  /*   Now its cleanup time, just reinitialize all the pages that we just
   *   flushed and just link the whole chain back into free cache pool
   */
  temp_cached_page = blk_cache->cached_pages;
  prev_page = INVALID_CACHE_PAGE;

  /* Lets clear all the meta data in page buffers */
  while (CACHED_PAGE_IS_VALID(temp_cached_page))
  {
    temp_cached_page->dirty = 0;
    temp_cached_page->sector_num = INVALID_PAGE;
    prev_page = temp_cached_page;
    temp_cached_page = temp_cached_page->next;
  }

  /* According to above loop we will have the prev_page valid */
  if (CACHED_PAGE_IS_NOT_VALID (prev_page)){
    FS_ERR_FATAL ("Problem in chain..??%d %d %d\n", 0, 0, 0);
  }

  /* Insert the free pages back into free pool */
  prev_page->next = pt->free_cache_pool;
  pt->free_cache_pool = blk_cache->cached_pages;

  /* Clear values in the cache block */
  blk_cache->cached_pages = INVALID_CACHE_PAGE;
  blk_cache->block_number = INVALID_BLOCK;
  blk_cache->age = 0;
  blk_cache->sector_count = 0;
  blk_cache->dirty = 0;

  return 0;
}

/******************************************************************************
FUNCTION      add_page_to_cache

DESCRIPTION
          Add the newly written cached page into cache. This routine will
          - Add the page into appropriate block cache if available.
          - Otherwise allocates a new block cache if free block available.
          - If free block is not found, then flushes a block cache to
             make space for the new block data.

PARAMETERS
          pt               : ftl cached partition internal data pointer.
          new_cached_page  : New cached page to add to block cache.

RETURN VALUE
          0      : Success, added the page to cache.
          -1     : invalid cache page.
          Fatal  : Failure
******************************************************************************/
static int
add_page_to_cache (ftl_cache_data *pt,
                   cached_page_type *new_cached_page)
{
  uint32 sector_blk;
  block_cache_type *blk_cache;

  /* Sanity check */
  ASSERT (pt != NULL);
  if (CACHED_PAGE_IS_NOT_VALID(new_cached_page))
  {
    FS_ERR_FATAL("Invalid cached page to insert %d %d %d\n", 0, 0, 0);
  }

  /* Check if we have this block already cached */
  sector_blk = SECTOR_TO_PHY_BLK (new_cached_page->sector_num);
  blk_cache = lookup_block_in_cache (pt, sector_blk);

  if (BLOCK_CACHE_IS_NOT_VALID (blk_cache))
  {
    /* Need to allocate new block for this page */
    blk_cache = alloc_cached_block (pt);
    if (BLOCK_CACHE_IS_NOT_VALID (blk_cache))
    {
      FS_ERR_FATAL("Didn't get valid block cache...%d%d%d\n", 0, 0, 0);
    }

    /* Update this new block's parameters */
    blk_cache->block_number = sector_blk;
    blk_cache->age = pt->oldest_block_age++;
  }

  /* Insert this page list into the block */
  blk_cache->cached_pages =
    insert_page_into_list (new_cached_page, blk_cache->cached_pages);

  /* Update more params */
  blk_cache->dirty = 1;
  blk_cache->sector_count++;

  /* Check if the block is fully filled for an immediate flush */
  if (((new_cached_page->sector_num % ftl_dev_sectors_per_block)
        == (ftl_dev_sectors_per_block - 1))
      && (blk_cache->sector_count == ftl_dev_sectors_per_block))
  {
    /*  Flush if we have full block worth of data.
     */
    flush_block_cache (pt, blk_cache);
  }

  return 0;
}

#else
extern int __dont_complain_about_empty_file;
#endif /* FEATURE_EFS_FTL */

