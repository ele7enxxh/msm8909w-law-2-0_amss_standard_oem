/***********************************************************************
 * fs_device_read_pointer_cache.c
 *
 * Device layer read-pointer API cache.
 * Copyright (C) 2012-2013 QUALCOMM Technologies, Inc.
 *
 * NOR flash is direct-memory mapped and the PM is designed to make use of
 * this through the read-pointer API that returns the address in flash to
 * the page making it efficient to read only required bytes from flash.
 * SPI-NOR flash does not support this API but uses the same PM. This
 * device layer cache simulates read-pointer operation for SPI-NOR flash by
 * caching in the page and returning address to the cached buffer.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_device_read_pointer_cache.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2012-12-02   dks   Create

===========================================================================*/

#include "fs_config_i.h"
#include "fs_util.h"
#include "fs_device_read_pointer_cache.h"
#include "assert.h"
#include <string.h>

#ifdef FEATURE_EFS_EFS2_ON_SPI_NOR

/* Device Read pointer Cache structure. When the read_pointer operation is not
   supported by the underlying NOR device, we read the entire page and return
   pointer to buffers in the cache, to make it appear to the PM as though
   read_pointer API succeeded and returned address of the requested page in
   flash. By caching the data we avoid reading the same page if different
   offsets in the same page were requested to be read multiple times. */
struct fs_device_read_pointer_cache_node
{
  uint8 *buff;
  fs_page_id page;
};

/* The number of buffers for the read-pointer cache has been chosen based on
   the number and pattern of reads done by NOR PM. There are two primary places
   where read-pointer is used heavily. During initialization to collect logs
   for replaying and during Garbage collection to move pages from GC-Block to
   Allocator Block.

   During initialization, while finding log pages, rptable page is read to find
   out if a page is a log page and if it is, the actual log page's sequence
   number is read to find out if it is a good log. With a 2-way cache, this can
   be made efficient to not read the rptable page multiple times.

   During Garbage Collection, status of page being GC'ed is read form the
   rptable page so it can be moved to the allocator block. To allocate a new
   cluster, the rptable page for allocator block is now read. This happens in
   tandem for all pages in the GC Block. A 2-way cache can reduce multiple
   reads to rptable pages of both GC and Allocator blocks.

   The following APIs are not meant to be used for a generic cache. So do not
   change the value the number of buffers for use with these APIs. */
#define FS_READ_PTR_CACHE_NUM_BUFFS 2
#if FS_READ_PTR_CACHE_NUM_BUFFS != 2
  #error "Read pointer cache API only works correctly with 2 buffers."
#endif

/* Buffers for the cached data. */
FS_ALIGN static uint8
   read_pointer_cache_buff [FS_READ_PTR_CACHE_NUM_BUFFS][FS_MAX_PAGE_SIZE];

struct fs_device_read_pointer_cache_type
{
  struct fs_device_read_pointer_cache_node
             read_cache[FS_READ_PTR_CACHE_NUM_BUFFS];
  uint8 last_used_index;     /* Index that was last used */
};
static struct fs_device_read_pointer_cache_type fs_device_read_pointer_cache;

void
fs_device_read_pointer_cache_init (void)
{
  uint32 i;
  memset (read_pointer_cache_buff, 0, sizeof (read_pointer_cache_buff));
  memset (&fs_device_read_pointer_cache, 0,
                                 sizeof (fs_device_read_pointer_cache));

  /* While selecting new index, we return the index that was not last used. So
     start with 1 as last used index to chose 0. */
  fs_device_read_pointer_cache.last_used_index =
                                      (FS_READ_PTR_CACHE_NUM_BUFFS - 1);

  for (i = 0; i < FS_READ_PTR_CACHE_NUM_BUFFS; i++)
  {
    fs_device_read_pointer_cache.read_cache[i].buff =
                                              read_pointer_cache_buff[i];
    fs_device_read_pointer_cache.read_cache[i].page = FS_INVALID_PAGE_ID;
  }
}

void *
fs_device_read_pointer_cache_find_page (fs_page_id page)
{
  uint32 i;
  void *buff = NULL;

  for (i = 0; i < FS_READ_PTR_CACHE_NUM_BUFFS; i++)
  {
    if (fs_device_read_pointer_cache.read_cache[i].page == page)
    {
      buff = fs_device_read_pointer_cache.read_cache[i].buff;
      fs_device_read_pointer_cache.last_used_index = i;
    }
  }
  return buff;
}

void
fs_device_read_pointer_cache_update_page_data (fs_page_id page, void *new_data,
                                               fs_off_t offset, fs_size_t size)
{
  void *buff;
  uint32 max_copy_size;

  ASSERT (new_data != NULL);
  ASSERT ((offset + size) <= FS_MAX_PAGE_SIZE);

  buff = fs_device_read_pointer_cache_find_page (page);
  if (buff == NULL)
    return;

  max_copy_size = FS_MAX_PAGE_SIZE - offset;
  fs_memscpy (((byte *)buff) + offset, max_copy_size, new_data, size);
}

void *
fs_device_read_pointer_cache_find_empty_buff (fs_page_id page)
{
  uint8 new_index = !(fs_device_read_pointer_cache.last_used_index);
  fs_device_read_pointer_cache.read_cache[new_index].page = page;
  fs_device_read_pointer_cache.last_used_index = new_index;
  return fs_device_read_pointer_cache.read_cache[new_index].buff;
}

void
fs_device_read_pointer_cache_invalidate_cache (void)
{
  uint32 i;
  for (i = 0; i < FS_READ_PTR_CACHE_NUM_BUFFS; i++)
  {
    fs_device_read_pointer_cache.read_cache[i].page = FS_INVALID_CLUSTER_ID;
    fs_device_read_pointer_cache.last_used_index =
                                          (FS_READ_PTR_CACHE_NUM_BUFFS - 1);
  }
}

void
fs_device_read_pointer_cache_invalidate_page (fs_page_id page)
{
  uint32 i;
  for (i = 0; i < FS_READ_PTR_CACHE_NUM_BUFFS; i++)
  {
    if (fs_device_read_pointer_cache.read_cache[i].page == page)
    {
      fs_device_read_pointer_cache.read_cache[i].page = FS_INVALID_PAGE_ID;
      fs_device_read_pointer_cache.last_used_index = !(i);
    }
  }
}

#endif /* FEATURE_EFS_EFS2_ON_SPI_NOR */
