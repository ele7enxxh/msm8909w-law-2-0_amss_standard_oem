/***********************************************************************
 * fs_counters.h
 *
 * This file holds the counters to track flash operations.
 * Copyright (C) 2008 - 2010, 2012-2013 QUALCOMM Technologies, Inc.
 *
 * This file exposes the counters to track the read/write/erase operations
 * on the flash.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/inc/fs_counters.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-02   dks   Add flash_counter reset function.
2012-11-28   nr    Implement EFS Profile framework.
2012-09-10   nr    Implement new FS Benchmarking scheme.
2012-04-03   rp    Code review comments for Separate EFS and Hotplug counters.
2012-03-14   rp    Separate EFS and Hotplug counters.
2010-06-25   dks   Added counters for partial-write, begin and suspend-erase.
2009-05-08   ebb   Renamed hotplug counters to blockdev counters.
2008-12-15   yg    Move the header file to internal src folder from public.
2008-12-12   rp    Move counters from fs_device to this rightful place.
2008-05-23   rp    Create

===========================================================================*/

#ifndef __FS_COUNTERS_H__
#define __FS_COUNTERS_H__


#include "comdef.h"

/* NAND and NOR operations */
#define fs_flash_write_page_counter fs_flash_counter.flash_write_page_counter
#define fs_flash_read_page_counter fs_flash_counter.flash_read_page_counter
#define fs_flash_erase_block_counter fs_flash_counter.flash_erase_block_counter

/* Addtional operation supported exclusively in NOR */
#define fs_flash_write_partial_counter \
                                fs_flash_counter.flash_write_partial_counter
#define fs_flash_erase_suspend_counter   \
                                fs_flash_counter.flash_erase_suspend_counter
#define fs_flash_erase_begin_counter     \
                                fs_flash_counter.flash_erase_begin_counter


/* Type of flash operations. */
enum fs_flashop
{
  FS_FOP_READ,
  FS_FOP_WRITE,
  FS_FOP_ERASE,
  FS_FOP_WRITE_PARTIAL,
  FS_FOP_ERASE_BEGIN,
  FS_FOP_ERASE_SUSPEND,
  FS_NUM_FOP_OPS
};

/* Types of flash categorizations. */
enum fs_flashop_category
{
  FS_FOP_SUPER,
  FS_FOP_USER,
  FS_FOP_LOG,
  FS_FOP_DATA_GC_MOVE,
  FS_FOP_DATA_GC_RECOVERY,
  FS_FOP_USER_RECOVERY,
  FS_FOP_PTABLE_GC_MOVE,
  FS_FOP_PTABLE,
  FS_FOP_FACTORY_RECOVERY,
  FS_FOP_UNKNOWN,
  FS_FOP_BIRDVIEW,
  NUM_FS_FLASHOP_CATEGORIES
};

/* Types of buffer operations. */
enum fs_bufop_category {
  FS_BOP_DB,
  FS_BOP_FS,
  FS_BOP_ALLOC_TREE,
  FS_BOP_ALLOC_MAP,
  FS_BOP_UNKNOWN,
  NUM_FS_BUFOP_CATEGORIES
};

/* Read, Write, Erase Counters */
struct fs_flash_counters
{
  uint32 flash_write_page_counter;
  uint32 flash_write_partial_counter;
  uint32 flash_read_page_counter;
  uint32 flash_erase_block_counter;
  uint32 flash_erase_begin_counter;
  uint32 flash_erase_suspend_counter;
};

struct fs_time_counter
{
  uint32 min_time;
  uint32 max_time;
  uint32 total_time;

};

/* Read, Write, Erase time counters */
struct fs_flash_time_counters
{
  struct fs_time_counter read_timings;
  struct fs_time_counter write_timings;
  struct fs_time_counter erase_block_timings;
  struct fs_time_counter erase_suspend_timings;
};


/* There exists one global counter set that is updated by EFS2 & Drivers. */
extern struct fs_flash_counters fs_flash_counter;
extern struct fs_flash_time_counters fs_flash_time_counter;

extern uint32 fs_flash_pcache_miss_counter;
extern uint32 fs_flash_pcache_hit_counter;
extern uint32 fs_flash_buffer_miss_counter;
extern uint32 fs_flash_buffer_hit_counter;
extern uint32 fs_flash_dealloc_counter;
extern uint32 fs_flash_bird_view_miss_counter;
extern uint32 fs_flash_bird_view_hit_counter;


void fs_counters_init (void);

void fs_counters_update (enum fs_flashop op, enum fs_flashop_category category,
                         uint32 count, uint32 time);

void fs_counters_take_snaphot (struct fs_flash_counters *snapshot);

void fs_device_buffer_fill (enum fs_bufop_category category);

void fs_counters_reset_flash_counters (void);

static inline void
counters_success_stub (enum fs_flashop_category category, uint32 count)
{
  (void) category; (void) count;
}


#ifdef FS_UNIT_TEST
/* Off target, gather statistics on the statistics.  The statistics should
 * be sampled at fixed intervals, and the sample code wants to know the
 * number of user-level operations requested.  The statistics will return
 * useful information about each operation as a factor of how many
 * operations were made per user-level operation.
 * The counters should not be reset/modified while gathering statistics.
 * The reset stats call will zero all of the counters.
 */
void fs_device_reset_stats (void);
void fs_device_take_sample (unsigned user_ops);

/* The result of a single statistic. */
struct stat_result {
  const char *name;
  double min_factor;
  double max_factor;
  double avg_factor;
};

/* How many statistics do we keep? */
unsigned fs_device_number_stats (void);

/* Return the current statistics in slot
 * (0 <= slot < fs_device_number_stats()) */
struct stat_result fs_device_get_stat (unsigned slot);

#endif /* FS_UNIT_TEST */





#endif /* not __FS_COUNTERS_H__ */
